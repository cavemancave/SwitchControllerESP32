#!/usr/bin/env python3
"""
Nintendo Switch Controller Python Interface
===========================================

This script demonstrates how to control the RawBinaryAdapter example
by sending raw 8-byte HID reports over serial.

Requirements:
    pip install pyserial

Usage:
    python switch_controller.py [serial_port]
    
Example:
    python switch_controller.py /dev/ttyUSB0     # Linux
    python switch_controller.py COM3            # Windows

HID Report Format (8 bytes):
[Button_Low, Button_High, Hat, LX, LY, RX, RY, Dummy]
"""

import serial
import time
import sys

class SwitchController:
    # Button bit values
    BUTTONS = {
        'Y': 0x0001, 'B': 0x0002, 'A': 0x0004, 'X': 0x0008,
        'L': 0x0010, 'R': 0x0020, 'ZL': 0x0040, 'ZR': 0x0080,
        'MINUS': 0x0100, 'PLUS': 0x0200, 'LCLICK': 0x0400, 'RCLICK': 0x0800,
        'HOME': 0x1000, 'CAPTURE': 0x2000
    }
    
    # Hat direction values
    HAT = {
        'UP': 0x00, 'UP_RIGHT': 0x01, 'RIGHT': 0x02, 'RIGHT_DOWN': 0x03,
        'DOWN': 0x04, 'DOWN_LEFT': 0x05, 'LEFT': 0x06, 'LEFT_UP': 0x07,
        'CENTER': 0x08
    }
    
    def __init__(self, port, baudrate=115200):
        """Initialize serial connection to ESP32"""
        self.ser = serial.Serial(port, baudrate, timeout=1)
        time.sleep(2)  # Wait for Arduino to initialize
        print(f"Connected to {port}")
        
    def send_raw_report(self, buttons=0, hat=0x08, lx=0x80, ly=0x80, rx=0x80, ry=0x80, dummy=0x00):
        """Send raw HID report to Switch"""
        # Convert 16-bit button value to little-endian bytes
        button_low = buttons & 0xFF
        button_high = (buttons >> 8) & 0xFF
        
        report = bytes([button_low, button_high, hat, lx, ly, rx, ry, dummy])
        self.ser.write(report)
        
    def press_button(self, button_name, duration=0.1):
        """Press a button for specified duration"""
        if button_name.upper() not in self.BUTTONS:
            print(f"Unknown button: {button_name}")
            return
            
        button_value = self.BUTTONS[button_name.upper()]
        
        # Press
        self.send_raw_report(buttons=button_value)
        time.sleep(duration)
        
        # Release
        self.send_raw_report()
        
    def press_buttons(self, button_names, duration=0.1):
        """Press multiple buttons simultaneously"""
        button_value = 0
        for name in button_names:
            if name.upper() in self.BUTTONS:
                button_value |= self.BUTTONS[name.upper()]
            else:
                print(f"Unknown button: {name}")
                
        # Press combination
        self.send_raw_report(buttons=button_value)
        time.sleep(duration)
        
        # Release
        self.send_raw_report()
        
    def set_hat(self, direction):
        """Set hat/D-pad direction"""
        if direction.upper() not in self.HAT:
            print(f"Unknown hat direction: {direction}")
            return
            
        hat_value = self.HAT[direction.upper()]
        self.send_raw_report(hat=hat_value)
        
    def set_stick(self, stick, x, y):
        """Set stick position (-100 to 100)"""
        # Convert percentage to 0-255 range
        stick_x = int((x + 100) * 255 / 200)
        stick_y = int((y + 100) * 255 / 200)
        
        # Clamp values
        stick_x = max(0, min(255, stick_x))
        stick_y = max(0, min(255, stick_y))
        
        if stick.lower() == 'left':
            self.send_raw_report(lx=stick_x, ly=stick_y)
        elif stick.lower() == 'right':
            self.send_raw_report(rx=stick_x, ry=stick_y)
        else:
            print(f"Unknown stick: {stick}")
            
    def neutral(self):
        """Return controller to neutral state"""
        self.send_raw_report()
        
    def close(self):
        """Close serial connection"""
        self.ser.close()

def demo(controller):
    """Demonstration of various controller functions"""
    print("Running demo sequence...")
    
    # Single button presses
    print("Testing single buttons...")
    for button in ['A', 'B', 'X', 'Y']:
        print(f"Pressing {button}")
        controller.press_button(button, 0.5)
        time.sleep(0.2)
    
    # Button combinations
    print("Testing button combinations...")
    controller.press_buttons(['A', 'B'], 0.5)
    time.sleep(0.5)
    
    # D-pad directions
    print("Testing D-pad...")
    for direction in ['UP', 'RIGHT', 'DOWN', 'LEFT']:
        print(f"D-pad {direction}")
        controller.set_hat(direction)
        time.sleep(0.3)
    
    # Return to center
    controller.set_hat('CENTER')
    time.sleep(0.5)
    
    # Stick movements
    print("Testing stick movements...")
    controller.set_stick('left', 100, 0)   # Full right
    time.sleep(0.5)
    controller.set_stick('left', -100, 0)  # Full left
    time.sleep(0.5)
    controller.set_stick('left', 0, 100)   # Full up
    time.sleep(0.5)
    controller.set_stick('left', 0, -100)  # Full down
    time.sleep(0.5)
    
    # Return to neutral
    controller.neutral()
    print("Demo complete!")

def main():
    if len(sys.argv) < 2:
        print("Usage: python switch_controller.py <serial_port>")
        print("Example: python switch_controller.py /dev/ttyUSB0")
        sys.exit(1)
    
    port = sys.argv[1]
    
    try:
        controller = SwitchController(port)
        
        print("Nintendo Switch Controller Ready!")
        print("Type 'demo' for demonstration, 'quit' to exit")
        print("Commands: press <button>, combo <btn1> <btn2>, hat <dir>, stick <left/right> <x> <y>, neutral")
        
        while True:
            try:
                cmd = input("> ").strip().split()
                if not cmd:
                    continue
                    
                if cmd[0] == 'quit':
                    break
                elif cmd[0] == 'demo':
                    demo(controller)
                elif cmd[0] == 'press' and len(cmd) == 2:
                    controller.press_button(cmd[1])
                elif cmd[0] == 'combo' and len(cmd) >= 3:
                    controller.press_buttons(cmd[1:])
                elif cmd[0] == 'hat' and len(cmd) == 2:
                    controller.set_hat(cmd[1])
                elif cmd[0] == 'stick' and len(cmd) == 4:
                    controller.set_stick(cmd[1], int(cmd[2]), int(cmd[3]))
                elif cmd[0] == 'neutral':
                    controller.neutral()
                else:
                    print("Invalid command")
                    
            except KeyboardInterrupt:
                break
            except Exception as e:
                print(f"Error: {e}")
        
        controller.close()
        
    except Exception as e:
        print(f"Failed to connect: {e}")

if __name__ == "__main__":
    main()