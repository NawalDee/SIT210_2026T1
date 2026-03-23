Task 1.1P - Switching ON Lights

Description:
This project is designed for a smart elderly care home system. When the push button is pressed, the porch light turn on for 30 seconds, and the hallway light turn on for 60 seconds to help the user safely enter the house.

Components Used:
- Arduino Nano 33 IoT
- 2 LEDs (Porch and Hallway lights)
- Push Button
- Resistors
- Breadboard and jumper wires

Code Explanation:
The program uses modular programming by creating a separate function called turnOnLights().

- The setup() function initializ the pins.
- The loop() function chicks if the button is pressed.
- When the button is pressed, the turnOnLights() function is called.
- The function turns on both lights, then turns off the porch light after 30 seconds and the hallway light after 60 seconds.

How it Works?
1. User presses the button.
2. Both lights turn ON.
3. Porch light turns OFF after 30 seconds.
4. Hallway light turns OFF after 60 seconds.
