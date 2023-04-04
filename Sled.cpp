// Sled.cpp
#include "Sled.h"
#include <math.h>

Sled::Sled(float x_start, float y_start, float x_board, float y_board, float r, int rsize){
  _x_position = x_start;
  _y_position = y_start;
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
    stepper_x = Sled::GetXBoard();
  }
  float stepper_y = 0;

  // Serial.print("x_board: ");
  // Serial.println(_x_board);
  // Serial.print("y_board: ");
  // Serial.println(_y_board);

  // Serial.println(stepper_y);

  float stepper_distance_straight = sqrt(pow((point_x-stepper_x),2)+pow((point_y),2));
  float a = asin((point_y)/stepper_distance_straight);
  // float a = atan((point_y)/(point_x));

  // Serial.println(asin((point_y)/stepper_distance_straight));
  // Serial.println(atan((point_y)/(point_x)));

  float b = asin((_radius/stepper_distance_straight));
  float rope_angle = a + b;

  float rope_around_spool = rope_angle * _radius;
  float rope_straight = sqrt(pow(stepper_distance_straight,2) - pow(_radius,2));
  float rope_length = rope_around_spool + rope_straight;

  return rope_length;
}

void Sled::Update(float x_destination, float y_destination){
  _d1A = Sled::CalculateRopeLength(_x_position, _y_position, 1);
  // Serial.print("d1A: ");
  // Serial.println(d1A);
  _d1B = Sled::CalculateRopeLength(x_destination, y_destination, 1);
  // Serial.print("d1B: ");
  // Serial.println(d1B);
  _d2A = Sled::CalculateRopeLength(_x_position, _y_position, 2);
  // Serial.print("d2A: ");
  // Serial.println(d2A);
  _d2B = Sled::CalculateRopeLength(x_destination, y_destination, 2);
  // Serial.print("d2B: ");
  // Serial.println(d2B);
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

void Sled::SetPosition(float x, float y){
  _x_position = x;
  _y_position = y;
}

float Sled::GetXPosition(){
  return _x_position;
}

float Sled::GetYPosition(){
  return _y_position;
}

float Sled::GetXBoard(){
  return _x_board;
}