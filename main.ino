/*
    Controlling two stepper with the AccelStepper library

     by Dejan, https://howtomechatronics.com
*/

#include <AccelStepper.h>
#include <MultiStepper.h>
#include "Sled.h"
#include <math.h>

// use 1/16 steps! 200 steps per revolution -> 3200 steps per revolution
// Origin in upper left corner

// Define the stepper motor and the pins that is connected to
AccelStepper stepper1(1, 5, 4);  // (Typeof driver: with 2 pins, STEP, DIR)
AccelStepper stepper2(1, 3, 2);
MultiStepper steppers_control;  // Create instance of MultiStepper
long go_to_position[2];         // An array to store the target position (x and y coordinates)
unsigned long t1;
unsigned long t2;
unsigned long myTime;
int rotation_size = 200;
// float distance_stepper_edge = 0.008 + 0.021;  //AFSTAND TUSSEN STEPPER AS EN BORD, 8mm rand + 21 mm naar as van stepper
// float offset = distance_stepper_edge;
float offset = 8 + 21;  //AFSTAND TUSSEN STEPPER AS EN BORD, 8mm rand + 21 mm naar as van stepper
float radius = 15;              //[mm]
float x_board = 800;            //[mm]
float y_board = 1000;           //[mm]
float x_start = 200 + offset;   //[mm]
float y_start = 100;            //[mm]
// Sled krijtje;
Sled krijtje(x_start, y_start, x_board + 2 * offset, y_board, radius, rotation_size);  //Create sled object

const byte numChars = 32;
char receivedChars[numChars];
boolean newData = false;

void setup() {
  stepper1.setMaxSpeed(500);       // Set maximum speed value for the stepper
  stepper1.setCurrentPosition(0);  // Set the current position to 0 steps

  stepper2.setMaxSpeed(500);       // Set maximum speed value for the stepper
  stepper2.setCurrentPosition(0);  // Set the current position to 0 steps

  steppers_control.addStepper(stepper1);
  steppers_control.addStepper(stepper2);

  Serial.begin(9600);  //9600 baud rate

  Serial.println("-----------------------");
}

void loop() {
  if (Serial.available() > 0) {

    String received_command = Serial.readString();
    received_command.trim();
    Serial.println(received_command);

    InterpretGCode(received_command);


    // if (received_command == "A"){
    //   InterpretGCode("G0 -0.05 0.15");  
    //   InterpretGCode("G0 0.05 -0.15");
    //   InterpretGCode("G0 0.05 0.15");    
    //   InterpretGCode("G0 -0.05 0.15");
    // }

    // Serial.print("received_commandeived: ");
    // Serial.println(received_commandeivedChars);
    //=====This works=======
    // int value = Serial.parseInt();
    // Serial.read();
    // delay(2000);

    // if (value == 1) {  //horizontaal
    //   Serial.println("-----Right-----");
    //   StraightRelative(0.1, 0, steppers_control);
    //   delay(1000);
    // } else if (value == 2) {  //verticaal
    //   Serial.println("-----Left-----");
    //   StraightRelative(-0.1, 0, steppers_control);
    //   delay(1000);
    // } else if (value == 3) {  //diagonaal
    //   Serial.println("-----Diagonal-----");
    //   StraightRelative(0.2, 0.1, steppers_control);
    //   delay(500);
    //   StraightRelative(-0.2, -0.1, steppers_control);
    // } 
  }
}

// ====================Functions====================
void MoveStraight(float x_destination, float y_destination, MultiStepper steppers_control) { //Steppers are moved to destination
  long go_to_position[2];

  // Check boundaries
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

  // Calculate number of steps
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

  Serial.println("ok");
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

void StraightRelative(float rel_x_destination, float rel_y_destination, MultiStepper steppers_control) {
  MoveStraight(rel_x_destination + krijtje.GetXPosition(), rel_y_destination + krijtje.GetYPosition(), steppers_control);
}

void InterpretGCode(String command){
  int t = 0;
  int k = 10;
  String command_parameters[k];
  float x_coord;
  float y_coord;

  //Seperate command string into list of parameters
  do {
    int i1 = command.indexOf(' ');

    String next_parameter = command.substring(0, i1);
    command_parameters[t] = next_parameter;
    t++;
    command = command.substring(i1);              //Only keeps the substring of the string after index i1
    command.trim();
    // Serial.println(command);
  } while(command.indexOf(' ') != -1);

  command_parameters[t] = command;

  //Print all elements of list
  Serial.print("Parameters: ");
  for(int i=0; i<=t; i++){
    Serial.print(command_parameters[i]);
    Serial.print("; ");
  }

  //Iterate over parameters to assign their values
  //Letters and variables can be added
  for(int i=0; i<=t; i++){
    String parameter = command_parameters[i];
    if(parameter[0] == 'X'){ //The first character of the ith string in the array 
      x_coord = parameter.substring(1).toFloat();
    } else if (parameter[0] == 'Y'){
      y_coord = parameter.substring(1).toFloat();
    }
  }

  int command_type = command_parameters[0].substring(1).toInt(); //Take the int after G

  switch (command_type){ //G0, G01, etc, more can be added
    case 0:
      StraightRelative(x_coord, y_coord, steppers_control);
      break;
    case 1:
      MoveStraight(x_coord, y_coord, steppers_control) ;
      break;
 }

}

//Not finished yet
// void Arc(float x_destination, float y_destination, float r_center, Sled krijtje, MultiStepper steppers_control, int n_points) {
//   // Calculate angle between source and destination points
//   float x_midpoint = (x_destination + krijtje.GetXPosition())/2;
//   float y_midpoint = (y_destination + krijtje.GetYPosition())/2;
//   //
// }