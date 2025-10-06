//Needed Libraries
#include <Wire.h> // Necessary for I2C communication
#include <RTClib.h> // Library for RTC functions
#include <LiquidCrystal.h> // Library for 16x2 LCD
LiquidCrystal lcd(23, 33, 29, 27, 25, 31);


// Initiate Components
const int boardLed = 13;
int led = 9;
RTC_DS1307 rtc;

//Variable to set before program start
int ledState = LOW;
int brightness = 0;
int fadeAmount = 5;

// Millis() timing
unsigned long previousBoard = 0;
unsigned long previousLed = 0;
unsigned long previousRTC = 0;
const long boardInterval = 1000;
const long LedInterval = 30;
const long RTCInterval = 1000;

void setup() {
  // put your setup code here, to run once:
  pinMode(boardLed, OUTPUT);
  pinMode(led,OUTPUT);
  analogWrite(led, brightness);
  Wire.begin();
  lcd.begin(16, 2);
  lcd.print("Clock Starting:");

   // Check if the RTC module is connected and working
  if (! rtc.begin()) {
    Serial.println("Error: Couldn't find RTC! Check wiring and power.");
    Serial.flush();
    // This stops the program if the RTC isn't found
    while (1) delay(10); 
  }
  
  Serial.begin(9600);
}

void loop() {
  // Code to run every cycle
  DateTime now = rtc.now();
  unsigned long currentMillis = millis();
  
  // Onboard LED (Status Light)
  
  if (currentMillis - previousBoard >= boardInterval) {
    if (ledState == LOW) {ledState = HIGH;}
    else { ledState = LOW;}
    previousBoard = currentMillis;
    digitalWrite(boardLed, ledState);
  }
  //External LED
 if (currentMillis - previousLed >= LedInterval) {
   brightness = brightness + fadeAmount;
   if (brightness <= 0 || brightness >= 255) {fadeAmount = -fadeAmount;}
  analogWrite(led, brightness);
  previousLed = currentMillis;
 }
  //Time in Serial Moniter
  if (currentMillis - previousRTC >= RTCInterval) {
    previousRTC = currentMillis;
    // LCD Output:
    lcd.clear();
    // Serial Moniter Output:
    Serial.print(now.year(), DEC);
    Serial.print("/");
    Serial.print(now.month(), DEC);
    Serial.print("/");
    Serial.print(now.day(), DEC);

    Serial.print(" - ");

     Serial.print(now.hour(), DEC);
  Serial.print(':');
  
  if (now.minute() < 10) {Serial.print('0');} // Add leading zero for minutes
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  
  if (now.second() < 10) {Serial.print('0');} // Add leading zero for seconds
  Serial.print(now.second(), DEC);
  
  Serial.println(); // Start a new line

  }
 


  
  
  
}
