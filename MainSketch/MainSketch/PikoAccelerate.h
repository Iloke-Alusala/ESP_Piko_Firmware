#ifndef PikoAccelerate_h
#define PikoAccelerate_h
#include <stdint.h>
#include <math.h>
// #include <DFRobot_LIS.h> //used to communicate with the accelerometer
// #include <FiltersFromGit.h> //used for preprocessing of acceleration values
// #include <Wire.h> //used to facilitate I2C communication

//Preprocessor DEFINES
#define EXERCISING_THRESHOLD 1000
#define WALKING_THRESHOLD 1500
#define RUNNING_THRESHOLD 5000
#define SPRINTING_THRESHOLD 8000
#define fc 15
#define WINDOW 3
#define I2C_ACCE_ADDRESS 0x18

enum MotionState {
  idling,
  walking,
  running,
  sprinting,
};

float getMagnitude(int32_t x, int32_t y, int32_t z);
void determineMovementType(float ave, float std);
void countSteps(float a, MotionState movementType);
void takeStep(float a, int threshold);

extern int32_t ax, ay, az;
extern float a, afiltered, a_ave, a_std;
extern int steps;
extern bool stepping;
extern MotionState motionType;

#endif