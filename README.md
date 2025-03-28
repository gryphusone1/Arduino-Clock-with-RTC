NTP Time with DST Toggle and City-Based Timezone Selection
This project allows you to set the time on your Arduino-based device using NTP (Network Time Protocol), with the ability to toggle Daylight Saving Time (DST) on or off. You can choose a city from a predefined list, and the program will automatically adjust the time zone and consider whether DST is active for that city. The time is then displayed on a 20x4 LCD screen.

Features
Fetches the current time from an NTP server.

Allows selection of a major city from a list.

Toggles Daylight Saving Time (DST) based on user input.

Displays the current time and date on a 20x4 LCD.

Automatically adjusts the time zone based on the selected city.

Handles time synchronization even when the device is powered off and on again.

Components Required
Arduino Uno R4 WiFi (or similar board with Wi-Fi capability)

20x4 LCD Display with I2C interface

RTC DS3231 Module (for backup time keeping)

Wi-Fi connection (to fetch the time via NTP)

Libraries Used
Wire: For I2C communication with the LCD.

RTClib: For interfacing with the DS3231 RTC module.

hd44780: For controlling the 20x4 LCD display with I2C.

WiFi: For connecting to the Wi-Fi network.

NTPClient: For fetching time from the NTP server using UDP.

Setup
Install Libraries:

In Arduino IDE, go to Sketch > Include Library > Manage Libraries.

Install the following libraries:

Wire (already included with Arduino IDE)

RTClib (for RTC DS3231)

hd44780 (for the LCD screen)

WiFi (for Wi-Fi connection)

NTPClient (for fetching time from NTP server)

Hardware Connections:

LCD Display: Connect the 20x4 LCD with I2C interface to the Arduino Uno R4 WiFi.

RTC DS3231: Connect the DS3231 RTC module to the Arduino via I2C (SCL to A5, SDA to A4).

Wi-Fi Configuration:

Update the following Wi-Fi credentials with your own:

cpp
Copy
Edit
const char* ssid     = "your_SSID";
const char* password = "your_PASSWORD";
How to Use
Upload the code to your Arduino.

Open the Serial Monitor.

The program will prompt you to choose a city from a predefined list. Example cities include New York, Los Angeles, London, Tokyo, and Berlin.

After selecting a city, you will be asked whether you want to enable DST (Daylight Saving Time). Type yes to enable DST or no to disable it.

The program will adjust the time zone accordingly and display the current date and time on the LCD screen.

City List
You can choose from the following cities:

New York (UTC-5, DST-4)

Los Angeles (UTC-8, DST-7)

London (UTC+0, DST+1)

Tokyo (UTC+9, no DST)

Berlin (UTC+1, DST+2)

Example Interaction
yaml
Copy
Edit
Enter your city for time zone:
1: New York
2: Los Angeles
3: London
4: Tokyo
5: Berlin

New York
Do you want to enable DST? (yes/no):
yes
Timezone set to: New York (UTC-4 DST)
Epoch Time: 1618806840
Code Explanation
Wi-Fi Connection: The code connects the Arduino to a Wi-Fi network using the provided SSID and password.

NTP Time Fetching: The program fetches the current time from an NTP server (pool.ntp.org) and stores the time in epoch format (seconds since January 1, 1970).

City and Time Zone: You can choose a city from the list of available cities. The program then sets the time zone based on the selected city and whether DST is enabled.

RTC Adjustment: The program adjusts the RTC DS3231 with the correct time based on the fetched NTP time.

LCD Display: The current time (with or without DST) is displayed on the 20x4 LCD screen.

Troubleshooting
Problem: The device cannot connect to Wi-Fi.

Solution: Check the Wi-Fi credentials and ensure your device has a stable internet connection.

Problem: The LCD displays incorrect or no time.

Solution: Ensure the RTC DS3231 module is connected correctly and is powered. If the RTC has lost power, it may need to be set initially using the rtc.adjust() function.

License
This project is licensed under the MIT License - see the LICENSE file for details.
