import tkinter as tk
from tkinter import filedialog
import os
from serial import Serial
import time

# Define serial port settings
SERIAL_PORT = 'COM5'#''/dev/tty.usbmodem21101''  # replace with your serial port name
BAUDRATE = 9600
TIMEOUT = 1

####            GUI         ####
class FileSelectionGUI(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("Select File and Draw")
        self.geometry("400x150")

        self.label = tk.Label(self, text="No file selected.")
        self.label.pack(pady=10)

        self.select_button = tk.Button(self, text="Select file", command=self.select_file)
        self.select_button.pack(side=tk.LEFT, padx=10)

        self.draw_button = tk.Button(self, text="Draw", command=self.draw_file)
        self.draw_button.pack(side=tk.RIGHT, padx=10)

        self.selected_file = None

    def select_file(self):
        file_path = filedialog.askopenfilename()
        if file_path:
            self.selected_file = file_path
            file_name = os.path.basename(file_path)
            self.label.config(text=f"Selected file: {file_name}")

    def draw_file(self):
        if self.selected_file:
            # Call the function that draws the file with the selected file path as an argument
            draw_function(self.selected_file, ser)
        else:
            self.label.config(text="No file selected.")


####            START SERIAL CONNECTION             ####
def open_serial():
    # Initialize the serial port
    print('test')
    ser = Serial(SERIAL_PORT, BAUDRATE, timeout=TIMEOUT)
    # Wait for the serial connection to be established
    time.sleep(2)

    return ser


####            SEND FILE TO ARDUINO TO DRAW            ####
def draw_function(file_path, ser):
    # Replace this function with the actual function that draws the file
    print(f"Drawing file: {file_path}")

    file = open(file_path, 'r')

    start = 0 # debug

    # Loop through the file and send each line to the serial port
    for line in file:
        # Remove any newlines or carriage returns from the line
        line = line.strip()

        # Send the line to the serial port
        ser.write(line.encode())

        stop = time.time() #debug

        elapsed_time = stop - start #debug
        print('Execution time:', elapsed_time, 'seconds') #debug

        # Wait for an "ok" response from the serial port
        while True:
            response = ser.readline().decode().strip()
            if response == "ok":
                start = time.time()
                break
            elif response:
                print(response)

    # Close the file and serial port
    file.close()




####        MAIN        ####
if __name__ == "__main__":

    ser = open_serial()
    app = FileSelectionGUI()
    app.mainloop()
    