/*
    Controlling two stepper with the AccelStepper library

     by Dejan, https://howtomechatronics.com
*/

#include <AccelStepper.h>
#include <MultiStepper.h>
#include "Sled.h"
#include <math.h>

// use 1/16 steps! 200 steps per revolution -> 3200 steps per revolution
// This is defined in rotation_size
// Origin in upper left corner

// Define the stepper motor and the pins that is connected to
AccelStepper stepper1(1, 5, 4);  // (Typeof driver: with 2 pins, STEP, DIR)
AccelStepper stepper2(1, 3, 2);
MultiStepper steppers_control;  // Create instance of MultiStepper
long go_to_position[2];         // An array to store the target position (x and y coordinates)
unsigned long t1;
unsigned long t2;
unsigned long myTime;
int rotation_size = 3200;       // Microstepping, amount of steps for one rotation of stepper motor
float offset = 8 + 21;          // AFSTAND TUSSEN STEPPER AS EN BORD, 8mm rand + 21 mm naar as van stepper
float radius = 15;              // [mm]
float x_board = 800;            // [mm]
float y_board = 1000;           // [mm]
float x_start = 200 + offset;   // [mm]
float y_start = 100;            // [mm]
Sled krijtje(x_start, y_start, x_board + 2 * offset, y_board, radius, rotation_size);  //Create sled object
bool movement_type = false;     // false if absolute movement, true if relative movement

void setup() {
  stepper1.setMaxSpeed(500);       // Set maximum speed value for the stepper
  stepper1.setCurrentPosition(0);  // Set the current position to 0 steps

  stepper2.setMaxSpeed(500);       // Set maximum speed value for the stepper
  stepper2.setCurrentPosition(0);  // Set the current position to 0 steps

  steppers_control.addStepper(stepper1);  //Add both steppers to MultiStepper object;
  steppers_control.addStepper(stepper2);

  Serial.begin(9600);  //9600 baud rate
  Serial.setTimeout(10);

  Serial.println("-----------------------");
}

void loop() {
  if (Serial.available() > 0) {
    long int t3 = millis();
    String received_command = Serial.readString();
    long int t4 = millis();
    received_command.trim();
    Serial.println(received_command);
    Serial.print("Time taken by reading serial: "); Serial.print(t4-t3); Serial.println(" milliseconds");

    long int t1 = millis();
    InterpretGCode(received_command);
    long int t2 = millis();
    Serial.print("Time taken by InterpretGCode: "); Serial.print(t2-t1); Serial.println(" milliseconds");
  }
}


// ====================Functions====================
void MoveStraight(float x_destination, float y_destination, MultiStepper steppers_control, bool coordinate_type) { 
  // Steppers are moved to destination
  // X and Y coordinates, MultiStepper object, coordinate_type to set relative or absolute movement
  // false for absolute movement, true for relative movement
  long go_to_position[2];

  Serial.print("X: ");
  Serial.print(x_destination);
  Serial.print(", Y: ");
  Serial.println(y_destination);
  if(coordinate_type == true){ // When relative movement, update the coordinates
    x_destination = x_destination + krijtje.GetXPosition();
    y_destination = y_destination + krijtje.GetYPosition();
  }

  // Check boundaries of board
  if(x_destination >  x_board + 2 * offset){
    x_destination =  x_board + 2 * offset;
  } else if (x_destination < 0){
    x_destination = 0;
  }
  if(y_destination >  y_board){
    y_destination =  y_board;
  } else if (y_destination < 0){
    y_destination = 0;
  }

  // Calculate number of steps steppers need to move
  krijtje.Update(x_destination, y_destination);
  long steps1 = krijtje.CalculateSteps(1);
  Serial.print("steps1: ");
  Serial.print(steps1);
  long steps2 = krijtje.CalculateSteps(2);
  Serial.print(", steps2: ");
  Serial.println(steps2);

  go_to_position[0] = steps1;
  go_to_position[1] = steps2;

  steppers_control.moveTo(go_to_position);
  steppers_control.runSpeedToPosition();

  //Stepper position reset to 0 so that they move again next round
  stepper1.setCurrentPosition(0);
  stepper2.setCurrentPosition(0);

  //Set the internal stepper position to the destination. Currently no calibration/FB so the object needs to remember its position
  krijtje.SetPosition(x_destination, y_destination);

}

//Move in a straight line using several intermediate points
//Doesn't work 100%
// void StraightLine(float x_destination, float y_destination, MultiStepper steppers_control, int n_points) {
//   // Calculate intermediate points(delta)
//   float total_x = x_destination - krijtje.GetXPosition();
//   float total_y = y_destination - krijtje.GetYPosition();
//   float delta_x = total_x / n_points;
//   float delta_y = total_y / n_points;

//   float current_destination[2] = {krijtje.GetXPosition() + delta_x, krijtje.GetYPosition() + delta_y};

//   // Move to each point
//   for (int i = 0; i < n_points; i++) {
//     MoveStraight(current_destination[0],current_destination[1],krijtje,steppers_control);
//     current_destination[0] += delta_x;
//     current_destination[1] += delta_y;
//   }
// }

// void StraightRelative(float rel_x_destination, float rel_y_destination, MultiStepper steppers_control) {
//   MoveStraight(rel_x_destination + krijtje.GetXPosition(), rel_y_destination + krijtje.GetYPosition(), steppers_control, movement_type);
// }

void InterpretGCode(String command){
  // command is a single line of G-code, function interprets it and acts according to the command
  // In the form: G01 X200 Y300

  float x_coord;          // X and Y coordinates
  float y_coord;
  bool move = false;      // Set to true if the Gcommand is for moving (G0, G1)

  //Seperate command string into array of parameters
  do {
    int i1 = command.indexOf(' ');    // Takes index of space character in string

    String next_parameter = command.substring(0, i1); // Next parameter to consider is everything until the space
    switch(next_parameter[0]){
      case 'X':
        x_coord = next_parameter.substring(1).toFloat();
        break;
      case 'Y':
        y_coord = next_parameter.substring(1).toFloat();
        break;
      case 'G':        
        int command_type = next_parameter.substring(1).toInt(); // Take the int after G
        if (command_type == 0){   // Travel move (marker / chalk not engaged)
          move = true;
        }
        else if (command_type == 1){    // Draw move
          move = true;
        }
        else if (command_type == 90){   // Absolute
          movement_type = false;
        }
        else if (command_type == 91){   // Relative
          movement_type = true;
        }
        break;
      
      default:
        break;
    }

    command = command.substring(i1);    // Only keeps the substring of the string after index i1
    command.trim();
  } while(command != "");   // While loop until string is empty


  if(move == true){
    MoveStraight(x_coord, y_coord, steppers_control, movement_type);
    Serial.println("movement finished");
  }

  Serial.println("ok");   // This is needed for the UART communication with Python
}