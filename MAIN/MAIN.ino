// This is the MAIN code for my Christmas Village
// Please don't make commits unless they are proven and tested in in other projects

//Setup Libraries
#include "Arduino.h";

//Initialize Components
const int lamp1 = 2;
const int lamp2 = 3;
const int lamp3 = 4;

//Millis Timing Var.
unsigned long previousMillis1 = 0;
unsigned long previousMillis2 = 0;
unsigned long previousMillis3 = 0;
unsigned long lamp1Timing = 0;
unsigned long lamp2Timing = 0;
unsigned long lamp3Timing = 0;

void setup() {
//Start Pins
pinMode(lamp1, OUTPUT);
pinMode(lamp2, OUTPUT);
pinMode(lamp3, OUTPUT);

//Millis Timing Var.
randomSeed(1000);

}

void loop() {
  // put your main code here, to run repeatedly:
LampFlicker();
}

void LampFlicker() {
  unsigned long currentMillis = millis();

  //Check to update LED flicker
  if(currentMillis - previousMillis1 >= lamp1Timing){
    previousMillis1 = currentMillis;
    lamp1Timing = random(60);
    analogWrite(lamp1, random(120)+135);
  }
  if(currentMillis - previousMillis2 >= lamp2Timing){
    previousMillis2 = currentMillis;
    lamp2Timing = random(60);
    analogWrite(lamp2, random(120)+135);
  }
  if(currentMillis - previousMillis3 >= lamp3Timing){
    previousMillis3 = currentMillis;
    lamp3Timing = random(60);
    analogWrite(lamp3, random(120)+135);
  }
}
