/**
  @file    SwitchControllerESP32.cpp
  @author  interimadd,rokkoku
  @Improved by sorasen2020
  @brief   A library that combines commonly used Switch inputs
  @version 0.2
  @date    2023-1-29
*/
#include "SwitchControllerESP32.h"

// Time from pressing to releasing a button
const uint16_t BUTTON_PUSHING_MSEC = 40;

/**
  @brief Initialize the Switch controller
*/
void switchcontrolleresp32_init(void)
{
  _button_pushing_msec = BUTTON_PUSHING_MSEC;
  SwitchController().begin();
}

/**
  @brief Initialize the Switch controller
  @param button_pushing_msec        Time from pressing to releasing a button
*/
void switchcontrolleresp32_init(uint16_t button_pushing_msec)
{
  _button_pushing_msec = button_pushing_msec;
  SwitchController().begin();
}

/**
  @brief Reset all Switch controller inputs
*/
void switchcontrolleresp32_reset(void)
{
  SwitchController().setStickTiltRatio(0,0,0,0);
  SwitchController().releaseHatButton();
}

/**
  @brief Press a Switch controller button

  @param button                   Button to press
  @param delay_after_pushing_msec Delay after releasing the button
  @param loop_num                 Number of times to press the button
*/
void pushButton(Button button, int delay_after_pushing_msec, int loop_num = 1)
{
  for (int i = 0; i < loop_num; i++)
  {
    SwitchController().pressButton(button);
    delay(_button_pushing_msec);
    SwitchController().releaseButton(button);
    delay(delay_after_pushing_msec);
  }
}

/**
  @brief Press a Switch controller button

  @param button                   Button to press
  @param pushing_time_msec        Duration to hold the button
  @param delay_after_pushing_msec Delay after releasing the button
  @param loop_num                 Number of times to press the button
*/
void pushButton2(Button button, int pushing_time_msec, int delay_after_pushing_msec, int loop_num = 1)
{
  for (int i = 0; i < loop_num; i++)
  {
    SwitchController().pressButton(button);
    delay(pushing_time_msec);
    SwitchController().releaseButton(button);
    delay(delay_after_pushing_msec);
  }
}

/**
  @brief Press a D-pad (directional) button

  @param button                   D-pad button to press
  @param delay_after_pushing_msec Delay after releasing the button
  @param loop_num                 Number of times to press the button
*/
void pushHatButton(Hat button, int delay_after_pushing_msec, int loop_num = 1)
{
  for (int i = 0; i < loop_num; i++)
  {
    SwitchController().pressHatButton(button);
    delay(_button_pushing_msec);
    SwitchController().releaseHatButton();
    delay(delay_after_pushing_msec);
  }
}

/**
  @brief Hold a D-pad button for a specified duration

  @param button            D-pad button to press
  @param pushing_time_msec Duration to hold the button
*/
void pushHatButtonContinuous(Hat button, int pushing_time_msec)
{
  SwitchController().pressHatButton(button);
  delay(pushing_time_msec);
  SwitchController().releaseHatButton();
  delay(_button_pushing_msec);
}

/**
  @brief Hold the left stick in a specified direction

  @param Lstick                Direction to tilt the stick
  @param tilt_time_msec        Duration to hold the tilt
  @param delay_after_tilt_msec Delay after releasing the stick
*/
void UseLStick(LS Lstick, int tilt_time_msec, int delay_after_tilt_msec)
{
  int lx_per = 0;
  int ly_per = 0;
  int rx_per = 0;
  int ry_per = 0;
  switch (Lstick)
  {
    case LS::LS_DOWN:
      ly_per = 100;
      break;
    case LS::LS_UP:
      ly_per = -100;
      break;
    case LS::LS_LEFT:
      lx_per = -100;
      break;
    case LS::LS_RIGHT:
      lx_per = 100;
      break;
    default:
      break;
  }
  tiltJoystick(lx_per, ly_per, rx_per, ry_per, tilt_time_msec, delay_after_tilt_msec);
}

/**
  @brief Hold the right stick in a specified direction

  @param Rstick                Direction to tilt the stick
  @param tilt_time_msec        Duration to hold the tilt
  @param delay_after_tilt_msec Delay after releasing the stick
*/
void UseRStick(RS Rstick, int tilt_time_msec, int delay_after_tilt_msec)
{
  int lx_per = 0;
  int ly_per = 0;
  int rx_per = 0;
  int ry_per = 0;
  switch (Rstick)
  {
    case RS::RS_DOWN:
      ry_per = 100;
      break;
    case RS::RS_UP:
      ry_per = -100;
      break;
    case RS::RS_LEFT:
      rx_per = -100;
      break;
    case RS::RS_RIGHT:
      rx_per = 100;
      break;
    default:
      break;
  }
  tiltJoystick(lx_per, ly_per, rx_per, ry_per, tilt_time_msec, delay_after_tilt_msec);
}

/* Function to tilt the left stick */
/**
  @brief Tilt the left stick in a specified angle (degrees)

  @param direction_deg         Direction to tilt (in degrees)
  @param power                 Tilt magnitude (0 to 1)
  @param holdtime             Duration to hold the tilt
  @param delay_after_tilt_msec Delay after releasing the stick
*/
void TiltLeftStick(int direction_deg, double power, int holdtime, int delaytime)
{
  double rad = (double)direction_deg * PI / 180.0; // Convert degrees to radians
  //int x, y;
  int lx_per;
  int ly_per;
  lx_per = (double)sin(rad) * power * 100;
  ly_per = (double) - 1 * cos(rad) * power * 100;
  SwitchController().setStickTiltRatio(lx_per, ly_per, 0, 0);
  if (holdtime > 0) { // If holdtime=0, the stick remains tilted
    delay(holdtime);
    //SwitchControlLibrary().MoveLeftStick(128, 128); // Reset stick position
    SwitchController().setStickTiltRatio(0, 0, 0, 0);
  }
  if (delaytime > 0) delay(delaytime);
  return;
}

/**
  @brief Set the tilt amount for the joysticks

  @param lx_per                Left stick X-axis tilt percentage (-100 to 100)
  @param ly_per                Left stick Y-axis tilt percentage (-100 to 100)
  @param rx_per                Right stick X-axis tilt percentage (-100 to 100)
  @param ry_per                Right stick Y-axis tilt percentage (-100 to 100)
  @param tilt_time_msec        Duration to hold the tilt
  @param delay_after_tilt_msec Delay after releasing the sticks
*/
void tiltJoystick(int lx_per, int ly_per, int rx_per, int ry_per, int tilt_time_msec, int delay_after_tilt_msec)
{
  SwitchController().setStickTiltRatio(lx_per, ly_per, rx_per, ry_per);
  delay(tilt_time_msec);
  if (delay_after_tilt_msec > 0)
  {
    SwitchController().setStickTiltRatio(0, 0, 0, 0);
    delay(delay_after_tilt_msec);
  }
}

void sendReportOnly(USB_JoystickReport_Input_t t_joystickInputData)
{
  SwitchController().sendReportOnly(t_joystickInputData);
}
