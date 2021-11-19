#include <WiFi.h>
#include "time.h"
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <Wire.h>
#include <Arduino.h>
#include <Fonts/FreeSans24pt7b.h>
#include <Fonts/FreeSans9pt7b.h>

// SPI pins for the TFT display
#define TFT_CS         16
#define TFT_RST        15
#define TFT_DC         32
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

#define WARM_PIN       22
#define COOL_PIN       21
#define WARM_CH        0
#define COOL_CH        1
#define PWM_RES        8
#define PWM_FREQ       5000

#define TOUCH_SENS     30
#define SEL_P          0
#define NEXT_P         2
#define TOUCH_WAIT     1000


const char* ssid       = "ThinkPad";
const char* password   = "6EYEBTA8";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 3600;

bool touchPins[10];
unsigned long long lastTouch[10];

void gotTouch0(){
  touchPins[0] = true;
}

void gotTouch2(){
  touchPins[2] = true;
}

void gotTouch3(){
  touchPins[3] = true;
}

void gotTouch4(){
  touchPins[4] = true;
}

bool getTouch(int pin){
  if (touchPins[pin] && millis() > lastTouch[pin]+TOUCH_WAIT){
    touchPins[pin] = false;
    lastTouch[pin] = millis();
    return true;
  }
  return false;
}

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
  tft.setCursor(0, 12);
  tft.setTextSize(1);
  tft.setTextWrap(true);
  tft.setFont(&FreeSans9pt7b);
  
  
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
  
  tft.println(longdate);
  tft.println(timedate);
  tft.setFont(&FreeSans24pt7b);
  tft.setCursor(0, 200);
  tft.setTextSize(2);
  tft.println(shortime);

}



void showMenu() {

}

void setup() {
  ledcSetup(WARM_CH, PWM_FREQ, PWM_RES);
  ledcSetup(COOL_CH, PWM_FREQ, PWM_RES);
  ledcAttachPin(WARM_PIN, WARM_CH);
  ledcAttachPin(COOL_PIN, COOL_CH);

  touchAttachInterrupt(T0, gotTouch0, TOUCH_SENS);
  //touchAttachInterrupt(T2, gotTouch2, TOUCH_SENS);
  //touchAttachInterrupt(T3, gotTouch3, TOUCH_SENS);
  //touchAttachInterrupt(T4, gotTouch4, TOUCH_SENS);

  Serial.begin(115200);
  tft.init(240, 320);
  tft.setTextSize(1);
  tft.setRotation(3);
  tft.setFont(&FreeSans9pt7b);
  //connect to WiFi
  Serial.printf("Connecting to %s ", ssid);
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0,12);
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
  
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE);
  
}
unsigned long long lastTime;

int lightState = 256;
void loop() {
  if (millis() >= lastTime + 1000) {
    lastTime = millis();
    printLocalTime();
    showTimeOnScreen();
  }
  

  
  ledcWrite(WARM_CH, lightState);
  if (getTouch(SEL_P)) {
    if (lightState == 256) {
      lightState = 0;
    } else {
      lightState = 256;
    }
    touchPins[SEL_P] = false;
  }
  // for (int i = 0; i <= 255; i++) {
  //   ledcWrite(WARM_CH, i);
  //   delay(10);
  // }
  // for (int i = 0; i < 255; i++) {
  //   ledcWrite(COOL_CH, i);
  //   delay(10);
  // }
  // for (int i = 255; i >= 0; i--) {
  //   ledcWrite(WARM_CH, i);
  //   delay(10);
  // }
  // for (int i = 255; i > 0; i--) {
  //   ledcWrite(COOL_CH, i);
  //   delay(10);
  // }  
  memset(touchPins, 0, sizeof(touchPins));
  delay(100);
}