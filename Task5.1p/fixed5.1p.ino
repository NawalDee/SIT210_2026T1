from gpiozero import LED
import tkinter as tk

//LEDs
living_room = LED(17)
bathroom = LED(27)
closet = LED(22)

//  comment this section if u want to use checkbutton
//track last selected radio button
last_selected = None

// RADIO BUTTON FUNCTIONS


def toggle_living():
    global last_selected
    print("Living room selected")

    if last_selected == "living":
        living_room.off()
        selected_room.set("none")
        last_selected = None
    else:
        living_room.on()
        bathroom.off()
        closet.off()
        last_selected = "living"

def toggle_bathroom():
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

def toggle_closet():
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

def exit_program():
    print("Exit Program")
    living_room.off()
    bathroom.off()
    closet.off()
    window.destroy()


// GUI WINDOW


window = tk.Tk()
window.title("Home Light Control")
window.geometry("300x250")

title_label = tk.Label(window, text="Choose a Room", font=("Arial", 14))
title_label.pack(pady=10)

// Radio Button variable
selected_room = tk.StringVar()
selected_room.set("none")

// RADIO BUTTONS (ACTIVE)


living_radio = tk.Radiobutton(
    window,
    text="Living Room",
    variable=selected_room,
    value="living",
    command=toggle_living
)
living_radio.pack(pady=5)

bathroom_radio = tk.Radiobutton(
    window,
    text="Bathroom",
    variable=selected_room,
    value="bathroom",
    command=toggle_bathroom
)
bathroom_radio.pack(pady=5)

closet_radio = tk.Radiobutton(
    window,
    text="Closet",
    variable=selected_room,
    value="closet",
    command=toggle_closet
)
closet_radio.pack(pady=5)

//CHECK BUTTONS (COMMENTED)


/* 
# Uncomment this section if u want multiple lights at the same time

living_var = tk.IntVar()
bathroom_var = tk.IntVar()
closet_var = tk.IntVar()

def toggle_living_check():
    if living_var.get():
        living_room.on()
    else:
        living_room.off()

def toggle_bathroom_check():
    if bathroom_var.get():
        bathroom.on()
    else:
        bathroom.off()

def toggle_closet_check():
    if closet_var.get():
        closet.on()
    else:
        closet.off()

living_check = tk.Checkbutton(window, text="Living Room", variable=living_var, command=toggle_living_check)
living_check.pack()

bathroom_check = tk.Checkbutton(window, text="Bathroom", variable=bathroom_var, command=toggle_bathroom_check)
bathroom_check.pack()

closet_check = tk.Checkbutton(window, text="Closet", variable=closet_var, command=toggle_closet_check)
closet_check.pack()
*/

//Exit button
exit_button = tk.Button(window, text="Exit", command=exit_program)
exit_button.pack(pady=15)

window.mainloop()