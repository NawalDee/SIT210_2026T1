from gpiozero import PWMLED, LED
import tkinter as tk

//LEDs
living_room = PWMLED(18)
bathroom = LED(27)
closet = LED(22)

// track last selected
last_selected = None

// Functions

def turn_on_living():
    global last_selected
    print("Living room selected")

    if last_selected == "living":
        //OFF if clicked again
        living_room.off()
        selected_room.set("none")
        last_selected = None
    else:
        brightness = brightness_slider.get() / 100
        living_room.value = brightness
        bathroom.off()
        closet.off()
        last_selected = "living"

def turn_on_bathroom():
    global last_selected
    print("Bathroom selected")

    if last_selected == "bathroom":
        bathroom.off()
        selected_room.set("none")
        last_selected = None
    else:
        living_room.off()
        bathroom.on()
        closet.off()
        last_selected = "bathroom"

def turn_on_closet():
    global last_selected
    print("Closet selected")

    if last_selected == "closet":
        closet.off()
        selected_room.set("none")
        last_selected = None
    else:
        living_room.off()
        bathroom.off()
        closet.on()
        last_selected = "closet"

def change_brightness(value):
  //Only affect living room if it's ON
    if last_selected == "living":
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
window.geometry("300x250")

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

// Exit button
exit_button = tk.Button(window, text="Exit", command=exit_program)
exit_button.pack(pady=15)

// Run GUI
window.mainloop()