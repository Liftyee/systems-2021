/*
DS3231_test.pde
Eric Ayars
4/11

Test/demo of read routines for a DS3231 RTC.

Turn on the serial monitor after loading this to check if things are
working as they should.

*/
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>
#include <Wire.h>
#include <Arduino.h>

// SPI pins for the TFT display
#define TFT_CS         16
#define TFT_RST        15
#define TFT_DC         32
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

int threshold = 40;
bool touch1detected = false;
bool touch2detected = false;

void gotTouch1(){
 touch1detected = true;
}

void gotTouch2(){
 touch2detected = true;
}
void setup() {
  tft.init(240, 320);
  tft.setTextSize(2);
 
  // Start the serial interface
  Serial.begin(9600);
  delay(1000);
  touchAttachInterrupt(T0, gotTouch1, threshold);
  touchAttachInterrupt(T2, gotTouch2, threshold);
  
}



void loop() {
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextWrap(true);
  tft.setCursor(0, 30);
  if(touch1detected){
    touch1detected = false;
    tft.setTextColor(ST77XX_GREEN);
    tft.print("Touch 0 detected");
  }
  tft.setCursor(0,50);
  if(touch2detected){
    touch2detected = false;
    tft.setTextColor(ST77XX_YELLOW);
    tft.print("Touch 2 detected");
  }
  delay(500);
}
