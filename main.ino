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
AccelStepper stepper1(1, 5, 4); // (Typeof driver: with 2 pins, STEP, DIR)
AccelStepper stepper2(1, 3, 2);
MultiStepper steppers_control;  // Create instance of MultiStepper
long go_to_position[2]; // An array to store the target position (x and y coordinates)
unsigned long t1;
unsigned long t2;
unsigned long myTime;
int rotation_size = 200;
float distance_stepper_edge = 0.008 + 0.021; //AFSTAND TUSSEN STEPPER AS EN BORD, 8mm rand + 21 mm naar as van stepper
float offset = distance_stepper_edge;
float radius = 0.015; //[m] 
float x_board = 0.8; //[m]
float y_board = 1; //[m]
float x_start = 0.2+offset; //[m]
float y_start = 0.1; //[m]
// Sled krijtje;
Sled krijtje(x_start, y_start, x_board + 2*offset, y_board, radius, rotation_size); //Create sled object


void setup() {
  stepper1.setMaxSpeed(500); // Set maximum speed value for the stepper
  stepper1.setCurrentPosition(0); // Set the current position to 0 steps

  stepper2.setMaxSpeed(500); // Set maximum speed value for the stepper
  stepper2.setCurrentPosition(0); // Set the current position to 0 steps

  steppers_control.addStepper(stepper1);
  steppers_control.addStepper(stepper2);

  Serial.begin(9600); //9600 baud rate

  Serial.println("-----------------------");
  // Serial.print("X_board setup: ");
  // Serial.println(krijtje.GetXBoard());

}

void loop() {
  if(Serial.available() > 0) {

    //=====This works=======
    int value = Serial.parseInt();  
    Serial.read();
    delay(2000);
    
    if(value==1){ //horizontaal
      // MoveStraight(0.4+offset,0.1,krijtje,steppers_control);
      // delay(2000);
      // MoveStraight(0.2+offset,0.1,krijtje,steppers_control);

      Serial.println("-----Right-----");
      StraightRelative(0.1,0,steppers_control);
      delay(1000);

      // StraightLine(0.4+offset,0.1,krijtje,steppers_control,5);
      // delay(2000);
      // StraightLine(0.2+offset,0.1,krijtje,steppers_control,5);
    }
    else if(value==2){ //verticaal
      // MoveStraight(0.2+offset,0.25,krijtje,steppers_control);
      // delay(2000);
      // MoveStraight(0.2+offset,0.1,krijtje,steppers_control);

      Serial.println("-----Left-----");
      StraightRelative(-0.1,0,steppers_control);
      delay(1000);
    }
    else if(value==3){ //diagonaal
      // MoveStraight(0.4+offset,0.2,steppers_control);
      // delay(2000);
      // MoveStraight(0.2+offset,0.1,steppers_control);

      Serial.println("-----Diagonal-----");
      StraightRelative(0.2,0.1,steppers_control);
      delay(500);
      StraightRelative(-0.2,-0.1,steppers_control);

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
void MoveStraight(float x_destination, float y_destination, MultiStepper steppers_control) {
  long go_to_position[2];

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

  stepper1.setCurrentPosition(0);
  stepper2.setCurrentPosition(0);

  // Serial.print("x before setting: ");
  // Serial.print(krijtje.GetXPosition());
  // Serial.print(", y before setting: ");
  // Serial.println(krijtje.GetYPosition());

  krijtje.SetPosition(x_destination, y_destination);

  // Serial.print("x after setting: ");
  // Serial.print(krijtje.GetXPosition());
  // Serial.print(", y after setting: ");
  // Serial.println(krijtje.GetYPosition());

}


//test-----------------



//Move in a straight line using several intermediate points
//Doesn't work 100%
// void StraightLine(float x_destination, float y_destination, Sled krijtje, MultiStepper steppers_control, int n_points) {
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
  
  // Serial.print("x before moving: ");
  // Serial.println(krijtje.GetXPosition());

  // Serial.print("target x: ");
  // Serial.print(rel_x_destination+krijtje.GetXPosition());
  // Serial.print(", target y: ");
  // Serial.println(rel_y_destination+krijtje.GetYPosition());

  float new_x = rel_x_destination+krijtje.GetXPosition();
  float new_y = rel_y_destination+krijtje.GetYPosition();

  MoveStraight(new_x, new_y, steppers_control);
  // krijtje.SetPosition(current_x, krijtje.GetYPosition());

  // Serial.print("x after moving: ");
  // Serial.println(krijtje.GetXPosition());
}

//Not finished yet
// void Arc(float x_destination, float y_destination, float r_center, Sled krijtje, MultiStepper steppers_control, int n_points) {
//   // Calculate angle between source and destination points
//   float x_midpoint = (x_destination + krijtje.GetXPosition())/2;
//   float y_midpoint = (y_destination + krijtje.GetYPosition())/2;
//   // 
// }


