import tkinter as tk
from tkinter import filedialog
import os
from serial import Serial
import time

# Define serial port settings
SERIAL_PORT = 'COM5'#'/dev/tty.usbmodem21101'  # replace with your serial port name
BAUDRATE = 9600
TIMEOUT = 1

# DEFAULT BOARD SIZE
BOARD_WIDTH = 2400 # mm
BOARD_HEIGHT = 1200 # mm
# is_valid_board_size = False
####            GUI         ####
class FileSelectionGUI(tk.Frame):
    def __init__(self, master):
        super().__init__(master)
        self.master = master
        self.frame = tk.Frame(self.master)

        self.master.title("Select File and Draw")
        w, h = root.winfo_screenwidth()-100, root.winfo_screenheight()-100
        self.master.geometry("{}x{}+0+0". format(w, h))
        # self.master.geometry("1000x600")      

        root.columnconfigure(0, weight=3)
        root.columnconfigure(1, weight=3)
        root.columnconfigure(2, weight=1)
        root.rowconfigure(0, weight=1)
        root.rowconfigure(1, weight=2)
        root.rowconfigure(2, weight=2)
        root.rowconfigure(3, weight=4)
        
        self.label = tk.Label(self.master, text="No file selected.")
        self.label.grid(row = 0, column = 1, sticky = 'w')

        self.preview_canvas = tk.Canvas(background="white")
        self.preview_canvas.grid(row = 1, column = 0, columnspan = 3, rowspan = 3, sticky = 'w'+'e'+'n'+'s')

        self.select_button = tk.Button(self.master, text="Select file", command=self.select_file)
        self.select_button.grid(row = 0, column = 0, padx=2, pady=2, sticky = 'w'+'e'+'n'+'s')

        self.input_dim_button = tk.Button(self.master, text = 'Input board dimensions', width = 25, command = self.open_board_size_window)
        self.input_dim_button.grid(row = 0, column = 2, padx=2, pady=2, sticky = 'w'+'e'+'n'+'s')

        self.dim_label = tk.Label(self.master, text="X: " + str(BOARD_WIDTH) + ", Y: " + str(BOARD_HEIGHT))
        self.dim_label.grid(row = 0, column = 3, sticky = 'e')

        self.clear_button = tk.Button(self.master, text="Clear", command=self.clear_preview)
        self.clear_button.grid(row = 2, column = 3, padx=2, pady=2, sticky = 'w'+'e'+'n'+'s')

        self.preview_button = tk.Button(self.master, text="Preview", command=self.preview_gcode_file)
        self.preview_button.grid(row = 1, column = 3, padx=2, pady=2, sticky = 'w'+'e'+'n'+'s')

        self.draw_button = tk.Button(self.master, text="Draw", command=self.draw_file)
        self.draw_button.grid(row = 3, column = 3, padx=2, pady=2, sticky = 'w'+'e'+'n'+'s')

        self.selected_file = None
        
    def open_board_size_window(self):
        self.newWindow = tk.Toplevel(self.master)
        self.app = BoardSizeGUI(self.newWindow)
    
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


class BoardSizeGUI(tk.Tk):
    def __init__(self, master):
        # super().__init__(master)
        self.master = master
        self.frame = tk.Frame(self.master)
        self.frame.pack()

        # input board size
        self.input_label = tk.Label(self.master, text="Input board height and width [mm]: ") #Default waarden ergens zetten in GUI?
        self.input_label.pack(side=tk.TOP, pady=10)

        self.input_button = tk.Button(self.master, text="Input", command=self.define_board_size)
        self.input_button.pack(side=tk.RIGHT, padx=10, pady=10)

        self.y_box = tk.Text(self.master, height = 5, width = 20)
        self.y_box.insert(1.0, str(BOARD_HEIGHT))
        self.y_box.pack(side=tk.RIGHT)

        self.x_box = tk.Text(self.master, height = 5, width = 20)
        self.x_box.insert(1.0, str(BOARD_WIDTH))
        self.x_box.pack(side=tk.RIGHT)

    def define_board_size(self):
        x_inp = self.x_box.get(1.0, "end-1c") #-1c to remove newline character
        y_inp = self.y_box.get(1.0, "end-1c")
        # self.input_label.config(text = "X: "+ x_inp + " mm; Y: " + y_inp + " mm")
        is_valid_board_size = True
        # set board sizes 
        BOARD_WIDTH = int(x_inp)
        BOARD_HEIGHT = int(y_inp)

        self.master.destroy()


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

    # Gcode to set the board size
    size_string = "A" + str(BOARD_WIDTH) + "," + str(BOARD_HEIGHT) + "\n"
    ser.write(size_string.encode())
    print(ser.readline().decode().strip())
    print(ser.readline().decode().strip())
    print(ser.readline().decode().strip())

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

    # Move back to default position
    # toevoegen M5
    # homing_string = ("G1 X"+str(BOARD_WIDTH/2)+" Y" + str(BOARD_HEIGHT) + ";\n")
    homing_string = ("G28;\n")
    ser.write(homing_string.encode())
    # Close the file and serial port
    file.close()




####        MAIN        ####
if __name__ == "__main__":

    # ser = open_serial() #uncomment to send to arduino
    root = tk.Tk()
    app = FileSelectionGUI(root)
    # root.attributes('-fullscreen', True)
    # app.state('zoomed')
    app.mainloop()
    