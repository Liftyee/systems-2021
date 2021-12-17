#include <WiFi.h>
#include "time.h"
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <Wire.h>
#include <Arduino.h>
#include <Fonts/FreeSans24pt7b.h>
#include <Fonts/FreeSans9pt7b.h>

// SPI pins for the TFT display
#define TFT_CS         5
#define TFT_RST        17
#define TFT_DC         16
#define TFT_BL         0
#define TFT_BL_CH      2
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

#define WARM_PIN       22
#define COOL_PIN       21
#define WARM_CH        0
#define COOL_CH        1
#define PWM_RES        8
#define PWM_FREQ       5000

#define TOUCH_SENS     50
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

//void printTextFill()

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


bool inMenu = false;
void showMenu() {
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 24);
  tft.setTextSize(2);
  tft.setFont(&FreeSans24pt7b);
  tft.write("test menu");
  tft.setTextSize(1);
}
#include "Arduino.h"
#include "HardwareSerial.h"
#include "DFRobotDFPlayerMini.h"

HardwareSerial mySoftwareSerial(0); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);


void setup() {
  ledcSetup(WARM_CH, PWM_FREQ, PWM_RES);
  ledcSetup(COOL_CH, PWM_FREQ, PWM_RES);
  ledcSetup(TFT_BL_CH, PWM_FREQ, PWM_RES);
  ledcAttachPin(WARM_PIN, WARM_CH);
  ledcAttachPin(COOL_PIN, COOL_CH);
  ledcAttachPin(TFT_BL, TFT_BL_CH);

  touchAttachInterrupt(T0, gotTouch0, TOUCH_SENS);
  //touchAttachInterrupt(T2, gotTouch2, TOUCH_SENS);
  //touchAttachInterrupt(T3, gotTouch3, TOUCH_SENS);
  //touchAttachInterrupt(T4, gotTouch4, TOUCH_SENS);

  Serial.begin(115200);
  tft.init(240, 320);
  tft.setTextSize(1);
  tft.setRotation(3);
  tft.setFont(&FreeSans9pt7b);
  ledcWrite(TFT_BL_CH, map(analogRead(34), 0, 4095, 1, 256));
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
  
  mySoftwareSerial.begin(9600, SERIAL_8N1, 3, 1);

  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));

  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true);
  }
  Serial.println(F("DFPlayer Mini online."));

  myDFPlayer.volume(10);  //Set volume value. From 0 to 30
  myDFPlayer.play(1);  //Play the first mp3
}
unsigned long long lastTime;

int lightState = 256;
void loop() {
  if (!inMenu) {
    if (millis() >= lastTime + 1000) {
      lastTime = millis();
      printLocalTime();
      showTimeOnScreen();
    }
    if (getTouch(SEL_P)) {
      inMenu = true;
      showMenu();
    }
  } else {
    showMenu();
  }

  ledcWrite(TFT_BL_CH, map(analogRead(34), 0, 4095, 1, 256));
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

  //dacWrite(25, 128);
  if (myDFPlayer.available()) {
    printDetail(myDFPlayer.readType(), myDFPlayer.read()); //Print the detail message from DFPlayer to handle different errors and states.
  }
}



void printDetail(uint8_t type, int value){
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}