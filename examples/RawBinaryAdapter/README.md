# Raw Binary Adapter

This example provides the most direct control over the Nintendo Switch controller by accepting raw 8-byte HID reports over serial.

## Features

- **Direct HID control**: Send raw `USB_JoystickReport_Input_t` data
- **No Arduino-side processing**: Minimal latency
- **Python interface**: Complete Python library included
- **Binary protocol**: Maximum efficiency

## Usage

### Arduino Side

1. Upload `RawBinaryAdapter.ino` to your ESP32
2. Connect ESP32 to Nintendo Switch via USB
3. Open Serial Monitor at 115200 baud (optional, for debugging)

### Python Side

```bash
# Install requirements
pip install pyserial

# Run the Python controller
python python_controller.py /dev/ttyUSB0  # Linux
python python_controller.py COM3          # Windows
```

## Protocol

Each HID report is exactly **8 bytes**:

| Byte | Type | Description | Range |
|------|------|-------------|-------|
| 0 | uint8 | Button bits (low byte) | 0x00-0xFF |
| 1 | uint8 | Button bits (high byte) | 0x00-0xFF |
| 2 | uint8 | Hat direction | 0x00-0x08 |
| 3 | uint8 | Left stick X | 0x00-0xFF |
| 4 | uint8 | Left stick Y | 0x00-0xFF |
| 5 | uint8 | Right stick X | 0x00-0xFF |
| 6 | uint8 | Right stick Y | 0x00-0xFF |
| 7 | uint8 | Dummy/Padding | 0x00 |

### Button Bits

```
Bit 0  (0x0001) = Y        Bit 8  (0x0100) = MINUS    
Bit 1  (0x0002) = B        Bit 9  (0x0200) = PLUS     
Bit 2  (0x0004) = A        Bit 10 (0x0400) = LCLICK   
Bit 3  (0x0008) = X        Bit 11 (0x0800) = RCLICK   
Bit 4  (0x0010) = L        Bit 12 (0x1000) = HOME     
Bit 5  (0x0020) = R        Bit 13 (0x2000) = CAPTURE  
Bit 6  (0x0040) = ZL       
Bit 7  (0x0080) = ZR       
```

### Hat Values

```
0x00 = UP        0x02 = RIGHT      0x04 = DOWN       0x06 = LEFT
0x01 = UP_RIGHT  0x03 = RIGHT_DOWN 0x05 = DOWN_LEFT  0x07 = LEFT_UP
0x08 = CENTER
```

### Stick Values

- `0x00` = Full left/down
- `0x80` = Center
- `0xFF` = Full right/up

## Python Examples

```python
import serial
ser = serial.Serial('/dev/ttyUSB0', 115200)

# Press A button with centered sticks
report = bytes([0x04, 0x00, 0x08, 0x80, 0x80, 0x80, 0x80, 0x00])
ser.write(report)

# Release all (neutral state)  
report = bytes([0x00, 0x00, 0x08, 0x80, 0x80, 0x80, 0x80, 0x00])
ser.write(report)

# Press A+B simultaneously
report = bytes([0x06, 0x00, 0x08, 0x80, 0x80, 0x80, 0x80, 0x00])
ser.write(report)

# Left stick full right, right stick full up
report = bytes([0x00, 0x00, 0x08, 0xFF, 0x80, 0x80, 0xFF, 0x00])
ser.write(report)
```

## Interactive Python Controller

The included `python_controller.py` provides an interactive command-line interface:

```bash
> press a           # Press A button
> combo a b         # Press A+B simultaneously  
> hat up            # D-pad up
> stick left 100 0  # Left stick full right
> neutral           # Release everything
> demo              # Run demonstration sequence
```