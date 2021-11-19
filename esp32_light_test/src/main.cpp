#include <Arduino.h>

// the number of the LED pin
const int ledPin = 22;  // 16 corresponds to GPIO16
const int led2Pin = 21;

// setting PWM properties
const int freq = 5000;
const int ledChannel = 0;
const int resolution = 8;
 
void setup(){
  // configure LED PWM functionalitites
  ledcSetup(ledChannel, freq, resolution);
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(ledPin, ledChannel);
}
 
void loop(){
  // increase the LED brightness
  for (int i = 0; i <= 255; i++) {
    ledcWrite(22, i);
    delay(15);
  }
  for (int i = 0; i <= 255; i++) {
    ledcWrite(21, i);
    delay(15);
  }
  for (int i = 255; i >= 0; i--) {
    ledcWrite(22, i);
    delay(15);
  }
  for (int i = 255; i >= 0; i--) {
    ledcWrite(21, i);
    delay(15);
  } 
}
