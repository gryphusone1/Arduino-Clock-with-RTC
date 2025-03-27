# RTC Clock with LCD Display

## Description
This project uses an **Arduino**, a **ZS-042 RTC module (DS3231)**, and an **I2C LCD** to display real-time date and time. The time can be updated via the serial monitor by entering a formatted date string.

## Components Used
- Arduino Uno (or compatible board)
- ZS-042 RTC module (DS3231)
- 20x4 LCD with I2C module
- Jumper wires

## Libraries Required
Make sure you have the following libraries installed in the Arduino IDE:
- `Wire.h` (Built-in, used for I2C communication)
- `RTClib.h` (For RTC module)
- `hd44780.h` and `hd44780ioClass/hd44780_I2Cexp.h` (For I2C LCD display)

You can install them via **Library Manager** in the Arduino IDE.

## Wiring Diagram
| Arduino | RTC Module (ZS-042) | LCD I2C |
|---------|---------------------|---------|
| 5V      | VCC                 | VCC     |
| GND     | GND                 | GND     |
| A4      | SDA                 | SDA     |
| A5      | SCL                 | SCL     |

## How It Works
1. The RTC module keeps track of the date and time.
2. The Arduino retrieves the current time from the RTC and displays it on the LCD.
3. Users can update the time via the **Serial Monitor** in the format:
   ```
   YYYY-MM-DD HH:MM:SS
   ```
   Example: `2025-03-27 14:30:00`
4. If the RTC loses power, it will reset to the last programmed time.

## Code Explanation
- **`setup()`**
  - Initializes the LCD and RTC module.
  - If the RTC lost power, it sets the default time to the code compilation time.
- **`loop()`**
  - Reads any new time input from the serial monitor.
  - Updates the LCD every second with the current date and time.
- **`checkSerialInput()`**
  - Reads and parses user input to update the RTC module.
- **`updateDisplay()`**
  - Fetches the current time from the RTC and displays it on the LCD.

## Usage Instructions
1. Upload the code to the Arduino.
2. Open the Serial Monitor (9600 baud rate).
3. If needed, enter a new date/time in the format `YYYY-MM-DD HH:MM:SS`.
4. The LCD will update automatically every second.

## Troubleshooting
- **LCD Not Displaying Data**
  - Ensure the I2C address is correct (default `0x27` or `0x3F`).
  - Verify wiring connections.
- **RTC Not Detected**
  - Check if the module is powered correctly.
  - Ensure the `RTClib.h` library is installed.
- **Time Not Updating**
  - Ensure the correct format is used when entering time in the Serial Monitor.



