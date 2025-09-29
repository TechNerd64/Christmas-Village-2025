const int ledPin = 13;
int ledState = LOW;
unsigned long previousMillis = 0;
const long interval = 1000;

void setup() {
  // put your setup code here, to run once:
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long currentMillis = millis();
  Serial.println(currentMillis);
  delay(100);

}
