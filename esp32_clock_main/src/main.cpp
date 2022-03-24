#include <Arduino.h>
#include <config.h>

// lists to store data about the touch pins
bool touchPins[20];
unsigned long long lastTouch[10];

// initialize interrupt service routines for each touch pin
// TODO: find a macro for this
#ifndef nosettouchpins
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

void gotTouch5(){
  touchPins[5] = true;
}

void gotTouch6(){
  touchPins[6] = true;
}

void gotTouch7(){
  touchPins[7] = true;
}
#endif

// general function to check if a touch pad has been pressed
bool getTouch(int pin){
  if (touchPins[pin] && (millis() > (lastTouch[pin]+TOUCH_WAIT))){
    touchPins[pin] = false;
    lastTouch[pin] = millis();
    return true;
  }
  return false;
}

// function to print the time to Serial
void printLocalTime() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

// custom display print functions to print text onto the display with lower refresh time
void tftPrint(String s, int16_t x, int16_t y, uint32_t bgcolor = ST77XX_BLACK, uint32_t txtcolor = ST77XX_WHITE, uint8_t border = 0, bool fullscreen = true) {
  int16_t  x1, y1;
  uint16_t w, h;
  tft.getTextBounds(s, x, y, &x1, &y1, &w, &h);
  if (border == 0) {
    //tft.fillRect(x1, y1, w, h, bgcolor);
    tft.fillRect(x1-1, y1-1, w+(2*1), h+(2*1), bgcolor);
  } else {
    if (fullscreen) {
      tft.fillRect(x1-border, y1-border, tft.width(), h+(2*border), bgcolor);
    } else {
      tft.fillRect(x1-border, y1-border, w+(2*border), h+(2*border), bgcolor);
    }
  }
  tft.setTextColor(txtcolor);
  tft.setCursor(x, y);
  tft.print(s);
}
void tftPrintln(String s, uint32_t color = ST77XX_BLACK) { // tft print newline
  int16_t  x1, y1;
  uint16_t w, h;
  tft.getTextBounds(s, tft.getCursorX(), tft.getCursorY(), &x1, &y1, &w, &h);
  tft.fillRect(x1, y1, w, h, color);
  tft.println(s);
}
// void tftPrintNP(String s, uint32_t color = ST77XX_BLACK) { // tft print no position.
//   int16_t  x1, y1;
//   uint16_t w, h;
//   tft.getTextBounds(s, tft.getCursorX(), tft.getCursorY(), &x1, &y1, &w, &h);
//   tft.fillRect(x1, y1, w, h, color);
//   tft.println(s);
// }

// function to show time on screen
void showTimeOnScreen() {
  tft.setCursor(0, BASE_FONT_HEIGHT);
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
  strftime(longdate, sizeof(longdate), "%A, %B %d                 ", &timeinfo);
  strftime(timedate, sizeof(timedate), "%H:%M:%S %d/%m/%Y         ", &timeinfo);
  strftime(shortime, sizeof(shortime), "%H:%M ", &timeinfo);
  
  tftPrintln(longdate);
  tftPrintln(timedate);

  tft.setFont(&FreeSans24pt7b);
  tft.setTextSize(2);
  tftPrint(shortime, 0, 200);
}

// #define NUM_ALARMS 2
// struct tm alarms[NUM_ALARMS];
// bool alarmenabled[NUM_ALARMS];
// uint8_t tmpHour = 0;
// uint8_t tmpMinute = 0;


String fix2digits(uint8_t n) {
  String res = String(n);
  if (res.length() < 2) {
    res = "0" + res;
  }
  return res;
}

// void printAlarmSetTime() {
//   tft.setCursor(0,BASE_FONT_HEIGHT);
//   tftPrintNP("Set alarm time: ");
//   tftPrintNP(fix2digits(tmpHour));
//   tftPrintNP(fix2digits(tmpMinute));
// }

struct alarmData {
  bool enabled;
  uint8_t min;
  uint8_t hour; 
  //bool wday[8];
};
uint8_t changeidx = 0;
alarmData alarm1 = {false, 0, 0};

// #define NUM_ALARMS 2
// struct alarmData alarms[NUM_ALARMS];
// bool alarmenabled[NUM_ALARMS];

void constrainAlarmTime(alarmData &alm) {
  if (alm.hour > 24) {
    alm.hour = 0;
  }
  if (alm.hour > 200) {
    alm.hour = 0;
  }
  if (alm.min > 24) {
    alm.min = 0;
  }
  if (alm.min > 200) {
    alm.min = 0;
  }
}

void setAlarm() {
  //tft.fillScreen(ST77XX_BLACK);
  //Serial.print("run set alarm");

  // initialize text stuff
  tft.setTextSize(1); 
  tft.setFont(&FreeSans12pt7b);
  
  // print prompt message
  tft.setCursor(0,20);
  tft.print("Set alarm time: ");
  
  // set font for large letters
  tft.setFont(&FreeSans24pt7b);
  tft.setTextSize(2);

  if (changeidx == 0) {
    tftPrint(fix2digits(alarm1.hour), 20, 100, ST77XX_WHITE, ST77XX_BLACK, SEL_BORDER_WIDTH, false);
    if (getTouch(UP_P)) {
      //Serial.println("Up pressed!");
      alarm1.hour++;
    }
    if (getTouch(DOWN_P)) {
      //Serial.println("Down pressed!");
      alarm1.hour--;
    }
  } else {
    tftPrint(fix2digits(alarm1.hour), 20, 100, ST77XX_BLACK, ST77XX_WHITE, SEL_BORDER_WIDTH);
  }

  tftPrint(":", tft.getCursorX(), 100, ST77XX_BLACK, ST77XX_WHITE, SEL_BORDER_WIDTH);

  if (changeidx == 1) {
    tftPrint(fix2digits(alarm1.min), tft.getCursorX(), 100, ST77XX_WHITE, ST77XX_BLACK, SEL_BORDER_WIDTH, false);
    if (getTouch(UP_P)) {
      //Serial.println("Up pressed!");
      alarm1.min++;
    }
    if (getTouch(DOWN_P)) {
      //Serial.println("Down pressed!");
      alarm1.min--;
    }
  } else {
    tftPrint(fix2digits(alarm1.min), tft.getCursorX(), 100, ST77XX_BLACK, ST77XX_WHITE, SEL_BORDER_WIDTH);
  }

  // reset font
  tft.setTextSize(1); 
  tft.setFont(&FreeSans9pt7b);

  if (getTouch(SEL_P)) {
    //Serial.print("increasing change idx");
    changeidx++;
  }
  if (getTouch(BACK_P)) {
    //Serial.print("decreasing change idx");
    changeidx--;
  }
  if (changeidx > 1) {
    touchPins[BACK_P] = true;
  }
  if (changeidx < 0) {
    changeidx = 0;
  }
  
  constrainAlarmTime(alarm1);
  //Serial.print(changeidx);
  
}

void saveAlarm() {
  tft.setTextSize(1); 
  tft.setFont(&FreeSans9pt7b);
  
  // print prompt message
  tft.setCursor(0,BASE_FONT_HEIGHT);
  tft.print("Alarm saved!");
  alarm1.enabled = true;
  delay(1000);
  tft.fillScreen(ST77XX_BLACK);
}

void listAlarm() {
  tft.setTextSize(1); 
  tft.setFont(&FreeSans12pt7b);
  tftPrint("Alarm time:", 0, 20);
  tftPrint(fix2digits(alarm1.hour) + ":" + fix2digits(alarm1.min), tft.getCursorX(), 20);
}

uint8_t volume = 0;
void setVolume() {

  // initialize text stuff
  tft.setTextSize(1); 
  tft.setFont(&FreeSans12pt7b);
  
  // print prompt message
  tft.setCursor(0,20);
  tft.print("Set volume: ");
  
  // set font for large letters
  tft.setFont(&FreeSans24pt7b);
  tft.setTextSize(2);

  tftPrint(fix2digits(volume), 20, 100);

  // reset font
  tft.setTextSize(1); 
  tft.setFont(&FreeSans9pt7b);
  if (getTouch(UP_P)) {
    volume++;
  }
  if (getTouch(DOWN_P)) {
    volume--;
  }
  if (getTouch(SEL_P)) {
    MP3.volume(volume*3);
    touchPins[BACK_P] = true;
  }

  if (volume > 10) {
    volume = 10;
  }
  if (volume < 0) {
    volume = 0;
  }
  //Serial.print(changeidx);
  
}

void toggleAlarm() {

  // initialize text stuff
  tft.setTextSize(1); 
  tft.setFont(&FreeSans12pt7b);
  
  // print prompt message
  tft.setCursor(0,20);
  tft.print("Alarm is: ");
  
  // set font for large letters
  tft.setFont(&FreeSans24pt7b);
  tft.setTextSize(2);
  if (alarm1.enabled) {
    tftPrint("On", 20, 100);
  } else {
    tftPrint("Off", 20, 100);
  }

  // reset font
  tft.setTextSize(1); 
  tft.setFont(&FreeSans9pt7b);
  if (getTouch(UP_P) || getTouch(DOWN_P)) {
    alarm1.enabled = !alarm1.enabled;
  }
  if (getTouch(SEL_P)) {
    touchPins[BACK_P] = true;
  }
  //Serial.print(changeidx);
  
}

void testAudio() {
  tft.setTextSize(1); 
  tft.setFont(&FreeSans12pt7b);
  tft.setCursor(0,20);
  tft.print("Testing audio, press X to exit ");
  MP3.play(2);
  if (getTouch(BACK_P)) {
    MP3.pause();
    touchPins[BACK_P] = true;
  }
}

int16_t getlineY(uint8_t bwidth, uint16_t fheight, uint8_t line) {
  int16_t res;
  res = fheight*(line+1) + bwidth*line + (bwidth+1)*(line+1);
  return res;
}

#define NUM_SELS 5
bool inMenu = false;
int curSelection = 0;
typedef void (*func) (void);

const char *selections[] = {"Set alarm", "Alarm list", "Set alarm volume", "Alarm on/off", "Speaker test"};
func mainFuncList[NUM_SELS] = {setAlarm, listAlarm, setVolume, toggleAlarm, testAudio};
//func exitFuncList[NUM_SELS] = {saveAlarm};

#define MENU_FONT_HEIGHT 17
uint8_t menuPos = -1;
//uint8_t cursoridx = 0;
uint8_t oldidx = 0;
uint8_t progidx = -1;
bool inProgram = false;
void showMenu() {
  tft.setCursor(SEL_BORDER_WIDTH, SEL_BORDER_WIDTH+MENU_FONT_HEIGHT);
  tft.setTextSize(1);
  
  if (getTouch(UP_P)) {
    menuPos -= 1; 
  }
  if (getTouch(DOWN_P)) {
    
    menuPos += 1;
  }
  if (menuPos > NUM_SELS -1) {
    menuPos = NUM_SELS - 1;
  }
  if (menuPos < 0) {
    menuPos = 0;
  }

  if (menuPos != oldidx) {
    Serial.println(menuPos);
    for (int i = 0; i < NUM_SELS; i++) {
      if (i == menuPos) {
        // draw selected text
        tftPrint(selections[i], SEL_BORDER_WIDTH, getlineY(SEL_BORDER_WIDTH, MENU_FONT_HEIGHT, i), ST77XX_WHITE, ST77XX_BLACK, SEL_BORDER_WIDTH);
        // int16_t fx, fy; // fill x, fill y
        // uint16_t fw, fh; // fill width, fill height
        // tft.getTextBounds(selections[i], tft.getCursorX(), tft.getCursorY(), &fx, &fy, &fw, &fh);
        // tft.fillRect(fx-SEL_BORDER_WIDTH, fy-SEL_BORDER_WIDTH, tft.width(), fh+(2*SEL_BORDER_WIDTH), ST77XX_WHITE);
        // tft.setTextColor(ST77XX_BLACK);
        // tft.println(selections[i]);
        // tft.setTextColor(ST77XX_WHITE);
      } else {
        // draw normal text
        tftPrint(selections[i], SEL_BORDER_WIDTH, getlineY(SEL_BORDER_WIDTH, MENU_FONT_HEIGHT, i), ST77XX_BLACK, ST77XX_WHITE, SEL_BORDER_WIDTH);
      }
    }

    
  }
  if (getTouch(SEL_P)) {
      // Serial.print("Touched pad. ");
      // Serial.println(menuPos);

      tft.fillScreen(ST77XX_BLACK);
      inProgram = true;
      inMenu = false;
      progidx = menuPos;
      mainFuncList[menuPos]();
  }
  oldidx = menuPos;
}

void setup() {
  // setup pulse width modulation for LED strips and TFT backlight
  ledcSetup(WARM_CH, PWM_FREQ, PWM_RES);
  ledcSetup(COOL_CH, PWM_FREQ, PWM_RES);
  ledcSetup(TFT_BL_CH, PWM_FREQ, PWM_RES);
  ledcAttachPin(WARM_PIN, WARM_CH);
  ledcAttachPin(COOL_PIN, COOL_CH);
  ledcAttachPin(TFT_BL, TFT_BL_CH);

  // setup capacitive touch pad interrupts
  touchAttachInterrupt(T2, gotTouch2, TOUCH_SENS);
  touchAttachInterrupt(T4, gotTouch4, TOUCH_SENS);
  touchAttachInterrupt(T5, gotTouch5, TOUCH_SENS);
  touchAttachInterrupt(T6, gotTouch6, TOUCH_SENS);
  touchAttachInterrupt(T7, gotTouch7, TOUCH_SENS);

  // initialize Serial for debugging
  Serial.begin(115200);

  // initialize TFT LCD to display
  tft.init(240, 320);
  tft.setTextSize(1);
  tft.setRotation(3);
  tft.setFont(&FreeSans9pt7b);
  tft.setTextColor(ST77XX_WHITE);
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0,BASE_FONT_HEIGHT);

  // setup TFT backlight PWM
  ledcWrite(TFT_BL_CH, map(analogRead(LIGHT_SENS), 0, 4095, 1, 256));

  // connect to WiFi
  Serial.printf("Connecting to %s ", ssid);
  tft.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { // wait until WiFi connects
      delay(1000);
      Serial.print(".");
      tft.print(".");
  }
  Serial.println(" CONNECTED");
  tft.print(" CONNECTED");

  // configure local time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();

  // disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);

  tft.fillScreen(ST77XX_BLACK);

  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  
  DFSerial.begin(9600);
  if (!MP3.begin(DFSerial)) {  // Use SoftwareSerial to communicate with mp3.
    tft.setCursor(0,BASE_FONT_HEIGHT);
    tft.print("MP3 player error: not connected!");
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(!Serial);
  }
  Serial.println(F("DFPlayer Mini online."));

  MP3.volume(5);  //Set volume value. From 0 to 30
  //Play the first mp3

  getTouch(SEL_P);
  tft.fillScreen(ST77XX_BLACK);
}
unsigned long long lastTime;

int lightState = 256;
void loop() {
  if (!(inMenu || inProgram)) {
    if (millis() >= lastTime + 1000) {
      lastTime = millis();
      showTimeOnScreen();
    }
    if (getTouch(SEL_P)) {
      //Serial.println("SEL button pressed!");
      tft.setFont(&FreeSans12pt7b);
      inMenu = true;
      oldidx = -1;
      tft.fillScreen(ST77XX_BLACK);
      showMenu();
    }
  } else if (inMenu){
    showMenu();
    if (getTouch(BACK_P)) {
      //Serial.println("Back button pressed!");
      inMenu = false;
      tft.setFont(&FreeSans9pt7b);
      tft.fillScreen(ST77XX_BLACK);
    }
  } else {
    mainFuncList[progidx]();
    if (getTouch(BACK_P)) {
      //Serial.println("Back button pressed!");
      inProgram = false;
      //exitFuncList[progidx]();
      tft.setFont(&FreeSans9pt7b);
      tft.fillScreen(ST77XX_BLACK);
    }
  }


  ledcWrite(TFT_BL_CH, map(analogRead(34), 0, 4095, 1, 256));
  ledcWrite(WARM_CH, lightState);
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