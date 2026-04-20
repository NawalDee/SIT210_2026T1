Overview:

This project uses a Raspberry Pi with a GUI (Tkinter) to control lights in different rooms.

The system allows the user to:

Turn ON/OFF lights for different rooms

Adjust brightness of the living room using PWM

Features:

Living Room → Adjustable brightness (PWM LED)

Bathroom → ON/OFF LED

Closet → ON/OFF LED

Simple GUI with radio buttons and slider

How It Works?

The GUI lets the user select a room

Only one room can be active at a time

The slider controls brightness for the living room

PWM is used to change light intensity


Hardware Used:

Raspberry Pi

3 LEDs

Resistors

Breadboard & jumper wires


Software Used:

Python

gpiozero (for LED control)

tkinter (for GUI)


How to Run?

Connect the circuit to Raspberry Pi


Run the Python file:

python3 main.py

Use the GUI to control the lights
