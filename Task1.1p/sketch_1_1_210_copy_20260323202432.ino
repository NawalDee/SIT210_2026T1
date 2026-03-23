int porchLight = 2;      
int hallwayLight = 3; 
int buttonPin = 4;       

void setup() {
  pinMode(porchLight, OUTPUT);
  pinMode(hallwayLight, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP); 
}

void loop() {
  if (digitalRead(buttonPin) == LOW) {
    turnOnLights();
  }
}
void turnOnLights() {
  digitalWrite(porchLight, HIGH);
  digitalWrite(hallwayLight, HIGH);

  delay(30000);
  digitalWrite(porchLight, LOW);

  delay(30000);
  digitalWrite(hallwayLight, LOW);
}