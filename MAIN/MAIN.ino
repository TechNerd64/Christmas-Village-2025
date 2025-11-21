// This is the MAIN code for my Christmas Village
// Please don't make commits unless they are proven and tested in in other projects

//Setup Libraries
#include <Arduino.h>;
#include <Wire.h>;
#include <RTClib.h>;
#include <LiquidCrystal_I2C.h>;


//Initialize Components
const int lamp1 = 2;
const int lamp2 = 3;
const int lamp3 = 4;
LiquidCrystal_I2C lcd(0x27, 16, 2);
RTC_DS1307 rtc;

//Millis Timing Var.
unsigned long currentMillis = 0;

unsigned long previousMillis1 = 0;
unsigned long previousMillis2 = 0;
unsigned long previousMillis3 = 0;
unsigned long previousRTC = 0;
unsigned long lamp1Timing = 0;
unsigned long lamp2Timing = 0;
unsigned long lamp3Timing = 0;
const long RTCtiming = 3600000;

//Countdown Timing
const long targetDate = 1766646000;


void setup() {
//Start Pins
pinMode(lamp1, OUTPUT);
pinMode(lamp2, OUTPUT);
pinMode(lamp3, OUTPUT);

//Millis Timing Var.
randomSeed(1000);

//Wire (I2C) stuff
Wire.begin();
lcd.init();
lcd.backlight();
Serial.begin(9600);

lcd.print("Loading...");

 // Check if the RTC module is connected and working
if (! rtc.begin()) {
    Serial.println("Error: Couldn't find RTC! Check wiring and power.");
    Serial.flush();
    // This stops the program if the RTC isn't found
while (1) delay(10); 
}

// OPTIONAL: Uncomment the two lines below to SET the RTC time on upload
 //Serial.println("Setting time on upload...");
 //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
 //Serial.println("Time is set!");
  
}

void loop() {
  // put your main code here, to run repeatedly:
currentMillis = millis();
LampFlicker();
Countdown();
}

void LampFlicker() {

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

void Countdown() {
 DateTime now = rtc.now();
  long elapsedSec = targetDate - now.unixtime();
  long daysTill = elapsedSec / 86400;
  if(currentMillis - previousRTC >= 60000) {
    previousRTC = currentMillis;
   
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Only ");
    lcd.print(daysTill + 1);
    lcd.print(" days");
    lcd.setCursor(0,1);
    lcd.print("until Christmas!");
  }

}
