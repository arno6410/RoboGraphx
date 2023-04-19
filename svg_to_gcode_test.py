# msg = "Hello World"
# print(msg)
#import numpy
# import svgpathtools
# from svgpathtools import svg2paths

# https://github.com/PadLex/SvgToGcode
from svg_to_gcode.svg_parser import parse_file
from svg_to_gcode.compiler import Compiler, interfaces
import serial
import time
from svg_to_gcode import TOLERANCES

TOLERANCES['approximation'] = 1

# Instantiate a compiler, specifying the interface type and the speed at which the tool should move. pass_depth controls
# how far down the tool moves after every pass. Set it to 0 if your machine does not support Z axis movement.
gcode_compiler = Compiler(interfaces.Gcode, movement_speed=1000, cutting_speed=300, pass_depth=5)

curves = parse_file("gcode/test.svg") # Parse an svg file into geometric curves

gcode_compiler.append_curves(curves) 


# gcode_compiler.compile_to_file("gcode/drawing2.gcode", passes=1)
# # Load the SVG file and extract the path data
# paths, attributes = svg2paths('gcode/Circle.svg')

string = 'done'
# # Convert the path data to a series of line segments or curves
# for path in paths:
#     segments = path_to_segments(path)

#     # Convert the line segments or curves to G-code
#     gcode = segments_to_gcode(segments)
    
#     # Send the G-code to the CNC machine to execute the toolpath
#     send_to_cnc(gcode)

# Send G-code to serial port --------------------
# Define serial port settings
SERIAL_PORT = 'COM5'  # replace with your serial port name
BAUDRATE = 9600
TIMEOUT = 1

# Define file path and open the file
FILE_PATH = 'gcode/output9.gcode'  # replace with your file path
# FILE_PATH = 'gcode/drawing2.gcode'  # replace with your file path
file = open(FILE_PATH, 'r')

# Initialize the serial port
ser = serial.Serial(SERIAL_PORT, BAUDRATE, timeout=TIMEOUT)

# Wait for the serial connection to be established
time.sleep(2)
start = 0

# Loop through the file and send each line to the serial port
for line in file:
    # Remove any newlines or carriage returns from the line
    line = line.strip()

    # Send the line to the serial port
    ser.write(line.encode())

    stop = time.time()

    elapsed_time = stop - start
    print('Execution time:', elapsed_time, 'seconds')

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
ser.close()
