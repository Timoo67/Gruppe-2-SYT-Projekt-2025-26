import paho.mqtt.client as mqtt
import tkinter as tk
from tkinter import scrolledtext
import json

# ---------------- GUI SETUP ---------------- #
root = tk.Tk()
root.title("MQTT Temperatur Monitor")
root.geometry("700x400")

output_box = scrolledtext.ScrolledText(root, width=45, height=15, state="disabled")
output_box.pack(pady=10)

def add_text(message):
    output_box.config(state="normal")
    output_box.insert(tk.END, message + "\n")
    output_box.see(tk.END)
    output_box.config(state="disabled")

def on_connect(client, userdata, flags, reasonCode, properties):
    add_text("✅ Verbunden mit MQTT Broker (rc=" + str(reasonCode) + ")")
    client.subscribe("Gruppe2/daten")


def on_message(client, userdata, msg):
    try:
        data = json.loads(msg.payload.decode())
        display_text = (
            f"Bodenfeuchte: {data['soil']} %\n"
            f"Temperatur: {data['temp']} °C\n"
            f"Luftfeuchtigkeit: {data['humidity']} %\n"
            f"Pumpe: {data['pump']}\n"
            f"Tank: {data['tank']}\n"
            "----------------------------\n\n"
        )

        # Nachricht ins UI einfügen
        output_box.config(state="normal")
        output_box.insert(tk.END, display_text)
        output_box.see(tk.END)
        output_box.config(state="disabled")
    except Exception as e:
        print("Fehler beim Parsen des JSON:", e)

client = mqtt.Client(callback_api_version=mqtt.CallbackAPIVersion.VERSION2)
client.on_connect = on_connect
client.on_message = on_message

client.connect("172.16.93.132", 1883, 60)
client.loop_start()

# ---------------- GUI LOOP ---------------- #
root.mainloop()

# Clean disconnect when UI closes
client.loop_stop()
client.disconnect()
