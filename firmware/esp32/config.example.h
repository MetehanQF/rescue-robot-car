/*
 * Board-specific configuration for the ESP32 firmware.
 *
 * Copy this file to config.h and fill in the values for YOUR hardware:
 *
 *     cp config.example.h config.h
 *
 * config.h is gitignored. It is kept out of the repository because the
 * controller's Bluetooth MAC address identifies one specific physical board —
 * it is useless to anyone else, and publishing a device address is not
 * something to do by default.
 *
 * Finding your controller's MAC: flash the controller sketch and read the
 * serial monitor at 115200 baud, or call WiFi.macAddress() on it. The
 * Bluetooth address is normally the WiFi address with the last byte + 2;
 * printing it directly from the controller is the reliable way.
 *
 * The Arduino IDE compiles every .ino in a sketch folder together, so place
 * config.h next to the sketch you are building (or symlink it).
 */

#ifndef RESCUE_ROBOT_CONFIG_H
#define RESCUE_ROBOT_CONFIG_H

/* Bluetooth MAC of the handheld controller, which the vehicle connects to.
 * Replace every byte with your own controller's address. */
#define CONTROLLER_BT_ADDRESS { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }

/* Bluetooth device name the vehicle advertises. */
#define VEHICLE_BT_NAME "ARAC"

/* UART link to the STM32 motor controller.
 * These are the ESP32 pins wired to the STM32's USART RX/TX. */
#define STM32_UART_BAUD 115200
#define STM32_UART_RX_PIN 16
#define STM32_UART_TX_PIN 17

/* USB serial console baud rate. */
#define CONSOLE_BAUD 115200

#endif /* RESCUE_ROBOT_CONFIG_H */
