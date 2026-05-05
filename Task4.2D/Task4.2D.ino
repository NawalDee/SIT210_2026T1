#include <WiFiNINA.h>

// WiFi details
char ssid[] = "NetComm 3225";
char pass[] = "decefacuwa";

WiFiServer server(80);

// LED pins
int livingRoom = 2;
int bathroom = 3;
int closet = 4;

// this function takes room name as string and turns LED on/off
void controlLight(String room, String state) {

  int value;

  // check if we want ON or OFF
  if (state == "on") {
    value = HIGH;
  } else {
    value = LOW;
  }

  // check which room and control the correct LED
  if (room == "living room") {
    digitalWrite(livingRoom, value);
  }
  else if (room == "bathroom") {
    digitalWrite(bathroom, value);
  }
  else if (room == "closet") {
    digitalWrite(closet, value);
  }
}

void setup() {
  Serial.begin(9600);

  pinMode(livingRoom, OUTPUT);
  pinMode(bathroom, OUTPUT);
  pinMode(closet, OUTPUT);

  // connect to WiFi
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
  }

  Serial.println("Connected!");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {

  // simulate cloud/backend input using Serial
  if (Serial.available()) {
    String roomFromCloud = Serial.readStringUntil('\n');
    roomFromCloud.trim();

    controlLight(roomFromCloud, "on");

    Serial.println("Cloud command received: " + roomFromCloud);
  }

  WiFiClient client = server.available();

  if (client) {
    String request = client.readStringUntil('\r');
    client.flush();

    // call function using room name from web request
    if (request.indexOf("/control") != -1) {

      if (request.indexOf("room=living") != -1) {
        if (request.indexOf("state=on") != -1)
          controlLight("living room", "on");
        else
          controlLight("living room", "off");
      }

      if (request.indexOf("room=bathroom") != -1) {
        if (request.indexOf("state=on") != -1)
          controlLight("bathroom", "on");
        else
          controlLight("bathroom", "off");
      }

      if (request.indexOf("room=closet") != -1) {
        if (request.indexOf("state=on") != -1)
          controlLight("closet", "on");
        else
          controlLight("closet", "off");
      }
    }

    // simple HTML page
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println();

    client.println("<html><body>");
    client.println("<h1>LED Control</h1>");

    client.println("<h2>Living Room</h2>");
    client.println("<a href='/control?room=living&state=on'>ON</a><br>");
    client.println("<a href='/control?room=living&state=off'>OFF</a><br>");

    client.println("<h2>Bathroom</h2>");
    client.println("<a href='/control?room=bathroom&state=on'>ON</a><br>");
    client.println("<a href='/control?room=bathroom&state=off'>OFF</a><br>");

    client.println("<h2>Closet</h2>");
    client.println("<a href='/control?room=closet&state=on'>ON</a><br>");
    client.println("<a href='/control?room=closet&state=off'>OFF</a><br>");

    client.println("</body></html>");

    delay(1);
    client.stop();
  }
}
