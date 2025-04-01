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

// Heart symbol custom character
byte heart[8] = {
  0b00000,
  0b01010,
  0b11111,
  0b11111,
  0b11111,
  0b01110,
  0b00100,
  0b00000
};

// Radio wave symbol custom character
byte radioWave[8] = {
  0b00001,
  0b00011,
  0b00111,
  0b01010,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};

// Days of the week
String daysOfWeek[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

// Wi-Fi credentials
const char* ssid     = "xxxx";
const char* password = "xxxx";

// NTP settings
WiFiUDP udp;
NTPClient timeClient(udp, "pool.ntp.org", -4 * 3600, 60000); // Default New York (UTC-4), update every 60 sec

// NTP sync mode
enum NTPMode {SYNC_ONCE, SYNC_PERIODIC, MANUAL};
NTPMode syncMode = SYNC_ONCE;

// Manual time input variables
int manualYear, manualMonth, manualDay, manualHour, manualMinute, manualSecond;

// Wi-Fi connection status
bool connected = false;
bool ntpSynced = false; // Add NTP synced flag

// Timeout parameters
unsigned long timeoutMillis = 5000; // Timeout after 5 seconds
unsigned long startMillis = 0;      // Start time for timeout

// Last sync time
DateTime lastSyncDateTime;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("Select NTP sync mode:");
  Serial.println("1: Sync Once");
  Serial.println("2: Sync Periodically");
  Serial.println("3: Do Not Sync (Manual input)");

  startMillis = millis();

  while (millis() - startMillis < timeoutMillis) {
    if (Serial.available() > 0) {
      int choice = Serial.parseInt();

      if (choice == 1) {
        syncMode = SYNC_ONCE;
        Serial.println("Sync Once mode selected.");
        break;
      } else if (choice == 2) {
        syncMode = SYNC_PERIODIC;
        Serial.println("Sync Periodically mode selected.");
        break;
      } else if (choice == 3) {
        syncMode = MANUAL;
        Serial.println("Manual input mode selected.");
        Serial.println("Enter year (e.g., 2023): ");
        while (Serial.available() == 0) {}
        manualYear = Serial.parseInt();

        Serial.println("Enter month (1-12): ");
        while (Serial.available() == 0) {}
        manualMonth = Serial.parseInt();

        Serial.println("Enter day (1-31): ");
        while (Serial.available() == 0) {}
        manualDay = Serial.parseInt();

        Serial.println("Enter hour (0-23): ");
        while (Serial.available() == 0) {}
        manualHour = Serial.parseInt();

        Serial.println("Enter minute (0-59): ");
        while (Serial.available() == 0) {}
        manualMinute = Serial.parseInt();

        Serial.println("Enter second (0-59): ");
        while (Serial.available() == 0) {}
        manualSecond = Serial.parseInt();

        rtc.adjust(DateTime(manualYear, manualMonth, manualDay, manualHour, manualMinute, manualSecond));
        break;
      }
    }
  }

  if (millis() - startMillis >= timeoutMillis) {
    syncMode = SYNC_ONCE;
    Serial.println("\nTimeout reached! Using RTC time.");
  }

  int status = lcd.begin(LCD_COLS, LCD_ROWS);
  if (status) {
    hd44780::fatalError(status);
  }
  lcd.clear();

  lcd.createChar(0, heart);
  lcd.createChar(1, radioWave);

  // Connect to Wi-Fi with retry limit
  int wifiRetry = 0;
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED && wifiRetry < 3) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
    wifiRetry++;
  }
  if(WiFi.status() == WL_CONNECTED){
    Serial.println("Connected to WiFi");
    connected = true;
  } else {
    Serial.println("WiFi connection failed after 3 attempts.");
    connected = false;
  }

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, setting default time...");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  if (syncMode == SYNC_PERIODIC || syncMode == SYNC_ONCE) {
    timeClient.begin();
    timeClient.setTimeOffset(-4 * 3600);
  }
}

void loop() {
  if (syncMode == SYNC_PERIODIC) {
    if (timeClient.update()) {
      unsigned long epochTime = timeClient.getEpochTime();
      DateTime now(epochTime);
      rtc.adjust(now);
      lastSyncDateTime = now;
      ntpSynced = true;
    }
  } else if (syncMode == SYNC_ONCE) {
    if (!ntpSynced) {
      if (timeClient.update()) {
        unsigned long epochTime = timeClient.getEpochTime();
        DateTime now(epochTime);
        rtc.adjust(now);
        lastSyncDateTime = now;
        ntpSynced = true;
      }
    }
  }

  updateDisplay();

  delay(1000);
}

void updateDisplay() {
  DateTime now = rtc.now();

  lcd.clear();

  lcd.setCursor(0, 0);
  if (WiFi.status() == WL_CONNECTED) {
    lcd.print("WiFi Connected    ");
  } else {
    lcd.print("WiFi Disconnected ");
  }

  lcd.setCursor(0, 1);
  lcd.print(now.year());
  lcd.print("-");
  if (now.month() < 10) lcd.print("0");
  lcd.print(now.month());
  lcd.print("-");
  if (now.day() < 10) lcd.print("0");
  lcd.print(now.day());
  lcd.print(" ");

  if (now.hour() < 10) lcd.print("0");
  lcd.print(now.hour());
  lcd.print(":");

  if (now.minute() < 10) lcd.print("0");
  lcd.print(now.minute());
  lcd.print(":");

  if (now.second() < 10) lcd.print("0");
  lcd.print(now.second());

  lcd.setCursor(4, 2);
  lcd.print(daysOfWeek[now.dayOfTheWeek()]);

  lcd.setCursor(0, 3);
  if (ntpSynced) {
    lcd.write(1);
    lcd.print(" ");
    if (lastSyncDateTime.hour() < 10) lcd.print("0");
    lcd.print(lastSyncDateTime.hour());
    lcd.print(":");
    if (lastSyncDateTime.minute() < 10) lcd.print("0");
    lcd.print(lastSyncDateTime.minute());
    lcd.print(":");
    if (lastSyncDateTime.second() < 10) lcd.print("0");
    lcd.print(lastSyncDateTime.second());
    lcd.print(" ");
    if (lastSyncDateTime.month() < 10) lcd.print("0");
    lcd.print(lastSyncDateTime.month());
    lcd.print("/");
    if (lastSyncDateTime.day() < 10) lcd.print("0");
    lcd.print(lastSyncDateTime.day());
  }
}