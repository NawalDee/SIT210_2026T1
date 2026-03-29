Overview:
This project uses Arduino Nano 33 IoT and a BH1750 sensor to detect light level and send notifications using IFTTT.

How it works ?
Reads light in lux
If lux < 50 → sends Dark notification
If lux ≥ 50 → sends Light notification

Tools:
Arduino, BH1750 sensor, WiFiNINA, IFTTT

Testing:
Tested by covering and exposing the sensor to check notifications.
