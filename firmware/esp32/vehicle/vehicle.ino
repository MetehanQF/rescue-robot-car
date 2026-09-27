/*
 * Vehicle-side ESP32 — the radio bridge.
 *
 * Connects to the handheld controller over Bluetooth Serial, then forwards
 * every received command line to the STM32 over UART. It does not interpret
 * the commands: motor control belongs to the STM32, and keeping this side
 * dumb means a radio problem can never become a motor problem.
 *
 * Copy config.example.h to config.h and set CONTROLLER_BT_ADDRESS before
 * building; the address identifies one specific controller board.
 */

#include "BluetoothSerial.h"
#include "config.h"

BluetoothSerial SerialBT;

uint8_t controllerAddress[6] = CONTROLLER_BT_ADDRESS;

void setup() {

  Serial.begin(CONSOLE_BAUD);

  Serial2.begin(STM32_UART_BAUD, SERIAL_8N1,
                STM32_UART_RX_PIN, STM32_UART_TX_PIN);
  /* Serial2 is the UART link to the STM32. */

  SerialBT.begin(VEHICLE_BT_NAME, true);   /* true = master role */

  Serial.println("Connecting to controller by MAC address...");

  /* Block until the controller is reachable. The vehicle is useless without
     it, so there is nothing sensible to do in the meantime. */
  while (!SerialBT.connect(controllerAddress)) {

    Serial.println("Waiting for connection...");
    delay(1000);
  }

  Serial.println("Controller connected.");
}

void loop() {

  if (SerialBT.available()) {

    String data = SerialBT.readStringUntil('\n');
    data.trim();

    if (data.length() > 0) {

      Serial.print("Received: ");
      Serial.println(data);

      Serial2.print(data);
      Serial2.print('\n');
      /* Joystick data is passed straight through to the STM32. */
    }
  }
}
