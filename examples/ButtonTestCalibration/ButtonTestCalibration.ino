/*
Copyright (c) 2023 sorasen2020
released under the MIT license
https://opensource.org/licenses/mit-license.php
*/

/*
Nintendo Switch Controller Auto Test and Calibration

This example automatically tests controller buttons and hat directions in sequence.
Perfect for calibrating and verifying controller functionality.

Features:
- Automatically tests hat directions first, then normal buttons
- Skips HOME and CAPTURE buttons (not testable in Test Controller mode)
- Tests sequence: Hat directions -> Regular buttons (A,B,X,Y,L,R,etc)
- Serial output shows which button/direction is being tested
- Continuous loop for ongoing calibration

Setup:
1. Set Nintendo Switch to Test Controller Buttons mode
   Home -> System Settings -> Controllers and Sensors -> 
   Test Input Devices -> Test Controller Buttons
2. Connect ESP32 USB port
3. Open Serial Monitor (115200 baud) to see test progress
4. Watch the Switch screen to verify each button press

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
unsigned long lastButtonTime = 0;  // For non-blocking button test sequence
int currentButtonIndex = 0;  // Keep track of which button we're testing

// Auto-test buttons (excluding HOME and CAPTURE which can't be tested in Test mode)
const struct {
  const char* name;
  Button button;
} buttonTests[] = {
  {"Y", Button::Y},
  {"B", Button::B},
  {"A", Button::A},
  {"X", Button::X},
  {"L", Button::L},
  {"R", Button::R},
  {"ZL", Button::ZL},
  {"ZR", Button::ZR},
  {"MINUS", Button::MINUS},
  {"PLUS", Button::PLUS},
  {"LCLICK", Button::LCLICK},
  {"RCLICK", Button::RCLICK}
};

const struct {
  const char* name;
  Hat direction;
} hatTests[] = {
  {"UP", Hat::UP},
  {"UP_RIGHT", Hat::UP_RIGHT},
  {"RIGHT", Hat::RIGHT},
  {"RIGHT_DOWN", Hat::RIGHT_DOWN},
  {"DOWN", Hat::DOWN},
  {"DOWN_LEFT", Hat::DOWN_LEFT},
  {"LEFT", Hat::LEFT},
  {"LEFT_UP", Hat::LEFT_UP},
  {"CENTER", Hat::CENTER}
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
  Serial.println("\nNintendo Switch Controller Auto Test");
  Serial.println("====================================");
  Serial.println("Starting automatic button test sequence...");
  Serial.println("Test order: Hat directions first, then buttons");
  Serial.println("Open Switch Test Controller mode to see results");
}

void loop() {
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
      // Then test normal buttons
      int buttonIndex = currentButtonIndex - NUM_HAT_TESTS;
      Serial.print("Testing button: ");
      Serial.println(buttonTests[buttonIndex].name);
      pushButton(buttonTests[buttonIndex].button, 1000, 1);
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
