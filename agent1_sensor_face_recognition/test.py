import time
import keyboard
import bluetooth

# devices = bluetooth.discover_devices(lookup_names=True, duration=8)
# print(f"Devices found: {devices}")

socket = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
socket.connect(('98:D3:31:80:67:70', 1))

# arduino = serial.Serial("COM6", 9600)
time.sleep(2)

while True:
    if keyboard.is_pressed("space"):  # if the 'space' key is pressed
        print("Space key pressed")
        # arduino.write(bytes("1".encode()))
        socket.send("7")
        time.sleep(0.5)  # prevent bouncing effect