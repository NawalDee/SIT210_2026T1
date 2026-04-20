from gpiozero import LED
import tkinter as tk

//LEDs

living_room = LED(17)
bathroom = LED(27)
closet = LED(22)

//Functions

def turn_on_living():
    print("Living room selected")
    living_room.on()
    bathroom.off()
    closet.off()


def turn_on_bathroom():
    print("Bathroom  selected")
    living_room.off()
    bathroom.on()
    closet.off()

def turn_on_closet():
    print("Closet  selected")
    living_room.off()
    bathroom.off()
    closet.on()

def exit_program():
    print("Exit Program")
    living_room.off()
    bathroom.off()
    closet.off()
    window.destroy()

// GUI window
window = tk.Tk()
window.title("Home Light Control")
window.geometry("300x220")

title_label = tk.Label(window, text="Choose a Room", font=("Arial, 14"))
title_label.pack(pady=10)

//Radio Button varibale
selected_room = tk.StringVar()
selected_room.set("none")

//radio buttons
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

//Exit button
exit_button = tk.Button(window, text="Exit", command=exit_program)
exit_button.pack(pady=15)

window.mainloop()
