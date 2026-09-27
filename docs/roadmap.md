# Roadmap

Written down rather than kept as empty directories, so the repository shows
what exists instead of implying capabilities that do not.

## Prototype 01 — basic RC car ✅

Answered: can it move, be driven wirelessly, and power itself under motor load?

Delivered: differential drive over Bluetooth, STM32 PWM motor control, a
hand-built 3S2P pack with BMS, a working 5 V logic rail.

Surfaced four problems, which became the requirements below. See
[`problems-and-solutions.md`](problems-and-solutions.md).

## Prototype 02 — a serviceable platform (planned)

Everything later depends on a platform that is electrically quiet and can be
opened without disassembly, so that comes before any new capability.

- Modular chassis — electronics reachable without partial teardown
- Separated power and signal routing; twisted motor pairs; decoupling at the
  drivers; a clean common ground
- A captured schematic instead of point-to-point wiring
- Room and mounting for sensors not yet fitted
- Battery placement chosen for weight distribution

## Later — perception and autonomy

Deliberately vague, because prototype 02 will change the answers.

- **Perception:** camera, IMU, more distance sensing than one HC-SR04
- **Compute:** a Raspberry Pi class board alongside the STM32 for anything
  heavier than motor control
- **Autonomy:** obstacle avoidance first, then mapping
- **Payload:** the actual rescue function — two-way audio, environmental
  sensing, a light

## Not planned

- A steered axle. Differential drive is simpler and turns in place, which
  matters more in rubble than top speed.
- Moving motor control off the STM32. Splitting radio from real-time control
  is the design, not an accident.
