#include <WiFi.h>
#include "time.h"
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <Wire.h>
#include <Arduino.h>
#include <Fonts/FreeSans24pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSans9pt7b.h>

// define SPI pins for the TFT display
#define TFT_CS         5
#define TFT_RST        4
#define TFT_DC         0
#define TFT_BL         15
#define TFT_BL_CH      2
#define BASE_FONT_HEIGHT 12
#define SEL_BORDER_WIDTH 2
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

#define LIGHT_SENS     34
#define WARM_PIN       32
#define COOL_PIN       33
#define WARM_CH        0
#define COOL_CH        1
#define PWM_RES        8
#define PWM_FREQ       5000

#define TOUCH_SENS     30 // higher number = higher sensitivity
#define SEL_P          4
#define BACK_P         5
#define UP_P           7
#define DOWN_P         6
#define TOUCH_WAIT     500

// WiFi setup
const char* ssid       = "cafe-PPSK";
const char* password   = "cold.next.rule.ring";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 0;