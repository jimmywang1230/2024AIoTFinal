import serial
import time
arduino = serial.Serial('COM6', 9600)
time.sleep(2)
arduino.write(b'111')