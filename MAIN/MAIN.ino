// This is the MAIN code for my Christmas Village
// Plays a MIDI song every time the hour changes, manages lamp flicker, and displays a Christmas countdown.

//Setup Libraries
#include <Arduino.h>
#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>
#include <avr/pgmspace.h>

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
// Note: Check your LCD address (0x27 or 0x3F are common)
LiquidCrystal_I2C lcd(0x27, 16, 2); 
RTC_DS1307 rtc;
const int buzzerPin = 32; // Assuming you are using a board where pin 32 is available for tone()

//Millis Timing Var.
unsigned long currentMillis = 0;

unsigned long previousMillis1 = 0;
unsigned long previousMillis2 = 0;
unsigned long previousMillis3 = 0;
unsigned long previousRTC = 0;
unsigned long lamp1Timing = 0;
unsigned long lamp2Timing = 0;
unsigned long lamp3Timing = 0;
const long RTCtiming = 60000; // Check RTC status once every minute

//Countdown Timing
const long targetDate = 1766646000; // Unix time for a future Christmas morning

// New Song Array (Moved to PROGMEM for flash storage)
const int midi1[24][3] PROGMEM = {
  // {Frequency, Duration, Rest}
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

// --- MODIFIED MIDI TIMING VARIABLES for Hourly Trigger ---
unsigned long previousSongTime = 0;
unsigned long noteStartTime = 0;
bool isPlaying = false;
int currentNoteIndex = 0;
int lastPlayedHour = -1; // New variable to track the last hour the song was played

// ------------------------------------------------------------------
// --- REQUIRED ARDUINO FUNCTIONS ---
// ------------------------------------------------------------------

void setup() {
  Wire.begin();
  rtc.begin();
  
  // Start Pins
  pinMode(lamp1, OUTPUT);
  pinMode(lamp2, OUTPUT);
  pinMode(lamp3, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  
  // Millis Timing Var.
  randomSeed(analogRead(0)); // Better random seed

  // I2C setup
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);

  // Check if the RTC module is connected and working
  if (! rtc.begin()) {
    Serial.println("Error: Couldn't find RTC! Check wiring and power.");
    lcd.clear();
    lcd.print("RTC Error!");
    Serial.flush();
    while (1) delay(10); 
  }
  
  // Set initial screen and buzzer test tone
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

  tone(buzzerPin, 440, 500); // Short tone for startup

  // Initialize the lastPlayedHour to the current hour to prevent playing immediately on startup
  lastPlayedHour = rtc.now().hour(); 
}

void loop() {
  currentMillis = millis();
  
  // These must be called continuously:
  LampFlicker();
  PlayMidiSong(buzzerPin, midi1, ARRAY_LEN(midi1)); 
  
  // This runs the RTC check/countdown display once per minute (or less often)
  Countdown(); 
}

// ------------------------------------------------------------------
// --- CUSTOM FUNCTIONS ---
// ------------------------------------------------------------------

void LampFlicker() {
  // Logic to randomly flicker the village lamps
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

  // Check RTC status (and time for song trigger) once per minute
  if(currentMillis - previousRTC >= RTCtiming) {
    previousRTC = currentMillis;

    // --- HOUR CHANGE CHECK & SONG TRIGGER ---
    // If the current hour is different from the last played hour AND the song is not already playing
    if (now.hour() != lastPlayedHour && !isPlaying) {
      // Start the song!
      isPlaying = true;
      lastPlayedHour = now.hour(); // Record the new hour
      currentNoteIndex = 0; // Reset to the first note
      noteStartTime = currentMillis; // Start timing for the first note
      previousSongTime = currentMillis; 
    }
    
    // Update the LCD display
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

  if (!isPlaying) return; // Only execute if the song is actively playing

  // read note from PROGMEM
  int note      = pgm_read_word_near(&notes[currentNoteIndex][0]);
  int duration  = pgm_read_word_near(&notes[currentNoteIndex][1]);
  int rest      = pgm_read_word_near(&notes[currentNoteIndex][2]);
  long total    = (long)duration + rest; // Total time for this note (sound + silence)

  // If the total time for the current note has passed, move to the next one
  if (now - noteStartTime >= total) {
    currentNoteIndex++;
    noteStartTime = now;

    // Done?
    if (currentNoteIndex >= len) {
      noTone(pin);
      isPlaying = false;
      return;
    }

    // Load next note (only necessary if we didn't finish)
    note      = pgm_read_word_near(&notes[currentNoteIndex][0]);
    duration  = pgm_read_word_near(&notes[currentNoteIndex][1]);
  }

  // Play tone if frequency > 0 (it's a note)
  if (note > 0) {
    // Check if we are still within the note's duration (sound time)
    if (now - noteStartTime < duration) {
       tone(pin, note, duration);
    } else {
       noTone(pin); // Rest period between notes
    }
  } else {
    noTone(pin); // Zero frequency means a rest
  }
}