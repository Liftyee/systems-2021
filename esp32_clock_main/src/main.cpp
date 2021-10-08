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
#include <DS3231.h> // DS3231 RTC library
#include <SPI.h> 
#include <Wire.h>
#include <Arduino.h>

// SPI pins for the TFT display
#define TFT_CS         16
#define TFT_RST        15
#define TFT_DC         32
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

DS3231 rtc;
bool century = false;
bool h12Flag;
bool pmFlag;
byte alarmDay, alarmHour, alarmMinute, alarmSecond, alarmBits;
bool alarmDy, alarmH12Flag, alarmPmFlag;

byte Year;
byte Month;
byte Date;
byte DoW;
byte Hour;
byte Minute;
byte Second;

void GetDateStuff(byte& Year, byte& Month, byte& Day, byte& DoW, 
    byte& Hour, byte& Minute, byte& Second) {
  // Call this if you notice something coming in on 
  // the serial port. The stuff coming in should be in 
  // the order YYMMDDwHHMMSS, with an 'x' at the end.
  boolean GotString = false;
  char InChar;
  byte Temp1, Temp2;
  char InString[20];

  byte j=0;
  while (!GotString) {
    if (Serial.available()) {
      InChar = Serial.read();
      InString[j] = InChar;
      j += 1;
      if (InChar == 'x') {
        GotString = true;
      }
    }
  }
  Serial.println(InString);
  // Read Year first
  Temp1 = (byte)InString[0] -48;
  Temp2 = (byte)InString[1] -48;
  Year = Temp1*10 + Temp2;
  // now month
  Temp1 = (byte)InString[2] -48;
  Temp2 = (byte)InString[3] -48;
  Month = Temp1*10 + Temp2;
  // now date
  Temp1 = (byte)InString[4] -48;
  Temp2 = (byte)InString[5] -48;
  Day = Temp1*10 + Temp2;
  // now Day of Week
  DoW = (byte)InString[6] - 48;   
  // now Hour
  Temp1 = (byte)InString[7] -48;
  Temp2 = (byte)InString[8] -48;
  Hour = Temp1*10 + Temp2;
  // now Minute
  Temp1 = (byte)InString[9] -48;
  Temp2 = (byte)InString[10] -48;
  Minute = Temp1*10 + Temp2;
  // now Second
  Temp1 = (byte)InString[11] -48;
  Temp2 = (byte)InString[12] -48;
  Second = Temp1*10 + Temp2;
}
void setup() {
  tft.init(240, 320);
  tft.setTextSize(2);
  // Start the I2C interface 
  Wire.begin(21, 22);
 
  // Start the serial interface
  Serial.begin(9600);
}



void loop() {
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextWrap(true);
  //tft.print("hello world");
  //DateTime now = rtc.now();
  tft.setCursor(0, 30);
  tft.setTextColor(ST77XX_WHITE);
  tft.print(rtc.getYear(), DEC);
  tft.print('/');
  tft.print(rtc.getMonth(century), DEC);
  tft.print('/');
  tft.print(rtc.getDate(), DEC);
  tft.print(' ');
  tft.print(rtc.getHour(h12Flag, pmFlag), DEC);
  tft.print(':');
  tft.print(rtc.getMinute(), DEC);
  tft.print(':');
  tft.print(rtc.getSecond(), DEC);
  tft.println();


  // send what's going on to the serial monitor.
  
  // Start with the year
  Serial.print("2");
  if (century) {      // Won't need this for 89 years.
    Serial.print("1");
  } else {
    Serial.print("0");
  }
  Serial.print(rtc.getYear(), DEC);
  Serial.print(' ');
  
  // then the month
  Serial.print(rtc.getMonth(century), DEC);
  Serial.print(" ");
  
  // then the date
  Serial.print(rtc.getDate(), DEC);
  Serial.print(" ");
  
  // and the day of the week
  Serial.print(rtc.getDoW(), DEC);
  Serial.print(" ");
  
  // Finally the hour, minute, and second
  Serial.print(rtc.getHour(h12Flag, pmFlag), DEC);
  Serial.print(" ");
  Serial.print(rtc.getMinute(), DEC);
  Serial.print(" ");
  Serial.print(rtc.getSecond(), DEC);
 
  // Add AM/PM indicator
  if (h12Flag) {
    if (pmFlag) {
      Serial.print(" PM ");
    } else {
      Serial.print(" AM ");
    }
  } else {
    Serial.print(" 24h ");
  }
 
  // Display the temperature
  Serial.print("T=");
  Serial.print(rtc.getTemperature(), 2);
  
  // Tell whether the time is (likely to be) valid
  if (rtc.oscillatorCheck()) {
    Serial.print(" O+");
  } else {
    Serial.print(" O-");
  }
 
  // Indicate whether an alarm went off
  if (rtc.checkIfAlarm(1)) {
    Serial.print(" A1!");
  }
 
  if (rtc.checkIfAlarm(2)) {
    Serial.print(" A2!");
  }
 
  // New line on display
  Serial.println();
  
  // Display Alarm 1 information
  Serial.print("Alarm 1: ");
  rtc.getA1Time(alarmDay, alarmHour, alarmMinute, alarmSecond, alarmBits, alarmDy, alarmH12Flag, alarmPmFlag);
  Serial.print(alarmDay, DEC);
  if (alarmDy) {
    Serial.print(" DoW");
  } else {
    Serial.print(" Date");
  }
  Serial.print(' ');
  Serial.print(alarmHour, DEC);
  Serial.print(' ');
  Serial.print(alarmMinute, DEC);
  Serial.print(' ');
  Serial.print(alarmSecond, DEC);
  Serial.print(' ');
  if (alarmH12Flag) {
    if (alarmPmFlag) {
      Serial.print("pm ");
    } else {
      Serial.print("am ");
    }
  }
  if (rtc.checkAlarmEnabled(1)) {
    Serial.print("enabled");
  }
  Serial.println();
 
  // Display Alarm 2 information
  Serial.print("Alarm 2: ");
  rtc.getA2Time(alarmDay, alarmHour, alarmMinute, alarmBits, alarmDy, alarmH12Flag, alarmPmFlag);
  Serial.print(alarmDay, DEC);
  if (alarmDy) {
    Serial.print(" DoW");
  } else {
    Serial.print(" Date");
  }
  Serial.print(" ");
  Serial.print(alarmHour, DEC);
  Serial.print(" ");
  Serial.print(alarmMinute, DEC);
  Serial.print(" ");
  if (alarmH12Flag) {
    if (alarmPmFlag) {
      Serial.print("pm");
    } else {
      Serial.print("am");
    }
  }
  if (rtc.checkAlarmEnabled(2)) {
    Serial.print("enabled");
  }
 
  // display alarm bits
    Serial.println();
  Serial.print("Alarm bits: ");
  Serial.println(alarmBits, BIN);

  Serial.println();
  delay(1000);

  if (Serial.available()) {
    GetDateStuff(Year, Month, Date, DoW, Hour, Minute, Second);

    rtc.setClockMode(false);  // set to 24h
    //setrtcMode(true); // set to 12h

    rtc.setYear(Year);
    rtc.setMonth(Month);
    rtc.setDate(Date);
    rtc.setDoW(DoW);
    rtc.setHour(Hour);
    rtc.setMinute(Minute);
    rtc.setSecond(Second);
  }
}
