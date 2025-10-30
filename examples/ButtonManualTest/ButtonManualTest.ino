/*
Copyright (c) 2023 sorasen2020
released under the MIT license
https://opensource.org/licenses/mit-license.php
*/

/*
Nintendo Switch Controller Manual Button Test

This example allows manual control of any button or hat direction through Serial commands.
Including HOME and CAPTURE buttons that can't be tested in Test Controller mode.

Setup:
1. Connect ESP32 USB port to Nintendo Switch
2. Open Serial Monitor (115200 baud)
3. Send commands to test specific buttons or hat directions
4. Type "help" to see all available commands

Commands:
- Send full name or shortcut to press a button (e.g., "A" or "a")
- All hat direction shortcuts start with 'd' (e.g., "du" for UP, "dr" for RIGHT)
- Type "help" to see complete command list

*/

#ifndef ARDUINO_USB_MODE
#error This sketch should be used when USB is in OTG mode
void setup(){}
void loop(){}
#else
#include "SwitchControllerESP32.h"
#include "Arduino.h"
#endif /* ARDUINO_USB_MODE */

// Manual test buttons (including HOME and CAPTURE)
const struct {
  const char* name;
  const char* shortcut;
  Button button;
} buttonTests[] = {
  {"Y", "y", Button::Y},
  {"B", "b", Button::B},
  {"A", "a", Button::A},
  {"X", "x", Button::X},
  {"L", "l", Button::L},
  {"R", "r", Button::R},
  {"ZL", "zl", Button::ZL},
  {"ZR", "zr", Button::ZR},
  {"MINUS", "-", Button::MINUS},
  {"PLUS", "+", Button::PLUS},
  {"LCLICK", "lc", Button::LCLICK},
  {"RCLICK", "rc", Button::RCLICK},
  {"HOME", "h", Button::HOME},
  {"CAPTURE", "c", Button::CAPTURE}
};

const struct {
  const char* name;
  const char* shortcut;
  Hat direction;
} hatTests[] = {
  {"UP", "du", Hat::UP},           // d prefix for D-pad
  {"UP_RIGHT", "dur", Hat::UP_RIGHT},
  {"RIGHT", "dr", Hat::RIGHT},
  {"RIGHT_DOWN", "drd", Hat::RIGHT_DOWN},
  {"DOWN", "dd", Hat::DOWN},
  {"DOWN_LEFT", "ddl", Hat::DOWN_LEFT},
  {"LEFT", "dl", Hat::LEFT},
  {"LEFT_UP", "dlu", Hat::LEFT_UP},
  {"CENTER", "dc", Hat::CENTER}
};

const int NUM_BUTTON_TESTS = sizeof(buttonTests) / sizeof(buttonTests[0]);
const int NUM_HAT_TESTS = sizeof(hatTests) / sizeof(hatTests[0]);

void setup() {
  switchcontrolleresp32_init();
  USB.begin();
  switchcontrolleresp32_reset();
  
  // Initialize Serial communication
  Serial.begin(115200);
  delay(100);
  Serial.println("\nNintendo Switch Controller Manual Tester");
  Serial.println("=======================================");
  printHelp();
}

void printHelp() {
  Serial.println("\n=== Manual Mode Commands ===");
  Serial.println("Buttons:");
  for (int i = 0; i < NUM_BUTTON_TESTS; i++) {
    Serial.print("  ");
    Serial.print(buttonTests[i].name);
    Serial.print(" or ");
    Serial.println(buttonTests[i].shortcut);
  }
  Serial.println("\nD-pad Directions (all shortcuts start with 'd'):");
  for (int i = 0; i < NUM_HAT_TESTS; i++) {
    Serial.print("  ");
    Serial.print(hatTests[i].name);
    Serial.print(" or ");
    Serial.println(hatTests[i].shortcut);
  }
  Serial.println("\nOther Commands:");
  Serial.println("  help   - Show this help message");
  Serial.println("\nReady for commands:");
}

void handleSerialInput() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();  // Remove any whitespace or newlines
    
    if (cmd == "help") {
      printHelp();
    }
    else {
      // Try to match button or hat commands
      bool found = false;
      
      // Check normal buttons
      for (int i = 0; i < NUM_BUTTON_TESTS; i++) {
        if (cmd == buttonTests[i].name || cmd == buttonTests[i].shortcut) {
          Serial.print("Pressing button: ");
          Serial.println(buttonTests[i].name);
          pushButton(buttonTests[i].button, 1000, 1);
          found = true;
          break;
        }
      }
      
      // Check hat directions
      if (!found) {
        for (int i = 0; i < NUM_HAT_TESTS; i++) {
          if (cmd == hatTests[i].name || cmd == hatTests[i].shortcut) {
            Serial.print("Pressing hat direction: ");
            Serial.println(hatTests[i].name);
            pushHatButton(hatTests[i].direction, 1000, 1);
            found = true;
            break;
          }
        }
      }
      
      if (!found) {
        Serial.println("Unknown command. Type 'help' to see available commands.");
      }
    }
    
    // Clear any remaining characters
    while (Serial.available()) Serial.read();
  }
}

void loop() {
  handleSerialInput();
}