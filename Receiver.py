import serial
import json
import requests

SERIAL_PORT = 'COM3' # Replace with your port
BAUD_RATE = 9600

def send_to_flask(data):
    try:
        response = requests.post("http://localhost:5000/upload", json=data)
        print("Server response:", response.json())
    except Exception as e:
        print("Error sending to server:", e)

def read_serial():
    try:
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=2)
        print(f"Connected to {SERIAL_PORT}")
        while True:
            raw = ser.readline().decode('utf-8', errors='ignore').strip()
            if raw.startswith("{") and raw.endswith("}"):
                try:
                    data = json.loads(raw)
                    print("Received:", data)
                    send_to_flask(data)
                except json.JSONDecodeError:
                    print("Invalid JSON:", raw)
                except Exception as e:
                    print("Error:", e)

if __name__ == '__main__':
    read_serial()