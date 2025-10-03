// Created with Gemini 2.5 flash
// Sketch to set the current time on the DS1307 RTC
// Compile and upload this sketch ONCE when your computer's clock is correct.

#include <Wire.h>
#include <RTClib.h>

// Create an RTC object
RTC_DS1307 rtc;

void setup() {
  Serial.begin(9600);
  
  // Initialize I2C communication with the RTC
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort();
  }
   // --- THIS IS THE CRITICAL LINE ---
  // It sets the RTC to the date & time the sketch was compiled.
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  
  Serial.println("RTC time has been set!");
  Serial.println("You can now upload your main project sketch.");

}

void loop() {
  // put your main code here, to run repeatedly:

}
