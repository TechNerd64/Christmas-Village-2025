// This is the MAIN code for my Christmas Village
// Please don't make commits unless they are proven and tested in in other projects

//Setup Libraries
#include <Arduino.h>;
#include <Wire.h>;
#include <RTClib.h>;
#include <LiquidCrystal_I2C.h>;
#include <avr/pgmspace.h>;

// === NEW BUZZER DEFINITIONS ===
#define ARRAY_LEN(array) (sizeof(array) / sizeof(array[0]))
#define D5 587
#define E5 659
#define C5 523
#define B4 494
#define A4 440
#define G4 392
#define D4 294

//Initialize Components
const int lamp1 = 2;
const int lamp2 = 3;
const int lamp3 = 4;
LiquidCrystal_I2C lcd(0x27, 16, 2);
RTC_DS1307 rtc;
const int buzzerPin = 32;

//Millis Timing Var.
unsigned long currentMillis = 0;

unsigned long previousMillis1 = 0;
unsigned long previousMillis2 = 0;
unsigned long previousMillis3 = 0;
unsigned long previousRTC = 0;
unsigned long lamp1Timing = 0;
unsigned long lamp2Timing = 0;
unsigned long lamp3Timing = 0;
const long RTCtiming = 60000;

//Countdown Timing
const long targetDate = 1766646000;

// New Song Array (Moved to PROGMEM for flash storage)
const int midi1[24][3] PROGMEM = {
  {D5, 1200, 0},
  {E5, 300, 0},
  {D5, 300, 0},
  {C5, 300, 0},
  {B4, 300, 0},
  {C5, 1200, 0},
  {D5, 300, 0},
  {C5, 300, 0},
  {B4, 300, 0},
  {A4, 300, 0},
  {B4, 1200, 0},
  {C5, 300, 0},
  {B4, 300, 0},
  {A4, 300, 0},
  {G4, 300, 0},
  {A4, 900, 0},
  {D4, 300, 0},
  {D4, 1200, 0},
  {G4, 600, 0},
  {A4, 600, 0},
  {B4, 600, 0},
  {C5, 600, 0},
  {B4, 1200, 0},
  {A4, 600, 0},
};
unsigned long previousSongTime = 0;
const long songInterval = 10000;
unsigned long noteStartTime = 0;
bool isPlaying = false;
int currentNoteIndex = 0;



void setup() {
Wire.begin();
rtc.begin();
DateTime now = rtc.now();
long elapsedSec = targetDate - now.unixtime();
long daysTill = elapsedSec / 86400;
lcd.clear();
lcd.setCursor(0,0);
lcd.print("Only ");
lcd.print(daysTill + 1); 
lcd.print(" days");
lcd.setCursor(0,1);
lcd.print("until Christmas!");


tone(buzzerPin, 440, 2000);

//Start Pins
pinMode(lamp1, OUTPUT);
pinMode(lamp2, OUTPUT);
pinMode(lamp3, OUTPUT);
pinMode(buzzerPin, OUTPUT);

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
PlayMidiSong(buzzerPin, midi1, ARRAY_LEN(midi1));
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
  if(currentMillis - previousRTC >= RTCtiming) {
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

void PlayMidiSong(int pin, const int notes[][3], size_t len) {
  unsigned long now = millis();

  // Start song every 30 seconds
  if (!isPlaying && (now - previousSongTime >= songInterval)) {
    isPlaying = true;
    previousSongTime = now;
    currentNoteIndex = 0;
    noteStartTime = now;
  }

  if (!isPlaying) return;

  // read note from PROGMEM
  int note     = pgm_read_word_near(&notes[currentNoteIndex][0]);
  int duration = pgm_read_word_near(&notes[currentNoteIndex][1]);
  int rest     = pgm_read_word_near(&notes[currentNoteIndex][2]);
  long total   = (long)duration + rest;

  // If time has passed for this note → move to next
  if (now - noteStartTime >= total) {
    currentNoteIndex++;
    noteStartTime = now;

    // Done?
    if (currentNoteIndex >= len) {
      noTone(pin);
      isPlaying = false;
      return;
    }

    // Load next note
    note     = pgm_read_word_near(&notes[currentNoteIndex][0]);
    duration = pgm_read_word_near(&notes[currentNoteIndex][1]);
  }

  // Play tone if frequency > 0
  if (note > 0) {
    tone(pin, note, duration);
  } else {
    noTone(pin);
  }
}

