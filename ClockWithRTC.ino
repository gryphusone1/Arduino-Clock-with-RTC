#include <Wire.h>
#include <RTClib.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>

// Initialize LCD and RTC
hd44780_I2Cexp lcd;
RTC_DS3231 rtc;

// LCD dimensions
const int LCD_COLS = 16;
const int LCD_ROWS = 2;

// Days of the week
String daysOfWeek[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

void setup() {
  Serial.begin(9600);

  // Initialize LCD
  int status = lcd.begin(LCD_COLS, LCD_ROWS);
  if (status) {
    hd44780::fatalError(status);
  }
  lcd.clear();
  
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
  
  Serial.println("Enter date and time (YYYY-MM-DD HH:MM:SS):");
}

void loop() {
  checkSerialInput();  // Check for new time input from Serial
  updateDisplay();     // Update the LCD with the current date and time
  delay(1000);
}

// Function to check for Serial input and update RTC time
void checkSerialInput() {
  if (Serial.available() > 0) {
    String input = Serial.readString();
    int year, month, day, hour, minute, second;
    
    // Parse input format: YYYY-MM-DD HH:MM:SS
    if (sscanf(input.c_str(), "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second) == 6) {
      rtc.adjust(DateTime(year, month, day, hour, minute, second));
      Serial.println("RTC time updated!");
    } else {
      Serial.println("Invalid format! Use YYYY-MM-DD HH:MM:SS");
    }
  }
}

// Function to display time on the LCD
void updateDisplay() {
  DateTime now = rtc.now();
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(daysOfWeek[now.dayOfTheWeek()]); // Get day of the week
  
  lcd.setCursor(0, 1);
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
