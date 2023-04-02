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
AccelStepper stepper1(1, 3, 2); // (Typeof driver: with 2 pins, STEP, DIR)
AccelStepper stepper2(1, 5, 4);
MultiStepper steppers_control;  // Create instance of MultiStepper
long go_to_position[2]; // An array to store the target position (x and y coordinates)
unsigned long t1;
unsigned long t2;
unsigned long myTime;
int rotation_size = 200;
float radius = 0.015; //[m] 
float x_board = 0.8; //[m]
float x_start = 0.14+0.029; //[m]
float y_start = 0.1; // [m]
float distance_stepper_edge = 0.008 + 0.021; //AFSTAND TUSSEN STEPPER AS EN BORD, 8mm rand + 21 mm naar as van stepper
float offset = distance_stepper_edge;
float y_board = 1; //[m]
Sled krijtje(x_start, y_start, x_board + 2*distance_stepper_edge, y_board, radius, rotation_size); //Create sled object



void setup() {
  stepper1.setMaxSpeed(500); // Set maximum speed value for the stepper
  stepper1.setCurrentPosition(0); // Set the current position to 0 steps

  stepper2.setMaxSpeed(500); // Set maximum speed value for the stepper
  stepper2.setCurrentPosition(0); // Set the current position to 0 steps


  steppers_control.addStepper(stepper1);
  steppers_control.addStepper(stepper2);

  Serial.begin(9600); //9600 baud rate
}

void loop() {
  if(Serial.available() > 0) {

    //=====This works=======
    int value = Serial.parseInt();  
      delay(2000);
      
      if(value==1){ //horizontaal
        MoveStraight(0.4+offset,0.1,krijtje,steppers_control);
        delay(2000);
        MoveStraight(0.2+offset,0.1,krijtje,steppers_control);

        // StraightLine(0.4+offset,0.1,krijtje,steppers_control,5);
        // delay(2000);
        // StraightLine(0.2+offset,0.1,krijtje,steppers_control,5);
      }
      else if(value==2){ //verticaal
        MoveStraight(0.1+offset,0.25,krijtje,steppers_control);
        delay(2000);
        MoveStraight(0.1+offset,0.1,krijtje,steppers_control);
      }
      else if(value==3){ //diagonaal
        MoveStraight(0.4+offset,0.2,krijtje,steppers_control);
        delay(2000);
        MoveStraight(0.1+offset,0.1,krijtje,steppers_control);
      }
      else if(value==4){ //diagonaal nieuwe functie
        // StraightLine(0.3+0.029,0.2-0.037,krijtje,steppers_control,4);
        // delay(2000);
        // StraightLine(0.1+0.029,0.1-0.037,krijtje,steppers_control,4);
      }
      else if(value==5){ //diagonaal relative
        // StraightRelative(0.2,0.1,krijtje,steppers_control,4);
        // delay(2000);
        // StraightRelative(-0.2,-0.1,krijtje,steppers_control,4);
      }
      
  }  
}

// ====================Functions==================== 
void MoveStraight(float x_destination, float y_destination, Sled krijtje, MultiStepper steppers_control) {
  long go_to_position[2];

  // Calculate number of steps
  krijtje.Update(x_destination, y_destination);
  long steps1 = krijtje.CalculateSteps(1);
  Serial.print("steps1: ");
  Serial.println(steps1);
  long steps2 = krijtje.CalculateSteps(2);
  Serial.print("steps2: ");
  Serial.println(steps2);

  go_to_position[0] = steps1;
  go_to_position[1] = steps2;

  steppers_control.moveTo(go_to_position);  
  steppers_control.runSpeedToPosition();

  krijtje.SetPosition(x_destination, y_destination);
}

//Move in a straight line using several intermediate points
//Doesn't work 100%
void StraightLine(float x_destination, float y_destination, Sled krijtje, MultiStepper steppers_control, int n_points) {
  // Calculate intermediate points(delta)
  float total_x = x_destination - krijtje.GetXPosition();
  float total_y = y_destination - krijtje.GetYPosition();
  float delta_x = total_x / n_points;
  float delta_y = total_y / n_points;
  
  float current_destination[2] = {krijtje.GetXPosition() + delta_x, krijtje.GetYPosition() + delta_y};

  // Move to each point
  for (int i = 0; i < n_points; i++) {
    MoveStraight(current_destination[0],current_destination[1],krijtje,steppers_control);
    current_destination[0] += delta_x;
    current_destination[1] += delta_y;    
  } 
}

//Doesn't work yet
void StraightRelative(float rel_x_destination, float rel_y_destination, Sled krijtje, MultiStepper steppers_control, int n_points) {
  MoveStraight(rel_x_destination+krijtje.GetXPosition(), rel_y_destination+krijtje.GetYPosition(), krijtje, steppers_control);
}

//Not finished yet
// void Arc(float x_destination, float y_destination, float r_center, Sled krijtje, MultiStepper steppers_control, int n_points) {
//   // Calculate angle between source and destination points
//   float x_midpoint = (x_destination + krijtje.GetXPosition())/2;
//   float y_midpoint = (y_destination + krijtje.GetYPosition())/2;
//   // 
// }


