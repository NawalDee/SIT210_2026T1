#include <DHT.h>
#include <WiFiNINA.h>
#include <ThingSpeak.h>

//DHT sensor
#define DHTPIN 3
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Analog
int lightPin = A0;   

// WiFi
char ssid[] = "NetComm 3225";     
char pass[] = "decefacuwa"; 
WiFiClient client;

//ThingSpeak 
unsigned long channelNumber = 3308136;
const char * writeAPIKey = "IFW86MVM7FA5PGG5";

// FUNCTIONS

// read temperature
float readTemperature() {
  float temp = dht.readTemperature();

  // if the read is wrong we back to 0
  if (isnan(temp)) {
    Serial.println("Error reading temperature");
    return 0;
  }

  return temp;
}

// read light
int readLight() {
  return analogRead(lightPin); // it gives from 0 to 1023
}

// send data to ThingSpeak
void sendData(float temp, int light) {
  ThingSpeak.setField(1, temp);
  ThingSpeak.setField(2, light);
  ThingSpeak.writeFields(channelNumber, writeAPIKey);
}

void setup() {
  Serial.begin(9600);
  dht.begin();

  // connect to wifi
  WiFi.begin(ssid, pass);

  // waiting for wifi call
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nWiFi Connected!");

  ThingSpeak.begin(client);
}

void loop() {

  // read
  float temperature = readTemperature();
  int lightValue = readLight();

  // print Serial Monitor
  Serial.print("Temp: ");
  Serial.println(temperature);

  Serial.print("Light: ");
  Serial.println(lightValue);

  // send data to web
  sendData(temperature, lightValue);

  delay(30000);
}