// Initiate Components
const int boardLed = 13;
int led = 9;

//Variable to set before program start
int ledState = LOW;
int brightness = 0;
int fadeAmount = 5;

// Millis() timing
unsigned long previousBoard = 0;
unsigned long previousLed = 0;
const long boardInterval = 1000;

void setup() {
  // put your setup code here, to run once:
  pinMode(boardLed, OUTPUT);
  pinMode(led,OUTPUT);
  analogWrite(led, brightness);
  
  Serial.begin(9600);
}

void loop() {
  
  // Onboard LED (Status Light)
  unsigned long currentMillis = millis();
  if (currentMillis - previousBoard >= boardInterval) {
    if (ledState == LOW) {ledState = HIGH;}
    else { ledState = LOW;}
    Serial.print("Led is set to ");
    Serial.println(ledState);
    previousBoard = currentMillis;
    digitalWrite(boardLed, ledState);
  }
  //External LED
 if (currentMillis - previousLed >= 30) {
   brightness = brightness + fadeAmount;
   Serial.print("Led Brightness is ");
   Serial.println(brightness);
   if (brightness <= 0 || brightness >= 255) {fadeAmount = -fadeAmount;}
  analogWrite(led, brightness);
  previousLed = currentMillis;
 }
 

  // Update Components
  
  
  
}
