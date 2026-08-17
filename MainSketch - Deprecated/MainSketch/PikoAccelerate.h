#ifndef PikoAccelerate_h
#define PikoAccelerate_h
#include <stdint.h>
#include <math.h>

//Preprocessor DEFINES
#define EXERCISING_THRESHOLD 5
#define WALKING_THRESHOLD 50
#define RUNNING_THRESHOLD 100
#define SPRINTING_THRESHOLD 700
#define fc 15
#define WINDOW 2
#define I2C_ACCE_ADDRESS 0x18

enum MotionState {
  NONE = -1,
  idling=0,
  walking=1,
  running=2,
  sprinting=3,
  sleeping=4,
};

float getMagnitude(int32_t x, int32_t y, int32_t z);
MotionState determineMovementType(float ave, float std);
void countSteps(float a, MotionState movementType);
void takeStep(float a, int threshold);

extern int32_t ax, ay, az;
extern float a, afiltered, a_ave, a_std;
extern int steps;
extern bool stepping;
extern MotionState motionType;

#endif