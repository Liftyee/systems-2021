#include <WiFi.h>
#include "time.h"
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Wire.h>
#include <Arduino.h>
#include <Fonts/FreeSans24pt7b.h>
#include <Fonts/FreeSans9pt7b.h>

// SPI pins for the TFT display
#define TFT_CS         5
#define TFT_RST        -1
#define TFT_DC         0
#define TFT_BL         15
#define TFT_BL_CH      2
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

#define LIGHT_SENS     34
#define WARM_PIN       33
#define COOL_PIN       32
#define WARM_CH        0
#define COOL_CH        1
#define PWM_RES        8
#define PWM_FREQ       5000

#define TOUCH_SENS     30
#define SEL_P          3
#define NEXT_P         4
#define TOUCH_WAIT     1000

// BME280 setup
#define SEALEVELPRESSURE_HPA (1013.25)
//Adafruit_BME280 bme;

// WiFi setup
const char* ssid       = "JD_HOME";
const char* password   = "6EYEBTA8!";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 3600;

// MP3 Player setup
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

SoftwareSerial DFSerial(16, 17); // RX, TX
DFRobotDFPlayerMini MP3;
void printDetail(uint8_t type, int value);

bool touchPins[20];
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
  if (touchPins[pin] && (millis() > (lastTouch[pin]+TOUCH_WAIT))){
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

GFXcanvas1 screen(320, 240);
GFXcanvas1 oldscreen(320, 240);

void tftPrint(String s, int16_t x, int16_t y, uint32_t color = ST77XX_BLACK) {
  int16_t  x1, y1;
  uint16_t w, h;
  screen.getTextBounds(s, x, y, &x1, &y1, &w, &h);
  screen.fillRect(x1, y1, w, h, color);
  screen.setCursor(x, y);
  screen.print(s);
}

void tftPrintln(String s, uint32_t color = ST77XX_BLACK) {
  int16_t  x1, y1;
  uint16_t w, h;
  screen.getTextBounds(s, tft.getCursorX(), tft.getCursorY(), &x1, &y1, &w, &h);
  screen.fillRect(x1, y1, w, h, color);
  screen.println(s);
}

void showTimeOnScreen() {
  screen.setCursor(0, 12);
  screen.setTextSize(1);
  screen.setTextWrap(true);
  screen.setFont(&FreeSans9pt7b);
  
  
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    screen.print("Failed to obtain time");
    return;
  } 
  char longdate[32];
  char timedate[32];
  char shortime[16];
  strftime(longdate, sizeof(longdate), "%A, %B %d", &timeinfo);
  strftime(timedate, sizeof(timedate), "%H:%M:%S %d/%m/%Y", &timeinfo);
  strftime(shortime, sizeof(shortime), "%H:%M", &timeinfo);
  
  //tftPrintln(longdate);
  //tftPrintln(timedate);

  screen.println(longdate);
  screen.println(timedate);

  screen.setCursor(280, 12);
  //tft.print(bme.readTemperature());
  screen.print("20°C");

  screen.setFont(&FreeSans24pt7b);
  screen.setTextSize(2);
  //tftPrint(shortime, 0, 200);
  screen.setCursor(0, 160);
  screen.print(shortime);
  
}


bool inMenu = false;
void showMenu() {
  //tft.fillScreen(ST77XX_BLACK);
  screen.setCursor(0, 24);
  screen.setTextSize(2);
  screen.setFont(&FreeSans24pt7b);
  screen.write("test menu");
  screen.setTextSize(1);
}

void updateTFT(GFXcanvas1* oldS, GFXcanvas1* newS, uint16_t color, uint16_t bg) { 
  Serial.print("Updating TFT");
  for (int x = 0; x < 320; x++) {
    for (int y = 0; y < 240; y++) {
      if (oldS->getPixel(x, y) != newS->getPixel(x, y)) {
        //Serial.print("Diff pixel!");
        if (newS->getPixel(x, y) == 0x1) {
          tft.drawPixel(x, y, color);
        } else {
          Serial.print("blacc");
          tft.drawPixel(x, y, ST77XX_BLACK);
        }
      }
    }
  }
  
}

void setup() {
  ledcSetup(WARM_CH, PWM_FREQ, PWM_RES);
  ledcSetup(COOL_CH, PWM_FREQ, PWM_RES);
  ledcSetup(TFT_BL_CH, PWM_FREQ, PWM_RES);
  ledcAttachPin(WARM_PIN, WARM_CH);
  ledcAttachPin(COOL_PIN, COOL_CH);
  ledcAttachPin(TFT_BL, TFT_BL_CH);

  //touchAttachInterrupt(T0, gotTouch0, TOUCH_SENS);
  //touchAttachInterrupt(T2, gotTouch2, TOUCH_SENS);
  touchAttachInterrupt(T2, gotTouch3, TOUCH_SENS);
  touchAttachInterrupt(T4, gotTouch4, TOUCH_SENS);

  Serial.begin(115200);
  tft.init(240, 320);
  tft.setTextSize(1);
  tft.setRotation(3);
  tft.setFont(&FreeSans9pt7b);
  tft.setTextColor(ST77XX_WHITE);
  ledcWrite(TFT_BL_CH, map(analogRead(LIGHT_SENS), 0, 4095, 1, 256));
  //connect to WiFi
  Serial.printf("Connecting to %s ", ssid);
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0,12);

  memset(touchPins, 0, sizeof(touchPins));

  tft.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
      tft.print(".");
      if (getTouch(SEL_P)) {
        break;
      }
  }
  Serial.println(" CONNECTED");
  tft.print(" CONNECTED");
  //init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();

  //disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  
  Serial.println("BME280 test");

  // unsigned status;
  
  // // default settings
  // status = bme.begin();  
  // // You can also pass in a Wire library object like &Wire2
  // // status = bme.begin(0x76, &Wire2)
  // if (!status) {
  //     Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
  //     Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(),16);
  //     Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
  //     Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
  //     Serial.print("        ID of 0x60 represents a BME 280.\n");
  //     Serial.print("        ID of 0x61 represents a BME 680.\n");
  //     while (1) delay(10);
  // }
  //tft.drawBitmap(0, 0, screen.getBuffer(), 320, 240, ST77XX_WHITE, ST77XX_BLACK);
  tft.fillScreen(ST77XX_BLACK);
  
  DFSerial.begin(9600);

  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));

  if (!MP3.begin(DFSerial)) {  //Use softwareSerial to communicate with mp3.
    tft.print("DFPlayer not connected!");
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    //while(true);
  }
  Serial.println(F("DFPlayer Mini online."));

  MP3.volume(30);  //Set volume value. From 0 to 30
  MP3.play(3);  //Play the first mp3
  tft.fillScreen(ST77XX_BLACK);

  updateTFT(&oldscreen, &screen, ST77XX_WHITE, ST77XX_BLACK);
  delay(100);
  tft.fillScreen(ST77XX_BLACK);

}
unsigned long long lastTime;

int lightState = 256;
void loop() {
  
  tft.setCursor(0, 0);
  if (!inMenu) {
    if (millis() >= lastTime + 1000) {
      lastTime = millis();
      printLocalTime();
      showTimeOnScreen();
    }
    if (getTouch(SEL_P)) {
      Serial.println("SEL button pressed!");
      inMenu = true;
      tft.fillScreen(ST77XX_BLACK);
      showMenu();
    }
  } else {
    showMenu();
    if (getTouch(SEL_P)) {
      Serial.println("SEL button pressed!");
      inMenu = false;
      tft.fillScreen(ST77XX_BLACK);
    }
  }

  // DO ALL DISPLAY OPERATIONS BEFORE THIS POINT
  //tft.drawBitmap(0, 0, screen.getBuffer(), 320, 240, ST77XX_WHITE, ST77XX_BLACK);
  updateTFT(&oldscreen, &screen, ST77XX_WHITE, ST77XX_BLACK);
  
  //THIS DOENS@T WORK
  //oldscreen = screen;

  // THIS IS HIDEOUSLY INEFFICIENT
  for (int x = 0; x < 320; x++) {
    for (int y = 0; y < 240; y++) {
      oldscreen.drawPixel(x, y, screen.getPixel(x, y));
    }
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
  //   ledhhcWrite(WARM_CH, i);
  //   delay(10);
  // }
  // for (int i = 255; i > 0; i--) {
  //   ledcWrite(COOL_CH, i);
  //   delay(10);
  // }  
  memset(touchPins, 0, sizeof(touchPins));
  delay(100);

  //dacWrite(25, 128);
  if (MP3.available()) {
    printDetail(MP3.readType(), MP3.read()); //Print the detail message from DFPlayer to handle different errors and states.
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