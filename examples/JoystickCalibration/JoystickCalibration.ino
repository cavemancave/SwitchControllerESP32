/*
Copyright (c) 2023 sorasen2020
released under the MIT license
https://opensource.org/licenses/mit-license.php
*/

/*
Nintendo Switch Joystick Calibration Test

Function: tiltJoystick(lx, ly, rx, ry, hold_ms, delay_after_ms)
Parameters:
- lx: Left stick X-axis (-100 to +100, right is positive)
- ly: Left stick Y-axis (-100 to +100, down is positive)
- rx, ry: Right stick (not used in this test)
- hold_ms: How long to hold the position
- delay_after_ms: Delay before next movement (0 = no return to center)

Test Sequence:
1. Eight-Direction Test:
   - Tests 8 directions with full tilt
   - Order: Down → Down-Right → Right → Right-Up → Up → Up-Left → Left → Left-Down
   - Holds each direction for 3 seconds
   - 2 second pause between directions

2. Circular Test:
   - Draws circles with increasing radius
   - Starts at radius 20, increases by 20 up to 100
   - 8-degree steps for smooth movement
   - Brief pause between circles

Setup:
1. Set Nintendo Switch to Calibrate Control Sticks mode
   Home -> System Settings -> Controllers and Sensors -> Calibrate Control Sticks
2. Connect ESP32 to USB port
3. Watch Serial Monitor (115200 baud) for debug output
4. Observe stick movement on Switch screen
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
  // Serial for debug output
  Serial.begin(115200);
  delay(100);
  Serial.println("AutoJoystick test starting");
}

void directionTest() {
  // --- First test: eight cardinal/intercardinal directions with full tilt (100/0/-100)
  // Order: Down, Down-Right, Right, Right-Up, Up, Up-Left, Left, Left-Down
  const char* dirStrs[8] = {
    "Down", "Down-Right", "Right", "Right-Up", "Up", "Up-Left", "Left", "Left-Down"
  };
  const int dirHoldMs = 3000;
  const int dirStepDelay = 2000;
  int dirs[8][2] = {
    {0, 100},    // Down (y=100)
    {100, 100},  // Down-Right (x=100,y=100)
    {100, 0},    // Right (x=100)
    {100, -100}, // Right-Up (x=100,y=-100)
    {0, -100},   // Up (y=-100)
    {-100, -100},// Up-Left (x=-100,y=-100)
    {-100, 0},   // Left (x=-100)
    {-100, 100}  // Left-Down (x=-100,y=100)
  };

  Serial.println("Beginning 8-direction test...");
  for (int i = 0; i < 8; ++i) {
    int lx_dir = dirs[i][0];
    int ly_dir = dirs[i][1];
    Serial.print("Dir "); Serial.print(dirStrs[i]);
    Serial.print(" -> lx="); Serial.print(lx_dir);
    Serial.print(", ly="); Serial.println(ly_dir);

    // call tiltJoystick with full tilt values
    tiltJoystick(lx_dir, ly_dir, 0, 0, dirHoldMs, dirStepDelay);
  }
  Serial.println("8-direction test complete.");
}

void loop() {
  directionTest();

  // left stick circular test
  // radius increases by 20 each outer loop iteration (20, 40, 60, 80, 100)
  // tiltJoystick parameters: lx_per, ly_per, rx_per, ry_per, holdMs, stepDelayMs
  const int maxRadius = 100;
  const int radiusStep = 20;
  const int angleStepDeg = 8; // smaller = smoother circle
  const int holdMs = 100;     // how long to hold each tilt position
  const int stepDelayMs = 20; // small delay between positions (also passed to tiltJoystick)



  for (int radius = radiusStep; radius <= maxRadius; radius += radiusStep) {
    // run one full circle
    Serial.print("Starting circle with radius=");
    Serial.println(radius);
    for (int deg = 0; deg < 360; deg += angleStepDeg) {
      float rad = deg * (PI / 180.0);
      int lx = (int)(radius * cos(rad));
      int ly = (int)(radius * sin(rad));

      // first two args are lx_per and ly_per as requested
      tiltJoystick(lx, ly, 0, 0, holdMs, 0);

      Serial.print("deg="); Serial.print(deg);
      Serial.print(", lx="); Serial.print(lx);
      Serial.print(", ly="); Serial.println(ly);

      // small safe delay to allow the device to process input
      delay(stepDelayMs);
    }

    // pause a bit between circles so you can observe radius change
    Serial.println("--- circle complete ---");
    delay(600);
  }

  // after finishing all radii, wait and repeat
  Serial.println("All radii complete, repeating...");
  delay(1200);
}