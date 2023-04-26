import tkinter as tk
from tkinter import filedialog
import os
from serial import Serial
import time

# Define serial port settings
SERIAL_PORT = 'COM5'#'/dev/tty.usbmodem21101'  # replace with your serial port name
BAUDRATE = 9600
TIMEOUT = 1

# BOARD SIZE
BOARD_WIDTH = 2400 # mm
BOARD_HEIGHT = 1200 # mm
####            GUI         ####
class FileSelectionGUI(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("Select File and Draw")
        self.geometry("1000x600")

        self.label = tk.Label(self, text="No file selected.")
        self.label.pack(pady=10)

        self.preview_canvas = tk.Canvas(background="white")
        self.preview_canvas.pack(fill="both", expand = True)
        # self.preview_canvas.pack(padx=10)
        # self.preview_canvas.bind('<Configure>', self.resize_canvas)

        self.select_button = tk.Button(self, text="Select file", command=self.select_file)
        self.select_button.pack(side=tk.LEFT, padx=10)

        self.clear_button = tk.Button(self, text="Clear", command=self.clear_preview)
        self.clear_button.pack(side=tk.BOTTOM)#, padx=10)

        self.preview_button = tk.Button(self, text="Preview", command=self.preview_gcode_file)
        self.preview_button.pack(side=tk.BOTTOM)#, padx=10)

        self.draw_button = tk.Button(self, text="Draw", command=self.draw_file)
        self.draw_button.pack(side=tk.RIGHT, padx=10)

        self.selected_file = None

    def select_file(self):
        file_path = filedialog.askopenfilename()
        if file_path:
            self.selected_file = file_path
            file_name = os.path.basename(file_path)
            self.label.config(text=f"Selected file: {file_name}")
    

    def preview_gcode_file(self):
        x_coords = []
        y_coords = []

        draw_flag = False
        draw_list = []
        
        with open(self.selected_file, 'r') as f:
            for line in f:
                line = line.replace(";\n","")
                print(line)
                if line.startswith('G1'):
                    x_coord = float(line.split('X')[-1].split(' ')[0])
                    y_coord = float(line.split('Y')[-1].split(' ')[0])
                    
                    x_coords.append(x_coord)
                    y_coords.append(y_coord)
                    draw_list.append(draw_flag)

                    if (x_coord > BOARD_WIDTH or x_coord < 0 or y_coord > BOARD_HEIGHT or y_coord < 0):
                        self.label.config(text="ATTENTION: SOME MOVES ARE OUT OF BOUND!")

                elif line == "M3 S255":
                    draw_flag = True
                    print("drawing ON")
                elif line == "M5":
                    draw_flag = False
                    print("drawing OFF")

        
        scale_x = self.preview_canvas.winfo_width()/BOARD_WIDTH
        scale_y = self.preview_canvas.winfo_height()/BOARD_HEIGHT
        prev_x = x_coords[0] * scale_x
        prev_y = y_coords[0] * scale_y
        
        for i in range(1, len(x_coords)):
            curr_x = x_coords[i] * scale_x
            curr_y = y_coords[i] * scale_y
            
            if draw_list[i] == True:
                self.preview_canvas.create_line(prev_x, prev_y, curr_x, curr_y, fill = 'red')
            
            prev_x = curr_x
            prev_y = curr_y
        
    def clear_preview(self):
        self.preview_canvas.delete("all")

    def resize_canvas(self, event):
        self.update()
        w,h = event.width-100, event.height-100
        aspect_ratio = BOARD_WIDTH/BOARD_HEIGHT

        if w/h > aspect_ratio:
            self.preview_canvas.config(width=aspect_ratio*h, height=h)
        else:
            self.preview_canvas.config(width=w, height=w/aspect_ratio)
    

        
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

    # ser = open_serial() #uncomment to send to arduino
    app = FileSelectionGUI()
    # app.attributes('-fullscreen', True)
    app.state('zoomed')
    app.mainloop()
    