/*
Copyright (c) 2023 sorasen2020
released under the MIT license
https://opensource.org/licenses/mit-license.php
*/

/*
Nintendo Switch Controller Raw Binary Adapter

This example provides the most direct control possible over the Nintendo Switch controller
by accepting raw USB_JoystickReport_Input_t data (8 bytes) over serial and sending it
directly to the Switch using sendReportOnly().

Perfect for:
- Python script control with precise timing
- Raw HID report manipulation
- Direct binary protocol communication
- Custom controller emulation

Data Format (8 bytes total):
Byte 0-1: Button states (uint16_t, little-endian)
Byte 2:   Hat direction (uint8_t)
Byte 3:   Left stick X (uint8_t, 0x00=left, 0x80=center, 0xFF=right)
Byte 4:   Left stick Y (uint8_t, 0x00=down, 0x80=center, 0xFF=up)
Byte 5:   Right stick X (uint8_t, 0x00=left, 0x80=center, 0xFF=right) 
Byte 6:   Right stick Y (uint8_t, 0x00=down, 0x80=center, 0xFF=up)
Byte 7:   Dummy/Padding (uint8_t, usually 0x00)

Button Bits (uint16_t):
0x0001 = Y       0x0010 = L        0x0100 = MINUS    0x1000 = HOME
0x0002 = B       0x0020 = R        0x0200 = PLUS     0x2000 = CAPTURE  
0x0004 = A       0x0040 = ZL       0x0400 = LCLICK
0x0008 = X       0x0080 = ZR       0x0800 = RCLICK

Hat Values (uint8_t):
0x00 = UP        0x02 = RIGHT      0x04 = DOWN       0x06 = LEFT
0x01 = UP_RIGHT  0x03 = RIGHT_DOWN 0x05 = DOWN_LEFT  0x07 = LEFT_UP
0x08 = CENTER

Setup:
1. Connect ESP32 USB port to Nintendo Switch
2. Send 8-byte binary data over serial (115200 baud, no line ending)
3. Each 8-byte packet is immediately sent to the Switch

Python Example:
import serial
ser = serial.Serial('/dev/ttyUSB0', 115200)

# Press A button (0x0004) with centered sticks
report = bytes([0x04, 0x00, 0x08, 0x80, 0x80, 0x80, 0x80, 0x00])
ser.write(report)

# Release all (neutral state)
report = bytes([0x00, 0x00, 0x08, 0x80, 0x80, 0x80, 0x80, 0x00])
ser.write(report)

*/

#ifndef ARDUINO_USB_MODE
#error This sketch should be used when USB is in OTG mode
void setup(){}
void loop(){}
#else
#include "SwitchControllerESP32.h"
#include "Arduino.h"
#endif /* ARDUINO_USB_MODE */

void setup() {
  switchcontrolleresp32_init();
  USB.begin();
  switchcontrolleresp32_reset();
  
  // Initialize Serial communication
  Serial.begin(115200);
  delay(100);
  Serial.println("\nNintendo Switch Controller Raw Binary Adapter");
  Serial.println("=============================================");
  Serial.println("Ready to receive 8-byte HID reports over serial");
  Serial.println("Send raw USB_JoystickReport_Input_t data (no text commands)");
  Serial.println();
  Serial.println("Data format: [Button_Low, Button_High, Hat, LX, LY, RX, RY, Dummy]");
  Serial.println("Example - Press A: 0x04 0x00 0x08 0x80 0x80 0x80 0x80 0x00");
  Serial.println("Example - Neutral: 0x00 0x00 0x08 0x80 0x80 0x80 0x80 0x00");
  Serial.println("Waiting for binary data...");
}

void loop() {
  // Check if we have exactly 8 bytes available
  if (Serial.available() >= 8) {
    USB_JoystickReport_Input_t report;
    uint8_t buffer[8];
    
    // Read exactly 8 bytes
    Serial.readBytes(buffer, 8);
    
    // Parse the bytes into the report structure
    // Button is uint16_t (little-endian: low byte first, then high byte)
    report.Button = (uint16_t)buffer[0] | ((uint16_t)buffer[1] << 8);
    report.Hat = buffer[2];
    report.LX = buffer[3];
    report.LY = buffer[4];
    report.RX = buffer[5];
    report.RY = buffer[6];
    report.Dummy = buffer[7];
    
    // Send the report directly to the Switch
    sendReportOnly(report);
    
    // Optional: Echo received data for debugging (comment out for production)
    /*
    Serial.print("Sent: ");
    for (int i = 0; i < 8; i++) {
      Serial.print("0x");
      if (buffer[i] < 0x10) Serial.print("0");
      Serial.print(buffer[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
    */
  }
}