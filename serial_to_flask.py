# serial_to_flask.py
import serial
import threading
import json
import sqlite3
import re # Needed for regex
from flask import current_app
from models import db, SensorData

latest_data = {"message": "Waiting for sensor data"}
def save_to_db(ch4, co, nh3):
    conn = sqlite3.connect('sensor_data.db')
    cursor = conn.cursor()
    cursor.execute(
        "INSERT INTO sensor_logs (ch4, co, nh3) VALUES (?, ?, ?)",
        (ch4, co, nh3)
    )
    conn.commit()
    conn.close()

def read_serial_data():
    global latest_data
    try:
        ser = serial.Serial('COM3', 115200, timeout=3)
        print("✔ Serial connection opened on COM3.")
    except serial.SerialException as e:
        print("✗ Serial Port Error:", e)
        return

    while True:
        try:
            raw_bytes = ser.readline()
            try:
                line = raw_bytes.decode('utf-8').strip()
            except UnicodeDecodeError:
                print("▲ Skipped corrupted bytes.")
                continue

            if line:
                print("Raw:", line)
                try:
                    # Extract JSON portion using regex
                    match = re.search(r'\{.*\}', line)
                    if match:
                        json_str = match.group(0)
                        parsed = json.loads(json_str)
                        latest_data = parsed
                        print("Parsed:", latest_data)

                        # ✔ Extract values
                        ch4 = parsed.get("CH4", 0)
                        co = parsed.get("CO", 0)
                        nh3 = parsed.get("NH3", 0)

                        # ✔ Save to DB
                        save_to_db(ch4, co, nh3)
                    else:
                        print("▲ No valid JSON found in line.")

                except json.JSONDecodeError as err:
                    print("✗ JSON Parse Error:", err)
                except Exception as e:
                    print("✗ Read error:", e)

def start_serial_thread():
    thread = threading.Thread(target=read_serial_data, daemon=True)
    thread.start()