#include <Wire.h> 
#include <RTClib.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// Initialize LCD and RTC
hd44780_I2Cexp lcd;
RTC_DS3231 rtc;

// LCD dimensions
const int LCD_COLS = 20;
const int LCD_ROWS = 4;

// Days of the week
String daysOfWeek[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

// Wi-Fi credentials
const char* ssid     = "FIOT";
const char* password = "Wulianzhuanyong1!";

// NTP settings
WiFiUDP udp;
NTPClient timeClient(udp, "pool.ntp.org", 0, 60000); // UTC+0 offset, update every 60 seconds

// Timezone map for major cities
// Format: City -> (Standard Time Offset in seconds, DST Offset in seconds)
const String cities[] = {"New York", "Los Angeles", "London", "Tokyo", "Berlin"};
const long offsets[] = {-5, -8, 0, 9, 1};  // Standard Time offsets in hours
const long dstOffsets[] = {-4, -7, 0, 9, 2}; // DST offsets in hours

long timeZoneOffset = 0; // Default UTC offset (will be updated based on input)
bool isDST = false; // DST status flag

void setup() {
  Serial.begin(9600);

  // Initialize LCD
  int status = lcd.begin(LCD_COLS, LCD_ROWS);
  if (status) {
    hd44780::fatalError(status);
  }
  lcd.clear();
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  
  // Initialize NTPClient
  timeClient.begin();
  timeClient.setTimeOffset(timeZoneOffset);  // Set time zone offset here

  // Initialize RTC
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  
  // Check if RTC lost power
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, setting default time...");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // Sets RTC to compilation time
  }

  Serial.println("Enter your city for time zone:");
  for (int i = 0; i < sizeof(cities) / sizeof(cities[0]); i++) {
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.println(cities[i]);
  }
}

void loop() {
  // Get city name from Serial Monitor and update timezone
  if (Serial.available() > 0) {
    String input = Serial.readString();
    input.trim();  // Remove leading/trailing spaces

    // Check for matching city
    bool cityFound = false;
    for (int i = 0; i < sizeof(cities) / sizeof(cities[0]); i++) {
      if (input.equalsIgnoreCase(cities[i])) {
        cityFound = true;
        timeZoneOffset = offsets[i] * 3600; // Convert hours to seconds
        
        // Ask if DST should be enabled
        Serial.println("Do you want to enable DST? (yes/no):");
        while (Serial.available() == 0) {} // Wait for input
        String dstInput = Serial.readString();
        dstInput.trim();
        
        if (dstInput.equalsIgnoreCase("yes")) {
          isDST = true;
          timeZoneOffset = dstOffsets[i] * 3600; // Update to DST offset
          Serial.println("DST enabled.");
        } else {
          isDST = false;
          Serial.println("DST disabled.");
        }
        
        timeClient.setTimeOffset(timeZoneOffset);  // Update NTP client with new offset
        Serial.print("Timezone set to: ");
        Serial.print(cities[i]);
        Serial.print(" (UTC");
        Serial.print(offsets[i]);
        if (isDST) {
          Serial.print(" DST");
        }
        Serial.println(")");
        break;
      }
    }

    // If city not found, ask for valid city
    if (!cityFound) {
      Serial.println("City not recognized. Please enter a valid city.");
    }
  }

  // Update NTP time
  timeClient.update();

  // Get the current epoch time
  unsigned long epochTime = timeClient.getEpochTime();

  // Print the epoch time to check the output
  //Serial.print("Epoch Time: ");
  //Serial.println(epochTime);

  // Convert epoch time to RTC DateTime
  DateTime now(epochTime); // This converts the epoch time to DateTime format

  // Adjust RTC with the correct time
  rtc.adjust(now);

  // Update the display with the new time
  updateDisplay();

  delay(1000);
}

// Function to display time on the LCD
void updateDisplay() {
  DateTime now = rtc.now();
  
  lcd.clear();
  lcd.setCursor(4, 2);
  lcd.print(daysOfWeek[now.dayOfTheWeek()]); // Get day of the week
  
  lcd.setCursor(1, 1);
  lcd.print(now.year(), DEC);
  lcd.print("-");
  lcd.print(now.month(), DEC);
  lcd.print("-");
  lcd.print(now.day(), DEC);
  lcd.print(" ");
  
  if (now.hour() < 10) lcd.print("0");
  lcd.print(now.hour(), DEC);
  lcd.print(":");
  
  if (now.minute() < 10) lcd.print("0");
  lcd.print(now.minute(), DEC);
  lcd.print(":");
  
  if (now.second() < 10) lcd.print("0");
  lcd.print(now.second(), DEC);
}
