# SwitchControllerESP32

This is a microcontroller library for automating Nintendo Switch games using the ESP32-S3-DevkitC.

# About this Library

This library was created with reference to [NintendoSwitchControll](https://github.com/interimadd/NintendoSwitchControll) by interimadd.

# Installation

Required items:

    1x ESP32-S3-DevkitC-1 (https://akizukidenshi.com/catalog/g/gM-17073/)
    2x USB cables (required to connect ESP32-S3-DevkitC-1 to Switch and PC)

Operating Environment:

    Arduino IDE (1.8.19 recommended)
    Arduino core for ESP32 (2.0.6)

参考URL:https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html#installing-using-arduino-ide

# Usage

## Library Installation

Download and extract this repository to your Arduino libraries folder (e.g., C:/Users/username/Documents/Arduino/libraries)

## Upload

1. Connect the ESP32 S3 DevkitC-1's UART-labeled USB port to PC and the USB-labeled port to Switch
2. Launch Arduino IDE
3. Select Tools → Board → ESP32 Arduino → ESP32S3 Dev Module
4. Change the serial port to the connected port
5. Open the sample sketch (File → Examples → SwitchControllerESP32 → AutoButton)
6. Click Sketch → Upload

The sample rapidly presses the A button, so you can test it in front of an NPC at a Pokemon Center, for example.

# Important Note

While the main functions can be used in the same way as the original library, please note that you must execute the initialization functions before use:

    --setup functions--
    switchcontrolleresp32_init();  ← Required
    USB.begin();                   ← Required
    switchcontrolleresp32_reset(); ← Recommended

# License

This project is licensed under the MIT License. See the LICENSE file for details.
