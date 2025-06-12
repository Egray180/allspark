import asyncio
import json
import threading
import tkinter as tk
import websockets

# --- Shared State ---
latest_values = {}
calc_outputs = [0.0, 0.0, 0.0, 0.0]

# --- Calculation Logic ---
def run_calculation():
    A = 0
    B = 0
    C = 0
    D = 0
    for i in range(1, 5):
        if latest_values.get(i) is None:
            continue
        A += latest_values[i]["A"]
        B += latest_values[i]["B"]
        C += latest_values[i]["C"]
        D += latest_values[i]["D"]
    # assuming even force distribution between corners
    calc_outputs[0] = D - C # normal force
    calc_outputs[1] = 2 * min(A, B) # shear force
    calc_outputs[2] = B - A # z torque force
    calc_outputs[3] = 2 * min(D, C) # xy torque force

# --- WebSocket Handler ---
async def handler(websocket):
    async for message in websocket:
        try:
            data = json.loads(message)
            esp_id = data.get("esp_id")
            A = data.get("A")
            B = data.get("B")
            C = data.get("C")
            D = data.get("D")
            if all(i is not None for i in [esp_id, A, B, C, D]):
                latest_values[esp_id] = {"A": A, "B": B, "C": C, "D": D}
                run_calculation()
        except json.JSONDecodeError:
            print("Invalid JSON received")

async def websocket_server():
    async with websockets.serve(handler, "0.0.0.0", 8765):
        print("WebSocket server running on port 8765")
        await asyncio.Future()  # run forever

# --- Tkinter GUI ---
def start_gui():
    root = tk.Tk()
    root.title("ESP32 Sensor Dashboard")

    labels = []
    tags = ["Normal Force", "Shear Force", "Z Torque Force", "XY Torque Force"]
    for i in range(4):
        frame = tk.Frame(root)
        frame.pack()
        tk.Label(frame, text=f"{tags[i]}:", font=("Helvetica", 16)).pack(side=tk.LEFT)
        val_label = tk.Label(frame, text="0.0", font=("Helvetica", 16), fg="blue")
        val_label.pack(side=tk.LEFT)
        labels.append(val_label)

    def update_display():
        for i in range(4):
            labels[i].config(text=f"{calc_outputs[i]:.2f}")
        root.after(500, update_display)

    root.after(500, update_display)
    root.mainloop()

# --- Thread to run asyncio loop ---
def start_async_loop():
    asyncio.run(websocket_server())

# --- Main ---
if __name__ == "__main__":
    threading.Thread(target=start_async_loop, daemon=True).start()
    start_gui()
