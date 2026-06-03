#include "BluetoothSerial.h"   // Bluetooth library for ESP32

BluetoothSerial SerialBT;      // Bluetooth serial object

const int VRX_PIN = 34;        // Joystick X axis analog pin
const int VRY_PIN = 35;        // Joystick Y axis analog pin

void setup() {
  Serial.begin(115200);        // Start serial monitor communication

  if (!SerialBT.begin("KUMANDA")) {       // Start Bluetooth with device name KUMANDA
    Serial.println("Bluetooth baslatilamadi!");
  } else {
    Serial.println("Bluetooth basladi. Cihaz adi: KUMANDA");
  }
}

void loop() {
  int xValue = analogRead(VRX_PIN);       // Read joystick X value
  int yValue = analogRead(VRY_PIN);       // Read joystick Y value

  Serial.print(xValue);                   // Print X value to serial monitor
  Serial.print(",");
  Serial.println(yValue);                 // Print Y value to serial monitor

  SerialBT.print(xValue);                 // Send X value over Bluetooth
  SerialBT.print(",");
  SerialBT.println(yValue);               // Send Y value over Bluetooth

  delay(100);                             // Send data every 100 ms
}