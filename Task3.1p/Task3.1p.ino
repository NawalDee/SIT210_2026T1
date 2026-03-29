#include <WiFiNINA.h>
#include <BH1750.h>
#include <Wire.h>

// WiFi
char ssid[] = "Nawal16";
char pass[] = "nawaldee";

WiFiClient client;

char HOST_NAME[] = "maker.ifttt.com";
String PATH_NAME = "/trigger/light_detected/with/key/c-6vA8zDMuA6YS6SFWDWS";

// BH1750
BH1750 lightMeter;
float threshold = 50.0;

bool isDark = false;

void setup() {
  Serial.begin(9600);
  delay(1000);

  // I2C start
  Wire.begin();
  lightMeter.begin();

  // WiFi connect
  Serial.print("Connecting to WiFi...");
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected!");
}

void loop() {

  float lux = lightMeter.readLightLevel();

  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lx");

  // Dark
  if (lux < threshold && !isDark) {
    Serial.println("Dark - Notification sent");
    sendIFTTT("Dark");
    isDark = true;
  }

  // Light
  if (lux >= threshold && isDark) {
    Serial.println("Light - Notification sent");
    sendIFTTT("Light");
    isDark = false;
  }

  delay(2000);
}

// IFTTT
void sendIFTTT(String value) {

  if (client.connect(HOST_NAME, 80)) {

    String url = PATH_NAME + "?value1=" + value;

    client.println("GET " + url + " HTTP/1.1");
    client.println("Host: " + String(HOST_NAME));
    client.println("Connection: close");
    client.println();

    Serial.println("Notification sent to IFTTT");

    delay(500);

    while (client.available()) {
      char c = client.read();
      Serial.print(c);
    }

    client.stop();

  } else {
    Serial.println("Connection failed");
  }
}