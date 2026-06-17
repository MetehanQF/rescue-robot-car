#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

uint8_t kumandaAddress[6] = {0x1C, 0x69, 0x20, 0xA3, 0xE7, 0x1E};

void setup() {

  Serial.begin(115200);

  Serial2.begin(115200, SERIAL_8N1, 16, 17);
  /* Serial2 is used for UART communication with STM32.
     GPIO16 = RX
     GPIO17 = TX */

  SerialBT.begin("ARAC", true);

  Serial.println("KUMANDA MAC adresi ile baglaniliyor...");

  while (!SerialBT.connect(kumandaAddress)) {

    Serial.println("Baglanti bekleniyor...");
    delay(1000);
  }

  Serial.println("KUMANDA baglandi!");
}

void loop() {

  if (SerialBT.available()) {

    String data = SerialBT.readStringUntil('\n');
    data.trim();

    if (data.length() > 0) {

      Serial.print("Gelen veri: ");
      Serial.println(data);

      Serial2.print(data);
      Serial2.print('\n');
      /* Joystick data is sent to STM32 through UART */
    }
  }
}