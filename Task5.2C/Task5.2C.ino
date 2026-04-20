from gpiozero import PWMLED, LED
import tkinter as tk

//LEDs
living_room = PWMLED(18)   # supports brightness
bathroom = LED(27)
closet = LED(22)

//Functions

def turn_on_living():
    print("Living room selected")
    brightness = brightness_slider.get() / 100
    living_room.value = brightness
    bathroom.off()
    closet.off()

def turn_on_bathroom():
    print("Bathroom selected")
    living_room.off()
    bathroom.on()
    closet.off()

def turn_on_closet():
    print("Closet selected")
    living_room.off()
    bathroom.off()
    closet.on()

def change_brightness(value):
    //Only affect living room
    if selected_room.get() == "living":
        brightness = float(value) / 100
        living_room.value = brightness
        print("Brightness:", brightness)

def exit_program():
    print("Exit Program")
    living_room.off()
    bathroom.off()
    closet.off()
    window.destroy()

//GUI window
window = tk.Tk()
window.title("Home Light Control")
window.geometry("300x220")

title_label = tk.Label(window, text="Choose a Room", font=("Arial", 14))
title_label.pack(pady=10)

//Radio button variable
selected_room = tk.StringVar()
selected_room.set("none")

//Radio buttons
living_radio = tk.Radiobutton(
    window,
    text="Living Room",
    variable=selected_room,
    value="living",
    command=turn_on_living
)
living_radio.pack(pady=5)

bathroom_radio = tk.Radiobutton(
    window,
    text="Bathroom",
    variable=selected_room,
    value="bathroom",
    command=turn_on_bathroom
)
bathroom_radio.pack(pady=5)

closet_radio = tk.Radiobutton(
    window,
    text="Closet",
    variable=selected_room,
    value="closet",
    command=turn_on_closet
)
closet_radio.pack(pady=5)

//Brightness slider (Living Room only)
slider_label = tk.Label(window, text="Living Room Brightness")
slider_label.pack(pady=5)

brightness_slider = tk.Scale(
    window,
    from_=0,
    to=100,
    orient=tk.HORIZONTAL,
    command=change_brightness
)
brightness_slider.pack(pady=5)
brightness_slider.set(50)

//Exit button
exit_button = tk.Button(window, text="Exit", command=exit_program)
exit_button.pack(pady=15)

//Run GUI
window.mainloop()