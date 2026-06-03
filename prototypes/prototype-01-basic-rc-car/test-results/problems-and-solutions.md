# Problems and Solutions - Prototype 01

## 1. Motor Noise Problem

### Problem
The motors caused electrical noise during operation.  
This affected the stability of the system and sometimes caused unexpected behavior.

### Possible Causes
- Motor power cables were close to signal cables.
- Grounding was not clean enough.
- Power distribution was not separated properly.
- Motor driver switching noise affected the control system.

### Planned Solutions
- Twist motor power cables.
- Keep motor cables away from signal cables.
- Use better cable routing.
- Add capacitors near power lines.
- Improve common ground connection.
- Separate power and control sections in Prototype 02.

---

## 2. Power Distribution Problem

### Problem
The first prototype power system was functional but not clean enough for a modular robot.

### Possible Causes
- Multiple modules were powered from the same 5V line.
- Current demand changed during motor operation.
- Voltage drops could affect control electronics.

### Planned Solutions
- Use a better power distribution board.
- Separate motor power and logic power.
- Use suitable buck converters for Raspberry Pi and STM32.
- Add protection and filtering components.

---

## 3. Mechanical Modularity Problem

### Problem
The chassis was not modular enough.  
Component replacement and debugging were difficult.

### Planned Solutions
- Design removable mechanical sections.
- Use separate mounting plates.
- Add cable channels.
- Add sensor mounting locations.
- Make the second prototype easier to assemble and repair.

---

## 4. Cable Management Problem

### Problem
Cable routing was messy and made debugging difficult.

### Planned Solutions
- Use shorter cables.
- Label all cables.
- Use JST connectors where possible.
- Separate power cables and signal cables.
- Route motor cables away from STM32 and ESP32 signal lines.
