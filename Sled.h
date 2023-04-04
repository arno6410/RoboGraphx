// Sled.h
#ifndef SLED_H
#define SLED_H

#include <Arduino.h>

class Sled {
  public:
    Sled(float x_start, float y_start, float x_board, float y_board, float r, int rsize);
    //void MoveSteppers();
    float CalculateRopeLength(float point_x, float point_y, int stepper_number);
    void Update(float destination_x, float destination_y);
    long CalculateSteps(int stepper_number);
    void SetPosition(float x, float y);
    float GetXPosition();
    float GetYPosition();
    float GetXBoard();
  private:
    float _x_position;
    float _y_position;
    float _x_board;
    float _y_board;
    float _radius;
    int _rotation_size;
    float _d1A;
    float _d1B;
    float _d2A;
    float _d2B;
};

#endif