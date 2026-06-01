#include <WiFiNINA.h>
#include <PubSubClient.h>
#include "HX711.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

#define DT 2
#define SCK 3

// HARDWARE INITIALIZATION 
HX711 scale;

char ssid[] = "NetComm 3225";
char pass[] = "decefacuwa";

char mqttServer[] = "broker.emqx.io";

WiFiClient wifiClient;
PubSubClient client(wifiClient);

int led1 = 5;
int led2 = 6;
int led3 = 7;
int buzzer = 8;

String medicineNames[3];
float medicineWeights[3];
int medicineCount = 0;

float totalWeight = 0;
String currentMedicine = "";
float currentMedicineWeight = 0;

bool monitoringStarted = false;
bool reminderActive = false;
bool removed = false;
bool notReturnedSent = false;
float weightAfterRemoval = 0;

unsigned long reminderTime = 0;
unsigned long removeTime   = 0;
unsigned long limit        = 60000;

// FUNCTION DECLARATIONS

void connectWiFi();
void reconnect();
void checkWeight();
void calibrateMedicine(String name);
void showMessage(String text);
void turnOffLEDs();
float readWeight();
void callback(char* topic, byte* payload, unsigned int length);

// SETUP

void setup()
{
  Serial.begin(9600);
  delay(3000);
  Serial.println("System Starting...");

  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(buzzer, OUTPUT);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("System Ready");

  scale.begin(DT, SCK);
  scale.set_scale(-7050);
  scale.tare();
  Serial.println("Scale Ready");

  connectWiFi();

  client.setServer(mqttServer, 1883);
  client.setCallback(callback);

  Serial.println("Setup Finished");
}

// LOOP

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }

  client.loop();

  if (monitoringStarted)
  {
    checkWeight();
  }
}

// READ WEIGHT

float readWeight()
{
  float w = scale.get_units(1);
  return abs(w);
}

// MQTT CALLBACK

void callback(char* topic,
              byte* payload,
              unsigned int length)
{
  String message = "";

  for (int i = 0; i < length; i++)
  {
    message += (char)payload[i];
  }

  Serial.print("Message: ");
  Serial.println(message);

  // CALIBRATION

  if (String(topic) == "medication/setup")
  {
    calibrateMedicine(message);
  }

  // START MONITORING

  if (String(topic) == "medication/start")
  {
    Serial.println("Monitoring Started");
    monitoringStarted = true;
    showMessage("Monitoring ON");
  }

  // REMINDER
  if (String(topic) == "medication/reminder")
  {
    currentMedicine       = message;
    reminderActive        = true;
    reminderTime          = millis();
    removed               = false;
    weightAfterRemoval    = 0;
    currentMedicineWeight = 0;
    notReturnedSent = false;

    // measure fresh total weight at reminder time
    totalWeight = abs(scale.get_units(20));

    // if totalWeight less than medicine weight
    // means calibration timing was off, correct it
    turnOffLEDs();

    if (medicineNames[0] == currentMedicine)
    {
      digitalWrite(led1, HIGH);
      currentMedicineWeight = medicineWeights[0];
    }

    if (medicineNames[1] == currentMedicine)
    {
      digitalWrite(led2, HIGH);
      currentMedicineWeight = medicineWeights[1];
    }

    if (medicineNames[2] == currentMedicine)
    {
      digitalWrite(led3, HIGH);
      currentMedicineWeight = medicineWeights[2];
    }

    // if totalWeight < medicineWeight something
    // is wrong, force totalWeight to be realistic
    if (totalWeight < currentMedicineWeight)
    {
      Serial.println("WARNING: totalWeight < medicineWeight");
      Serial.println("Fixing totalWeight...");
      totalWeight = currentMedicineWeight + 0.3;
    }

    Serial.print("Reminder For: ");
    Serial.println(currentMedicine);
    Serial.print("Medicine Weight: ");
    Serial.println(currentMedicineWeight);
    Serial.print("Total Weight NOW: ");
    Serial.println(totalWeight);
    Serial.print("Remove Threshold: ");
    Serial.println(totalWeight - (currentMedicineWeight * 0.7));

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Take Medicine:");
    lcd.setCursor(0, 1);
    lcd.print(currentMedicine);

    tone(buzzer, 1000);
  }
}

// CALIBRATION

void calibrateMedicine(String name)
{
  if (medicineCount >= 3)
  {
    Serial.println("Max Medicines Reached");
    return;
  }

  Serial.print("Calibrating: ");
  Serial.println(name);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Place Medicine:");
  lcd.setCursor(0, 1);
  lcd.print(name);

  float weight = 0;

  while (abs(weight) < 1)
  {
    weight = scale.get_units(5);

    float display = abs(weight);

    Serial.print("Waiting Weight: ");
    Serial.println(display);

    lcd.setCursor(0, 2);
    lcd.print("Weight: ");
    lcd.print(display);
    lcd.print(" g   ");

    delay(200);
  }

  weight = abs(scale.get_units(20));

  Serial.print("Final Weight: ");
  Serial.println(weight);

  medicineNames[medicineCount]   = name;
  medicineWeights[medicineCount] = weight;
  medicineCount++;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(name);
  lcd.setCursor(0, 1);
  lcd.print("Saved");
  lcd.setCursor(0, 2);
  lcd.print(weight);
  lcd.print(" g");

  Serial.println("Medicine Saved");

  delay(3000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Remove ");
  lcd.print(name);

  delay(3000);
}

// CHECK WEIGHT

void checkWeight()
{
  if (!reminderActive)
  {
    return;
  }

  float weight = readWeight();

  client.loop();

  float removeThreshold = totalWeight - (currentMedicineWeight * 0.7);

  Serial.print("W:");
  Serial.print(weight);
  Serial.print(" T:");
  Serial.print(totalWeight);
  Serial.print(" Thresh:");
  Serial.println(removeThreshold);

  // MEDICINE MISSED
  if (!removed &&
      millis() - reminderTime > limit &&
      weight >= removeThreshold)
  {
    Serial.println("Medicine Missed");
    tone(buzzer, 2000, 3000);
    turnOffLEDs();
    showMessage("Medicine Missed!");
    client.publish("medication/status",
      (currentMedicine + " Missed").c_str());
    reminderActive = false;
    return;
  }

  // MEDICINE REMOVED
  if (!removed && weight < removeThreshold)
  {
    removed            = true;
    removeTime         = millis();
    weightAfterRemoval = weight;

    noTone(buzzer);

    Serial.println("Medicine Removed");
    Serial.print("Weight after removal: ");
    Serial.println(weightAfterRemoval);

    showMessage("Medicine Taken");
    client.publish("medication/status",
      (currentMedicine + " Taken").c_str());
    return;
  }

  // MEDICINE RETURNED

  if (removed && weight >= removeThreshold)
  {
    removed = false;
    noTone(buzzer);

    Serial.println("Medicine Returned");
    Serial.print("Return weight: ");
    Serial.println(weight);

    float midPoint = (totalWeight + weightAfterRemoval) / 2.0;

    Serial.print("Midpoint: ");
    Serial.println(midPoint);

    if (weight >= midPoint)
    {
      Serial.println("Not Consumed");
      showMessage("Take Your Pill!");
      client.publish("medication/status",
        (currentMedicine + " Not Consumed").c_str());
    }
    else
    {
      Serial.println("Medicine Consumed");
      showMessage("Good Job!");
      client.publish("medication/status",
        (currentMedicine + " Consumed").c_str());

      // refill check
      if (weight < currentMedicineWeight * 0.2)
      {
        Serial.println("Refill Needed");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Low Supply!");
        lcd.setCursor(0, 1);
        lcd.print(currentMedicine);
        lcd.setCursor(0, 2);
        lcd.print("Please Refill");
        client.publish("medication/status",
          (currentMedicine + " Refill Needed").c_str());
        delay(3000);
      }
    }

    turnOffLEDs();
    reminderActive = false;
    return;
  }

  // NOT RETURNED IN TIME

 if (removed &&
    millis() - removeTime > limit &&
    !notReturnedSent)
 {
    notReturnedSent = true;

    Serial.println("Not Returned");

    tone(buzzer, 2000);

    turnOffLEDs();

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Return Medicine!");
    lcd.setCursor(0, 1);
    lcd.print(currentMedicine);

    client.publish("medication/status",
      (currentMedicine + " Not Returned").c_str());
 }
}

// SHOW MESSAGE

void showMessage(String text)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(text);
}

// WIFI CONNECT

void connectWiFi()
{
  Serial.println("Connecting WiFi...");

  while (WiFi.begin(ssid, pass) != WL_CONNECTED)
  {
    Serial.println("Trying WiFi...");
    delay(3000);
  }

  Serial.println("WiFi Connected");
}

// MQTT RECONNECT
void reconnect()
{
  while (!client.connected())
  {
    Serial.println("Connecting MQTT...");

    if (client.connect("ArduinoClient"))
    {
      Serial.println("MQTT Connected");
      client.subscribe("medication/setup");
      client.subscribe("medication/start");
      client.subscribe("medication/reminder");
    }
    else
    {
      Serial.println("MQTT Failed");
      delay(3000);
    }
  }
}

// TURN LEDs OFF

void turnOffLEDs()
{
  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
  digitalWrite(led3, LOW);
}