# Architecture

## The split

Two microcontrollers, one radio link, one UART. The division of labour is the
central decision in the design, so it is worth stating why.

```
  controller ESP32 ──BT SPP──► vehicle ESP32 ──UART──► STM32F407 ──PWM──► BTS7960B ──► motors
   joystick ADC                 forwards only          all control logic
```

**The ESP32 has the radio. The STM32 has the timers.**

An ESP32 could drive the motors by itself — it has PWM and plenty of speed. It
was not given the job because the Bluetooth stack runs on the same chip, and
the stack does not guarantee when it yields. A reconnection attempt, a scan, a
stalled `connect()` — any of those can hold the CPU long enough to matter when
the output is a PWM duty cycle feeding a 43 A H-bridge.

Separating them gives a property worth having: **a radio fault cannot become a
motor fault.** The worst a Bluetooth stall can do is stop new commands from
arriving, and the STM32's last commanded speed stays until the next line comes.

### The vehicle ESP32 does not parse

It reads a line and writes it to the UART. It does not interpret the numbers,
does not clamp them, does not decide anything. That is deliberate: two places
that both understand the protocol is two places that can disagree about it.
The STM32 is the only component that knows what `"2048,1900"` means.

## Protocol

Plain ASCII, newline-terminated, 10 Hz:

```
<x>,<y>\n        e.g.  2048,1900
```

Raw 12-bit ADC readings from the joystick, sent as text. Not a binary struct —
text survives a half-received line visibly (the receiver just fails to parse
and drops it) where a binary frame would silently misalign. At 10 Hz and two
small integers, the bandwidth cost is irrelevant.

There is no acknowledgement and no sequence number. A dropped packet means one
missed update at 100 ms resolution, which the next packet corrects.

## Motor control

On the STM32, each received pair becomes two independent motor speeds.

```
raw ADC (0..4095)
   │
   ├─ centre:    value - ADC_CENTER
   ├─ deadzone:  |centered| < DEADZONE  →  0
   ├─ scale:     → ±PWM_MAX (4200)
   └─ clamp:     limit_pwm() → ±PWM_MAX
   │
   ▼
TIM3 CH1/CH2 (left)   CH3/CH4 (right)
```

Three details that matter:

**The deadzone** exists because an analog joystick at rest does not read
exactly centre. Without it the robot creeps whenever it is powered on and
nobody is touching the controller.

**`limit_pwm()` clamps after scaling**, not before. A corrupted line that
parses to an absurd number cannot produce an out-of-range compare value.

**The opposite channel is driven to zero, not left alone.** Each motor uses two
TIM3 channels — one per direction. When commanding forward, the reverse channel
is explicitly set to 0. Leaving it at its previous value would assert both
sides of the H-bridge, which is a shoot-through path.

## Differential drive

There is no steered axle. The two motors take independent speeds, so:

| Left | Right | Result |
|---|---|---|
| +v | +v | forward |
| −v | −v | reverse |
| +v | −v | rotate in place |
| +v | +v/2 | gentle arc |

Rotating in place matters more than top speed for the intended environment.

## Distance sensing

The HC-SR04 needs microsecond timing on the echo pulse. `TIM2` runs as a free
counter for that; `PA1` triggers. Measurement only — prototype 01 does not act
on the reading, it validates that the timing works alongside the PWM
generation without either disturbing the other.

## What is generated and what is written

In `firmware/stm32/`:

- **`Controller_Motor.ioc`** — the STM32CubeMX project. The source of truth for
  pin assignment, clock configuration and peripheral setup.
- **`Core/`** — mixed. Peripheral init functions (`MX_*_Init`), the interrupt
  vector table and the startup assembly are CubeMX generated. The control logic
  lives between `USER CODE BEGIN` / `USER CODE END` markers, which regeneration
  preserves.
- **`Drivers/`** — entirely vendored: ST's HAL and ARM's CMSIS. Not written
  here, not modified here. See [`../NOTICE`](../NOTICE).

Change pins or peripherals in the `.ioc` and regenerate. Editing generated
sections by hand works until the next regeneration overwrites it.
