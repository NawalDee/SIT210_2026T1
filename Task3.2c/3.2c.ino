#include <WiFiNINA.h>
#include <PubSubClient.h>

// WiFi
char ssid[] = "NetComm 3225";
char pass[] = "decefacuwa";

// MQTT
char broker[] = "broker.emqx.io";

WiFiClient wifiClient;
PubSubClient client(wifiClient);

// Pins
int trigPin = 2;
int echoPin = 3;
int led1 = 5;
int led2 = 6;

long duration;
int distance;

void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";

  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.println(message);

  if (String(topic) == "ES/Wave") {
    digitalWrite(led1, HIGH);
    digitalWrite(led2, HIGH);
  }

  if (String(topic) == "ES/Pat") {
    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);
  }
}

void setup() {
  Serial.begin(9600);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);

  // WiFi connect
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("Connected to WiFi");

  client.setServer(broker, 1883);
  client.setCallback(callback);

  connectMQTT();
}

void connectMQTT() {
  while (!client.connected()) {
    if (client.connect("NawalClient")) {
      Serial.println("MQTT connected");

      client.subscribe("ES/Wave");
      client.subscribe("ES/Pat");

    } else {
      delay(2000);
    }
  }
}

void loop() {
  client.loop();

  // Ultrasonc read
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;

  Serial.print("Distance: ");
  Serial.println(distance);

  // Wave close hand
  if (distance < 10) {
    client.publish("ES/Wave", "Nawal Wave");
    delay(2000);
  }

  // Pat far hand
  if (distance > 30) {
    client.publish("ES/Pat", "Nawal Pat");
    delay(2000);
  }
}