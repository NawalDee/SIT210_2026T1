import paho.mqtt.client as mqtt
import requests
import time
import threading
import json
import os

import tkinter as tk
from tkinter import ttk
from tkinter import messagebox

from datetime import datetime


# GLOBALS & LOCKS

patientName = ""
selectedPatientName = ""
medicines = []
systemStarted = False
statusBox = None
root = None

data_lock = threading.Lock()


# MQTT CONFIGURATION & RECONNECT LOGIC

broker = "broker.emqx.io"
client = mqtt.Client()


def on_disconnect(client, userdata, rc):
    if rc != 0:
        print("Unexpected MQTT disconnection. Will auto-reconnect...")

client.on_disconnect = on_disconnect
client.connect(broker, 1883, keepalive=60)


# IFTTT


IFTTT_KEY = "c-6vA8zDMuA6YS6SFWDWS"



# JSON DATABASE

def loadData():
    if not os.path.exists("patients.json"):
        return {"patients": []}
    try:
        with open("patients.json", "r") as f:
            return json.load(f)
    except Exception as e:
        print(f"Error loading JSON: {e}")
        return {"patients": []}


def saveData(data):
    try:
        with open("patients.json", "w") as f:
            json.dump(data, f, indent=4)
    except Exception as e:
        print(f"Error saving JSON: {e}")

# RECEIVE FROM ARDUINO

def safe_update_status(message):
    global statusBox
    if statusBox is not None:
        try:
            statusBox.insert(tk.END, message + "\n")
            statusBox.see(tk.END)
        except Exception:
            pass


def on_message(mqttClient, userdata, msg):
    topic = msg.topic
    message = msg.payload.decode()

    print(f"\n[{topic}]: {message}")

    if topic == "medication/status":

        if root:
            root.after(0, lambda: safe_update_status(message))

        # OUTBOUND EXTERNAL NOTIFICATIONS (IFTTT API)

        def sendNotifications():
            try:
                requests.get(
                    "https://maker.ifttt.com/trigger/medicine_alert/with/key/" + IFTTT_KEY,
                    params={"value1": message},
                    timeout=5
                )
            except requests.exceptions.RequestException as e:
                print(f"IFTTT alert network failed: {e}")

            if "Refill Needed" in message:
                try:
                    requests.get(
                        "https://maker.ifttt.com/trigger/medicine_refill/with/key/" + IFTTT_KEY,
                        params={
                            "value1": message,
                            "value2": patientName,
                            "value3": "Please refill as soon as possible"
                        },
                        timeout=5
                    )
                except requests.exceptions.RequestException as e:
                    print(f"IFTTT refill network failed: {e}")


        threading.Thread(target=sendNotifications, daemon=True).start()

        if "Not Returned" in message:
            print("*** NOT RETURNED ***:", message)

        if "Missed" in message:
            print("*** MISSED ***:", message)


client.on_message = on_message
client.subscribe("medication/status")
client.loop_start()

# CHECK REMINDER TIMES (THREAD-SAFE & OPTIMIZED)

def checkReminderTimes():
    if not systemStarted:
        return

    now = datetime.now()
    currentHour = now.strftime("%H")
    currentMinute = now.strftime("%M")

    with data_lock:
        for med in medicines:
            startHour, startMinute = med["start_time"].split(":")

            if currentMinute == startMinute and not med["sent"]:
                currentHourInt = int(currentHour)
                startHourInt = int(startHour)
                hoursPassed = (currentHourInt - startHourInt) % 24

                if hoursPassed % med["frequency"] == 0:

                    threading.Thread(
                        target=lambda m=med: client.publish("medication/reminder", m["name"]),
                        daemon=True
                    ).start()

                    print(f"\nReminder Sent: {med['name']} at {currentHour}:{currentMinute}")
                    med["sent"] = True
                    med["sentHour"] = currentHour

            if med["sentHour"] != "" and med["sentHour"] != currentHour:
                med["sent"] = False
                med["sentHour"] = ""


#  BACKGROUND TIME MONITORING THREAD

def monitorTimes():
    last_minute = ""
    while True:
        current_minute = datetime.now().strftime("%M")
        if current_minute != last_minute:
            checkReminderTimes()
            last_minute = current_minute
        time.sleep(1)


threading.Thread(target=monitorTimes, daemon=True).start()

# GUI
def openGUI():
    global statusBox, systemStarted, root
    global selectedPatientName, patientName, medicines

    root = tk.Tk()
    root.title("Medication Monitoring System")
    root.geometry("650x820")

    # ADD PATIENT

    tk.Label(root, text="Patient Name", font=("Arial", 11)).pack(pady=(15, 2))
    patientEntry = tk.Entry(root, width=35)
    patientEntry.pack()

    def guiAddPatient():
        name = patientEntry.get().strip()
        if name == "":
            messagebox.showerror("Error", "Enter patient name")
            return

        data = loadData()
        for p in data["patients"]:
            if p["name"] == name:
                messagebox.showerror("Error", "Patient already exists")
                return

        data["patients"].append({
            "name": name,
            "age": "",
            "id": "",
            "medicines": []
        })
        saveData(data)
        messagebox.showinfo("Success", "Patient saved")
        patientEntry.delete(0, tk.END)
        refreshPatients()

    tk.Button(root, text="Add Patient", command=guiAddPatient, width=20).pack(pady=8)

    # SELECT PATIENT

    tk.Label(root, text="Select Patient", font=("Arial", 11)).pack(pady=(10, 2))
    patientDropdown = ttk.Combobox(root, width=33)
    patientDropdown.pack()

    def refreshPatients():
        data = loadData()
        names = [p["name"] for p in data["patients"]]
        patientDropdown["values"] = names

    def loadPatient():
        global selectedPatientName, patientName, medicines

        selectedPatientName = patientDropdown.get()
        patientName = selectedPatientName

        if selectedPatientName == "":
            return

        medicineList.delete(0, tk.END)

        with data_lock:
            medicines.clear()
            data = loadData()
            for patient in data["patients"]:
                if patient["name"] == selectedPatientName:
                    for med in patient["medicines"]:
                        if "sent" not in med: med["sent"] = False
                        if "sentHour" not in med: med["sentHour"] = ""
                        medicines.append(med)
                        medicineList.insert(
                            tk.END,
                            f'{med["name"]} | {med["start_time"]} | Every {med["frequency"]}h'
                        )
                    break

        print("Patient loaded:", selectedPatientName)
        print("Medicines:", len(medicines))

    tk.Button(root, text="Load Patient", command=loadPatient, width=20).pack(pady=8)

    # ADD MEDICINE

    tk.Label(root, text="Medicine Name", font=("Arial", 11)).pack(pady=(10, 2))
    medEntry = tk.Entry(root, width=35)
    medEntry.pack()

    tk.Label(root, text="Start Time (e.g. 08:00)").pack(pady=(5, 2))
    timeEntry = tk.Entry(root, width=35)
    timeEntry.pack()

    tk.Label(root, text="Frequency (hours, e.g. 8)").pack(pady=(5, 2))
    freqEntry = tk.Entry(root, width=35)
    freqEntry.pack()

    def guiAddMedicine():
        global medicines

        if selectedPatientName == "":
            messagebox.showerror("Error", "Load a patient first")
            return

        medName = medEntry.get().strip()
        startTime = timeEntry.get().strip()
        frequency = freqEntry.get().strip()

        if medName == "" or startTime == "" or frequency == "":
            messagebox.showerror("Error", "Fill all fields")
            return

        try:
            freqInt = int(frequency)
        except ValueError:
            messagebox.showerror("Error", "Frequency must be a number")
            return

        threading.Thread(
            target=lambda: client.publish("medication/setup", medName),
            daemon=True
        ).start()
        print("Sent setup to Arduino:", medName)

        statusBox.insert(tk.END, f"Sent to Arduino: '{medName}'\n")
        statusBox.see(tk.END)

        messagebox.showinfo(
            "Step 1 - Place Medicine",
            f"Place ONLY '{medName}' on the scale.\n\nPress OK then wait for the countdown."
        )

        countdown_label = tk.Label(root, text="", font=("Arial", 12), fg="blue")
        countdown_label.pack()

        def runCountdown(seconds_left):
            if seconds_left > 0:

                countdown_label.config(text=f"Reading scale... {seconds_left}s")
                root.after(1000, lambda: runCountdown(seconds_left - 1))
            else:
                countdown_label.config(text="")
                countdown_label.destroy()
                statusBox.insert(tk.END, "Scale reading done\n")
                statusBox.see(tk.END)
                step4_remove()

        def step4_remove():
            messagebox.showinfo(
                "Step 2 - Remove Medicine",
                f"Remove '{medName}' from the scale.\n\nPress OK after removing."
            )
            root.after(2000, step5_save)

        def step5_save():
            newMed = {
                "name": medName,
                "start_time": startTime,
                "frequency": freqInt,
                "sent": False,
                "sentHour": ""
            }

            data = loadData()
            for patient in data["patients"]:
                if patient["name"] == selectedPatientName:
                    patient["medicines"].append(newMed)
                    break
            saveData(data)

            with data_lock:
                medicines.append(newMed)

            medicineList.insert(tk.END, f'{medName} | {startTime} | Every {freqInt}h')

            medEntry.delete(0, tk.END)
            timeEntry.delete(0, tk.END)
            freqEntry.delete(0, tk.END)

            statusBox.insert(tk.END, f"'{medName}' saved successfully\n")
            statusBox.see(tk.END)

            messagebox.showinfo("Saved", f"'{medName}' saved successfully")

        runCountdown(12)

    tk.Button(root, text="Add Medicine", command=guiAddMedicine, width=20).pack(pady=10)

    # MEDICINE LIST

    tk.Label(root, text="Saved Medicines", font=("Arial", 11)).pack(pady=(5, 2))
    medicineList = tk.Listbox(root, width=60, height=6)
    medicineList.pack(pady=5)

    def deleteSelectedMedicine():
        global medicines

        if selectedPatientName == "":
            messagebox.showerror("Error", "Load a patient first")
            return

        selected = medicineList.curselection()
        if not selected:
            messagebox.showerror("Error", "Select a medicine to delete")
            return

        index = selected[0]

        data = loadData()
        for patient in data["patients"]:
            if patient["name"] == selectedPatientName:
                patient["medicines"].pop(index)
                break
        saveData(data)

        with data_lock:
            medicines.pop(index)

        loadPatient()
        messagebox.showinfo("Deleted", "Medicine deleted")

    tk.Button(root, text="Delete Selected Medicine", command=deleteSelectedMedicine, width=25).pack(pady=5)


    # START MONITORING


    def startMonitoring():
        global systemStarted

        if selectedPatientName == "":
            messagebox.showerror("Error", "Load a patient first")
            return

        with data_lock:
            med_count = len(medicines)

        if med_count == 0:
            messagebox.showerror("Error", "Add medicines first")
            return

        messagebox.showinfo(
            "Final Step",
            "Place ALL medicines on the scale.\n\nPress OK when ready."
        )

        threading.Thread(
            target=lambda: client.publish("medication/start", "START"),
            daemon=True
        ).start()
        print("Sent: START")

        systemStarted = True

        statusBox.insert(tk.END, "Monitoring Started\n")
        statusBox.see(tk.END)

        messagebox.showinfo("Monitoring", "System is now monitoring")

    tk.Button(
        root,
        text="Start Monitoring",
        command=startMonitoring,
        width=20,
        bg="green",
        fg="white"
    ).pack(pady=10)


    # STATUS BOX

    tk.Label(root, text="System Status", font=("Arial", 11)).pack(pady=(5, 2))
    statusBox = tk.Text(root, height=8, width=60)
    statusBox.pack(pady=5)

    tk.Button(root, text="Exit", command=root.destroy, width=15).pack(pady=10)

    refreshPatients()
    root.mainloop()



# START APPLICATION


openGUI()