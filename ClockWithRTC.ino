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
const String cities[] = {"New York", "Los Angeles", "London", "Tokyo", "Berlin"};
const long offsets[] = {-5, -8, 0, 9, 1};  // Standard Time offsets in hours
const long dstOffsets[] = {-4, -7, 0, 9, 2}; // DST offsets in hours

// Default settings: New York, UTC -4 (DST enabled)
String currentCity = "New York";
long timeZoneOffset = -4 * 3600; 
bool isDST = true;

// Scrolling text variables
String scrollText = " Clock ";
int scrollPos = 0;
unsigned long lastScrollTime = 0;
const int scrollDelay = 300; // Scroll speed (milliseconds)

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
  timeClient.setTimeOffset(timeZoneOffset);  // Set default timezone

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

  Serial.println("Do you want to update time settings? (yes/no)");
}

void loop() {
  // Check if user wants to update settings
  if (Serial.available() > 0) {
    String response = Serial.readString();
    response.trim(); // Remove spaces

    if (response.equalsIgnoreCase("yes")) {
      Serial.println("Enter your city for time zone:");
      for (int i = 0; i < sizeof(cities) / sizeof(cities[0]); i++) {
        Serial.print(i + 1);
        Serial.print(": ");
        Serial.println(cities[i]);
      }

      // Wait for city input
      while (Serial.available() == 0) {}
      String input = Serial.readString();
      input.trim();

      // Check for valid city
      bool cityFound = false;
      for (int i = 0; i < sizeof(cities) / sizeof(cities[0]); i++) {
        if (input.equalsIgnoreCase(cities[i])) {
          cityFound = true;
          currentCity = cities[i];
          timeZoneOffset = offsets[i] * 3600; // Convert hours to seconds

          // Ask for DST
          Serial.println("Do you want to enable DST? (yes/no):");
          while (Serial.available() == 0) {}
          String dstInput = Serial.readString();
          dstInput.trim();

          if (dstInput.equalsIgnoreCase("yes")) {
            isDST = true;
            timeZoneOffset = dstOffsets[i] * 3600;
            Serial.println("DST enabled.");
          } else {
            isDST = false;
            Serial.println("DST disabled.");
          }

          timeClient.setTimeOffset(timeZoneOffset); // Update NTP client
          Serial.print("Timezone set to: ");
          Serial.print(currentCity);
          Serial.print(" (UTC");
          Serial.print(offsets[i]);
          if (isDST) Serial.print(" DST");
          Serial.println(")");
          break;
        }
      }

      // If city is not recognized
      if (!cityFound) {
        Serial.println("City not recognized. Please enter a valid city.");
      }
    } else {
      Serial.println("Settings remain unchanged.");
    }
  }

  // Update NTP time
  timeClient.update();

  // Get the current epoch time
  unsigned long epochTime = timeClient.getEpochTime();

  // Convert epoch time to RTC DateTime
  DateTime now(epochTime); 

  // Adjust RTC with the correct time
  rtc.adjust(now);

  // Update the display with the new time
  updateDisplay();

  // Scroll "Clock" on first line
  scrollClockText();

  delay(1000);
}

// Function to scroll "Clock" across first line
void scrollClockText() {
  if (millis() - lastScrollTime > scrollDelay) {
    lastScrollTime = millis();
    
    lcd.setCursor(0, 0);
    for (int i = 0; i < LCD_COLS; i++) {
      int index = (scrollPos + i) % scrollText.length();
      lcd.print(scrollText[index]);
    }
    
    scrollPos = (scrollPos + 1) % scrollText.length();
  }
}

// Function to display time on the LCD
void updateDisplay() {
  DateTime now = rtc.now();

  // Line 2: Date
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

  // Line 3: Day of the week
  lcd.setCursor(4, 2);
  lcd.print(daysOfWeek[now.dayOfTheWeek()]);

  // Line 4: NTP Status
  lcd.setCursor(0, 3);
  if (WiFi.status() == WL_CONNECTED) {
    lcd.print("NTP Connected  ");
  } else {
    lcd.print("NTP Disconnected");
  }
}
