# Hardware

Electronics and power for the rescue robot.

## Contents

| Path | What it is |
|---|---|
| [`bom.md`](bom.md) | bill of materials for the current prototype |
| [`battery-pack/`](battery-pack/) | the hand-built 3S2P 18650 pack: specification, wiring diagram and build photos |

## Power architecture

```
  3S2P 18650 pack  ──► XT60 ──┬──────────────────► BTS7960B motor drivers (direct, 11.1 V)
  11.1 V / 5200 mAh           │
  behind a 3S 40 A BMS        └──► LM2596 buck ──► 5 V logic rail
                                                   STM32, ESP32, HC-SR04
```

Motors draw from the pack directly; logic runs off a regulated 5 V rail. Both
share a ground — necessary for the UART and PWM signals to have a reference,
and the reason motor switching noise reached the control electronics in
prototype 01. See [`../docs/problems-and-solutions.md`](../docs/problems-and-solutions.md).

## Not here yet

No schematic capture or PCB design exists: prototype 01 was wired point to
point on the chassis. A proper schematic is planned for prototype 02, together
with the noise and cable-routing fixes.
