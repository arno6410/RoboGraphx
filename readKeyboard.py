import serial
import sys
import keyboard  # You may need to install this package via pip

# Open serial communication with Arduino
ser = serial.Serial('COM3', 9600)  # Replace 'COM3' with the serial port of your Arduino

# Loop to read keyboard input and send it to Arduino
while True:
    try:
        # Read keyboard input
        key = keyboard.read_event().name

        # Send the character to Arduino
        ser.write(key.encode())
    except KeyboardInterrupt:
        # Close the serial communication and exit the program
        ser.close()
        sys.exit()
