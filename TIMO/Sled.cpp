// Sled.cpp
#include "Sled.h"
#include <math.h>

Sled::Sled(float x_start, float y_start, float x_board, float y_board, float r, int rsize){
  // _x_position = x_start;
  // _y_position = y_start;
  Sled::SetPosition(x_start, y_start);
  _x_board = x_board;
  _y_board = y_board;
  _radius = r;
  _rotation_size = rsize;
  
  _d1A = 0;
  _d1B = 0;
  _d2A = 0;
  _d2B = 0;
}

float Sled::CalculateRopeLength(float point_x, float point_y, int stepper_number){ //Calculate length of rope from spool to one point
  float stepper_x;
  if(stepper_number == 1){
    stepper_x = 0;
  }
  else{
    stepper_x = _x_board;
  }
  float stepper_y = 0;

  float stepper_distance_straight = sqrt(pow((point_x-stepper_x),2)+pow((point_y),2));    //[mm]

  float a = asin((point_y)/stepper_distance_straight);                
  float b = asin((_radius/stepper_distance_straight));
  float rope_angle = a + b;

  float rope_around_spool = rope_angle * _radius;   //[mm]                                       
  float rope_straight = sqrt(pow(stepper_distance_straight,2) - pow(_radius,2));    //[mm]
  float rope_length = rope_around_spool + rope_straight;    //[mm]

  return rope_length;
}

void Sled::Update(float x_destination, float y_destination){ //efficienter maken
  // Serial.print("Updating sled position to ");
  // Serial.print(x_destination);
  // Serial.print(", ");
  // Serial.println(y_destination);


  _d1A = Sled::CalculateRopeLength(_x_position, _y_position, 1);
  // Serial.print("d1A: ");
  // Serial.print(_d1A);
  _d1B = Sled::CalculateRopeLength(x_destination, y_destination, 1);
  // Serial.print(", d1B: ");
  // Serial.print(_d1B);
  _d2A = Sled::CalculateRopeLength(_x_position, _y_position, 2);
  // Serial.print(", d2A: ");
  // Serial.print(_d2A);
  _d2B = Sled::CalculateRopeLength(x_destination, y_destination, 2);
  // Serial.print(", d2B: ");
  // Serial.println(_d2B);

  // DEBUG: print updated lengths
  Serial.print("Rope lengths: d1A = ");
  Serial.print(_d1A);
  Serial.print(", d1B = ");
  Serial.print(_d1B);
  Serial.print(", d2A = ");
  Serial.print(_d2A);
  Serial.print(", d2B = ");
  Serial.println(_d2B);
}

long Sled::CalculateSteps(int stepper_number){
  float steps_float;
  if(stepper_number == 1){
    steps_float = (_d1A-_d1B)*_rotation_size/(2*M_PI*_radius); //Omgedraaid omdat ze niet in de juiste richting draaiden
  }
  else{
    steps_float = (_d2B-_d2A)*_rotation_size/(2*M_PI*_radius);
  }
  
  long steps = (long) steps_float;
  return steps;
}

void Sled::SetPosition(float new_x, float new_y){
  _x_position = new_x;
  _y_position = new_y;
}

void Sled::SetBoardSize(float x_board, float y_board, float x_offset, float y_offset, float ground_to_pen){
  _x_board = x_board + 2*x_offset;
  _y_board = y_board - y_offset;
  Sled::SetPosition(_x_board/2, _y_board - ground_to_pen);
}

float Sled::GetXPosition(){
  return this->_x_position;
}

float Sled::GetYPosition(){
  return this->_y_position;
}

float Sled::GetXBoard(){
  return _x_board;
}