/*
Copyright (c) 2023 sorasen2020
released under the MIT license
https://opensource.org/licenses/mit-license.php
*/

/*
Nintendo Switch Controller Test with Auto/Manual Modes

Modes:
1. Auto Mode (default):
   - Automatically tests hat directions first, then normal buttons
   - Skips HOME and CAPTURE buttons (not testable in Test mode)
   - Tests sequence: Hat directions -> Regular buttons (A,B,X,Y,L,R,etc)
   - Any serial input switches to Manual Mode

2. Manual Mode:
   - Control any button/hat through Serial, including HOME/CAPTURE
   - Send full name or shortcut to press a button (e.g., "A" or "a")
   - Type "help" to see all commands
   - Type "auto" to return to Auto Mode

Setup:
1. Set Nintendo Switch to Test Controller Buttons mode
   Home -> System Settings -> Controllers and Sensors -> 
   Test Input Devices -> Test Controller Buttons
2. Connect ESP32 USB port
3. Open Serial Monitor (115200 baud)
4. Commands will be shown in Manual Mode

*/

#ifndef ARDUINO_USB_MODE
#error This sketch should be used when USB is in OTG mode
void setup(){}
void loop(){}
#else
#include "SwitchControllerESP32.h"
#include "Arduino.h"
#endif /* ARDUINO_USB_MODE */

// Global state control
bool isAutoMode = true;     // true = auto test sequence, false = manual button control
unsigned long lastButtonTime = 0;  // For non-blocking button test sequence
int currentButtonIndex = 0;  // Keep track of which button we're testing

// Auto-test buttons (excluding HOME and CAPTURE which can't be tested in Test mode)
const struct {
  const char* name;
  const char* shortcut;
  Button button;
  bool includeInAutoTest;  // Whether to include in auto test sequence
} buttonTests[] = {
  {"Y", "y", Button::Y, true},
  {"B", "b", Button::B, true},
  {"A", "a", Button::A, true},
  {"X", "x", Button::X, true},
  {"L", "l", Button::L, true},
  {"R", "r", Button::R, true},
  {"ZL", "zl", Button::ZL, true},
  {"ZR", "zr", Button::ZR, true},
  {"MINUS", "-", Button::MINUS, true},
  {"PLUS", "+", Button::PLUS, true},
  {"LCLICK", "lc", Button::LCLICK, true},
  {"RCLICK", "rc", Button::RCLICK, true},
  {"HOME", "h", Button::HOME, false},      // Skip in auto test
  {"CAPTURE", "c", Button::CAPTURE, false} // Skip in auto test
};

const struct {
  const char* name;
  const char* shortcut;
  Hat direction;
} hatTests[] = {
  {"UP", "u", Hat::UP},
  {"UP_RIGHT", "ur", Hat::UP_RIGHT},
  {"RIGHT", "r", Hat::RIGHT},
  {"RIGHT_DOWN", "rd", Hat::RIGHT_DOWN},
  {"DOWN", "d", Hat::DOWN},
  {"DOWN_LEFT", "dl", Hat::DOWN_LEFT},
  {"LEFT", "l", Hat::LEFT},
  {"LEFT_UP", "lu", Hat::LEFT_UP},
  {"CENTER", "c", Hat::CENTER}
};

const int NUM_BUTTON_TESTS = sizeof(buttonTests) / sizeof(buttonTests[0]);
const int NUM_HAT_TESTS = sizeof(hatTests) / sizeof(hatTests[0]);
const int TOTAL_TESTS = NUM_BUTTON_TESTS + NUM_HAT_TESTS;


void setup() {
  switchcontrolleresp32_init();
  USB.begin();
  switchcontrolleresp32_reset();
  
  // Initialize Serial communication
  Serial.begin(115200);
  delay(100);
  Serial.println("\nNintendo Switch Controller Tester");
  Serial.println("Starting in Auto Mode...");
  Serial.println("Send any character to enter Manual Mode");
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
  Serial.println("\nHat Directions:");
  for (int i = 0; i < NUM_HAT_TESTS; i++) {
    Serial.print("  ");
    Serial.print(hatTests[i].name);
    Serial.print(" or ");
    Serial.println(hatTests[i].shortcut);
  }
  Serial.println("\nOther Commands:");
  Serial.println("  auto   - Return to Auto Mode");
  Serial.println("  help   - Show this help message");
}

void handleSerialInput() {
  if (Serial.available()) {
    // Any serial input in Auto Mode switches to Manual Mode
    if (isAutoMode) {
      isAutoMode = false;
      Serial.println("\nSwitched to Manual Mode");
      printHelp();
      // Clear the input buffer
      while (Serial.available()) Serial.read();
      return;
    }

    String cmd = Serial.readStringUntil('\n');
    cmd.trim();  // Remove any whitespace or newlines
    
    if (cmd == "auto") {
      isAutoMode = true;
      currentButtonIndex = 0; // Reset the sequence
      Serial.println("Returning to Auto Mode...");
    }
    else if (cmd == "help") {
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
  // Always check for serial commands first
  handleSerialInput();

  // Run automatic test sequence only in Auto Mode
  if (isAutoMode) {
    unsigned long currentTime = millis();
    
    // Wait at least 1000ms between button presses
    if (currentTime - lastButtonTime >= 1000) {
      if (currentButtonIndex < NUM_HAT_TESTS) {
        // Test hat directions first
        Serial.print("Testing hat direction: ");
        Serial.println(hatTests[currentButtonIndex].name);
        pushHatButton(hatTests[currentButtonIndex].direction, 1000, 1);
      }
      else if (currentButtonIndex < (NUM_HAT_TESTS + NUM_BUTTON_TESTS)) {
        // Then test normal buttons (except HOME and CAPTURE)
        int buttonIndex = currentButtonIndex - NUM_HAT_TESTS;
        if (buttonTests[buttonIndex].includeInAutoTest) {
          Serial.print("Testing button: ");
          Serial.println(buttonTests[buttonIndex].name);
          pushButton(buttonTests[buttonIndex].button, 1000, 1);
        }
      }
      
      currentButtonIndex++;
      // Check if we've completed all tests
      if (currentButtonIndex >= (NUM_HAT_TESTS + NUM_BUTTON_TESTS)) {
        currentButtonIndex = 0; // Reset to start with hat directions
        Serial.println("=== Test sequence complete, restarting... ===");
      }
      
      lastButtonTime = currentTime;
    }
  }
}
