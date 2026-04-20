#include <Wire.h>
#include <BH1750.h>

// BH1750 light sensor
BH1750 lightMeter;

// Pins
const int led1 = 5;
const int led2 = 6;
const int pirPin = 2;     
const int switchPin = 3;  

// Variables
float threshold = 50.0;   // light threshold
volatile bool motionDetected = false;
volatile bool switchPressed = false;

void setup() {
  Serial.begin(9600);
  delay(1000);

  // Start I2C
  Wire.begin();
  lightMeter.begin();

  // LED pins
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);

  // PIR sensor
  pinMode(pirPin, INPUT);

  // Switch (using internal pull-up)
  pinMode(switchPin, INPUT_PULLUP);

  // Attach interrupts
  attachInterrupt(digitalPinToInterrupt(pirPin), motionISR, RISING);
  attachInterrupt(digitalPinToInterrupt(switchPin), switchISR, FALLING);

  Serial.println("System Ready...");
}

// Interrupt for PIR
void motionISR() {
  motionDetected = true;
}

// Interrupt for Switch
void switchISR() {
  switchPressed = true;
}

void loop() {
  float lux = lightMeter.readLightLevel();

  // PIR triggered
  if (motionDetected) {
    motionDetected = false;

    Serial.println("Motion detected!");

    if (lux < threshold) {
      Serial.println("It's dark → Lights ON");
      turnOnLights();
    } else {
      Serial.println("It's bright → No need to turn lights ON");
    }
  }

  // Switch triggered
  if (switchPressed) {
    switchPressed = false;

    Serial.println("Switch pressed → Lights ON");
    turnOnLights();
  }
}

// Function to turn ON both LEDs
void turnOnLights() {
  digitalWrite(led1, HIGH);
  digitalWrite(led2, HIGH);

  delay(5000); 

  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);

  Serial.println("Lights OFF");
}