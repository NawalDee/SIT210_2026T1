In this task, I built a system to control lights remotely using Arduino and a web page. The Arduino Nano 33 IoT connects to WiFi and acts as a web server. When the user clicks buttons on the webpage, the LEDs turn ON or OFF.

Objective:
Connect Arduino to WiFi
Create a web server
Control LEDs from a browser

Hardware:

Arduino Nano 33 IoT
Breadboard
3 LEDs + resistors

Setup:
Living Room → Pin 2
Bathroom → Pin 3
Closet → Pin 4

Each LED is connected:

Pin → resistor → LED → GND

How it works?
Arduino connects to WiFi and gives an IP address
User opens the IP in a browser
User clicks ON/OFF buttons
Arduino reads the request
LED turns ON or OFF
