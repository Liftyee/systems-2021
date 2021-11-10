#include <WiFi.h>
#include "time.h"
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <Wire.h>
#include <Arduino.h>
#include <Fonts/FreeSans24pt7b.h>

// SPI pins for the TFT display
#define TFT_CS         16
#define TFT_RST        15
#define TFT_DC         32
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

const char* ssid       = "ThinkPad";
const char* password   = "6EYEBTA8";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 3600;

void printLocalTime() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

void showTimeOnScreen() {
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  tft.setTextWrap(true);
  tft.setFont();
  
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    tft.print("Failed to obtain time");
    return;
  } 
  char longdate[32];
  char timedate[32];
  char shortime[16];
  strftime(longdate, sizeof(longdate), "%A, %B %d", &timeinfo);
  strftime(timedate, sizeof(timedate), "%H:%M:%S %d/%m/%Y", &timeinfo);
  strftime(shortime, sizeof(shortime), "%H:%M", &timeinfo);
  
  tft.print(longdate);
  tft.setCursor(0, 16);
  tft.print(timedate);
  //tft.setFont(&FreeSans24pt7b);
  tft.setTextSize(8);
  tft.setCursor(16, 48);
  
  tft.print(shortime);

}

void setup() {
  Serial.begin(115200);
  tft.init(240, 320);
  tft.setTextSize(2);
  tft.setRotation(3);
  
  //connect to WiFi
  Serial.printf("Connecting to %s ", ssid);
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0,0);
  tft.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
      tft.print(".");
  }
  Serial.println(" CONNECTED");
  tft.print(" CONNECTED");
  //init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();

  //disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}

void loop() {
  delay(1000);
  printLocalTime();
  showTimeOnScreen();
}