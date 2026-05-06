from gpiozero import LED
import tkinter as tk

# LEDs connected to GPIO pins
living_room = LED(17)
bathroom = LED(27)
closet = LED(22)

# this variable help me know what was selected before
last_selected = "none"

# this function runs when user clicks a radio button
def handle_selection():
    global last_selected

    current = selected_room.get()  # this part to get what user selected

    # if user clicks same option again → turn it OFF
    if current == last_selected:
        print(current, "turned OFF")

        if current == "living":
            living_room.off()
        elif current == "bathroom":
            bathroom.off()
        elif current == "closet":
            closet.off()

        selected_room.set("none")
        last_selected = "none"

    # if user selects a different room → turn it ON
    else:
        print(current, "turned ON")

        # turn ON selected light and turn OFF others
        if current == "living":
            living_room.on()
            bathroom.off()
            closet.off()

        elif current == "bathroom":
            living_room.off()
            bathroom.on()
            closet.off()

        elif current == "closet":
            living_room.off()
            bathroom.off()
            closet.on()

        last_selected = current

# this function closes the program
def exit_program():
    print("Exit Program")

    # turn off all lights before exit
    living_room.off()
    bathroom.off()
    closet.off()

    window.destroy()

# create GUI window
window = tk.Tk()
window.title("Home Light Control")
window.geometry("300x220")

# title text
tk.Label(window, text="Choose a Room", font=("Arial", 14)).pack(pady=10)

# variable for radio buttons
selected_room = tk.StringVar(value="none")

# radio buttons
tk.Radiobutton(window, text="Living Room",
  variable=selected_room,
 value="living",
 command=handle_selection).pack(pady=5)

tk.Radiobutton(window, text="Bathroom",
 variable=selected_room,
 value="bathroom",
   command=handle_selection).pack(pady=5)

tk.Radiobutton(window, text="Closet",
  variable=selected_room,
     value="closet",
     command=handle_selection).pack(pady=5)

# exit button
tk.Button(window, text="Exit", command=exit_program).pack(pady=15)

# run the program
window.mainloop()