# Rescue Robot Car

A remote-operated rescue robot built from the ground up — mechanics, power,
firmware and radio link — developed one prototype at a time, with each
prototype's failures written down and carried into the next.

**Current status: prototype 01 complete.** A working remote-controlled vehicle
with differential drive, a Bluetooth link and a hand-built battery pack. It
drives; it is not yet autonomous, and it is not yet a rescue robot. What it is
is a validated base — power, motor control and communication all work under
load, and the problems that surfaced are documented rather than patched over.

---

## Project goal

A robot that can be driven into a place a person should not go — rubble, smoke,
a collapsed structure — carry a camera and sensors, and eventually navigate on
its own.

That is a long way off. The approach is to build it in prototypes where each
one answers a specific question, and to keep the answers.

| Prototype | Question it answers | Status |
|---|---|---|
| **01 — basic RC car** | Can it move, be controlled wirelessly, and power itself under motor load? | ✅ complete |
| 02 | Can the chassis be modular and the wiring survive motor noise? | planned |
| 03+ | Perception, autonomy, payload | planned |

See [`docs/roadmap.md`](docs/roadmap.md).

---

## System architecture

Control is split across two microcontrollers, deliberately.

```
  ┌──────────────────────────┐        Bluetooth SPP         ┌──────────────────────────┐
  │  Handheld controller     │ ───────────────────────────► │  Vehicle ESP32           │
  │  ESP32 + analog joystick │      "xValue,yValue\n"       │  radio bridge only       │
  └──────────────────────────┘          @ 10 Hz             └────────────┬─────────────┘
                                                                          │ UART 115200 8N1
                                                                          ▼
                                                            ┌──────────────────────────┐
                                                            │  STM32F407 Discovery     │
                                                            │  motion control          │
                                                            │  TIM3 → 4 PWM channels   │
                                                            └────────────┬─────────────┘
                                                                          │
                                                      ┌───────────────────┴───────────────────┐
                                                      ▼                                       ▼
                                             BTS7960B driver                         BTS7960B driver
                                              left DC motor                           right DC motor
```

### Why two microcontrollers

The ESP32 has the radio; the STM32 has the timers and a deterministic loop.
Splitting them means a Bluetooth stall cannot delay a PWM update. The vehicle
ESP32 stays deliberately dumb — it parses nothing and decides nothing, it only
forwards command lines — so a radio fault cannot become a motor fault.

### Roles

**Controller ESP32** — reads two joystick axes on ADC pins `GPIO34` / `GPIO35`
and sends `x,y\n` as plain text over Bluetooth Serial every 100 ms.

**Vehicle ESP32** — Bluetooth master. Connects to the controller by MAC
address, then forwards each received line verbatim to the STM32 over `Serial2`
(`GPIO16` RX, `GPIO17` TX) at 115200 baud.

**STM32F407** — receives the joystick pair on `USART2` and converts it to motor
commands:

- `joystick_to_pwm()` centres the raw ADC reading, applies a **deadzone** so a
  resting joystick means stop rather than crawl, and scales to `PWM_MAX = 4200`
- `limit_pwm()` clamps to ±`PWM_MAX`, so a malformed packet cannot command an
  out-of-range duty cycle
- `TIM3` channels 1–4 produce forward and reverse PWM for the two motors. The
  opposite channel is actively driven to 0 rather than left alone, so the
  H-bridge never sees both directions asserted at once
- `TIM2` provides a microsecond counter used to time the HC-SR04 echo
- Differential drive: each motor gets an independent speed, so steering is a
  speed difference rather than a steered axle

---

## Hardware

Full parts list: [`hardware/bom.md`](hardware/bom.md).

| Subsystem | Part | Notes |
|---|---|---|
| Motion control | STM32F407 Discovery | 4× PWM on TIM3, UART on USART2 |
| Radio | 2× ESP32 DevKit V1 | controller and vehicle |
| Motor driving | 2× BTS7960B | half-bridge pairs, one per motor |
| Drive | 2× DC motor | rear wheels, differential |
| Distance | HC-SR04 | forward obstacle sensing |
| Power | 3S2P 18650 pack | hand-built — see below |
| Regulation | LM2596 buck converter | pack voltage → 5 V logic rail |
| Connector | XT60 | main battery connection |

### Battery system

A hand-assembled **3S2P pack of six Samsung 25R 18650 cells** — 11.1 V nominal,
12.6 V full, 5200 mAh — behind a 3S 40 A BMS providing overcharge,
over-discharge and short-circuit protection.

Build details and photos: [`hardware/battery-pack/`](hardware/battery-pack/).

The pack feeds the motor drivers directly and the logic rail through the
LM2596. Motors and electronics share a ground, which turned out to matter —
see the noise problem below.

---

## Repository layout

```
docs/          architecture notes, roadmap, prototype 01 post-mortem
hardware/      bill of materials and the battery pack build
firmware/
  esp32/       controller and vehicle sketches, plus config.example.h
  stm32/       STM32CubeIDE project (Core/ is ours, Drivers/ is ST's)
prototypes/    per-prototype record: what was built, what broke, what changed
```

---

## Building and flashing

### ESP32 — both sketches

Requires the Arduino IDE with the **ESP32 board package**.

```bash
cp firmware/esp32/config.example.h firmware/esp32/vehicle/config.h
$EDITOR firmware/esp32/vehicle/config.h      # set CONTROLLER_BT_ADDRESS
```

`config.h` is gitignored because it holds the controller's Bluetooth MAC, which
identifies one specific board. To find yours: flash the controller sketch first
and read its address from the serial monitor at 115200 baud.

Then open `firmware/esp32/controller/controller.ino` and
`firmware/esp32/vehicle/vehicle.ino`, select your board, and upload each to its
own device.

### STM32

Requires **STM32CubeIDE** (free, from STMicroelectronics).

1. *File → Open Projects from File System…* and select `firmware/stm32/`
2. Build
3. Flash over the Discovery board's built-in ST-LINK

`Controller_Motor.ioc` is the STM32CubeMX configuration. To change pins or
peripherals, edit it in CubeMX/CubeIDE and regenerate — hand-editing generated
code is overwritten on the next generation. Keep your own code inside the
`USER CODE BEGIN` / `USER CODE END` markers, which CubeMX preserves.

### Wiring

| From | To | Notes |
|---|---|---|
| Controller ESP32 `GPIO34` / `GPIO35` | joystick VRX / VRY | analog in |
| Vehicle ESP32 `GPIO16` / `GPIO17` | STM32 `USART2` TX / RX | crossed; 115200 8N1 |
| STM32 `TIM3` CH1–CH4 | BTS7960B RPWM / LPWM ×2 | one channel pair per motor |
| STM32 `PA1` | HC-SR04 trigger | echo timed with TIM2 |
| Battery XT60 | BTS7960B V+ and LM2596 input | common ground with logic |

---

## Known problems and solutions

Prototype 01 worked, and taught four things. Full write-up with causes and
planned fixes: [`docs/problems-and-solutions.md`](docs/problems-and-solutions.md).

1. **Motor electrical noise destabilised the system.** Motor power cables ran
   close to signal cables and the common ground was not clean enough. Planned
   fixes: twist the motor pairs, separate power from signal routing, add
   decoupling near the drivers, improve the ground.
2. **Cable management was inadequate** — hard to trace, hard to service.
3. **The chassis was not modular.** Two levels, but reaching any electronics
   meant partial disassembly, and there was no room for future sensors.
4. **Power distribution needed rework.** Battery placement affected weight
   distribution as well as cable length.

These are the requirements for prototype 02.

---

## License

[MIT](LICENSE) for the code, documentation and designs written for this project.

**This does not cover the vendored STM32 files.** `firmware/stm32/Drivers/`
contains STMicroelectronics HAL and ARM CMSIS sources redistributed under their
own upstream licences, and parts of `firmware/stm32/Core/` are STM32CubeMX
generated code carrying ST copyright headers. Those headers are unmodified and
their terms apply. See [NOTICE](NOTICE).
