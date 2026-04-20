
Overview

This project uses hardware interrupts on the Arduino Nano 33 IoT to control LEDs based on motion detection and a manual switch. The system turns ON lights automatically when motion is detected in the dark, or when the switch is pressed.

#Features:

Motion detection using PIR sensor

Manual control using slider switch

Light condition check using BH1750 sensor

LEDs turn ON only when it is dark

Fast response using interrupts (no continuous polling)


How It Works:

PIR sensor triggers an interrupt when motion is detected

Switch triggers an interrupt when pressed

BH1750 checks light intensity

If it is dark, LEDs turn ON

Serial Monitor displays system status


Hardware Used:

Arduino Nano 33 IoT

PIR Motion Sensor

BH1750 Light Sensor

Slider Switch

2 LEDs

Resistors

Breadboard & jumper wires


Software Used:

Arduino IDE

BH1750 library

Wire (I2C communication)


How to Run?


Connect the circuit as per diagram

Upload the code to Arduino

Open Serial Monitor

Trigger motion or press switch to test
