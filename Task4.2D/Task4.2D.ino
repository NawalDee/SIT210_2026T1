#include <WiFiNINA.h>

// WiFi details
char ssid[] = "NetComm 3225";
char pass[] = "decefacuwa";

WiFiServer server(80);

// LED pins
int livingRoom = 2;
int bathroom = 3;
int closet = 4;

void setup() {
  Serial.begin(9600);

  pinMode(livingRoom, OUTPUT);
  pinMode(bathroom, OUTPUT);
  pinMode(closet, OUTPUT);

  // Connect to WiFi
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
  WiFiClient client = server.available();

  if (client) {
    String request = client.readStringUntil('\r');
    client.flush();

    // Control LEDs
    if (request.indexOf("/living/on") != -1) {
      digitalWrite(livingRoom, HIGH);
    }
    if (request.indexOf("/living/off") != -1) {
      digitalWrite(livingRoom, LOW);
    }

    if (request.indexOf("/bathroom/on") != -1) {
      digitalWrite(bathroom, HIGH);
    }
    if (request.indexOf("/bathroom/off") != -1) {
      digitalWrite(bathroom, LOW);
    }

    if (request.indexOf("/closet/on") != -1) {
      digitalWrite(closet, HIGH);
    }
    if (request.indexOf("/closet/off") != -1) {
      digitalWrite(closet, LOW);
    }

    // HTML Page
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println();

    client.println("<html><body>");
    client.println("<h1>LED Control</h1>");

    client.println("<h2>Living Room</h2>");
    client.println("<a href='/living/on'>ON</a><br>");
    client.println("<a href='/living/off'>OFF</a><br>");

    client.println("<h2>Bathroom</h2>");
    client.println("<a href='/bathroom/on'>ON</a><br>");
    client.println("<a href='/bathroom/off'>OFF</a><br>");

    client.println("<h2>Closet</h2>");
    client.println("<a href='/closet/on'>ON</a><br>");
    client.println("<a href='/closet/off'>OFF</a><br>");

    client.println("</body></html>");

    delay(1);
    client.stop();
  }
}