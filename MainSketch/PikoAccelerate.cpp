#include "PikoAccelerate.h"

int32_t ax, ay, az;
float a, afiltered, a_ave, a_std;
int steps = 0;
bool stepping = false;
MotionState motionType = idling;

float getMagnitude(int32_t x, int32_t y, int32_t z){
  return sqrt((float)x*x + (float)y*y + (float)z*z);
}

void determineMovementType(float ave, float std){

  if(std>EXERCISING_THRESHOLD){
    if(ave>SPRINTING_THRESHOLD){
      motionType = sprinting;
      return;
    }
    else if(ave>RUNNING_THRESHOLD){
      motionType = running;
      return;
    }
    else if(ave>WALKING_THRESHOLD){
      motionType = walking;
      return;
    }
    else{}
  }
  motionType = idling;
  return;
}

void countSteps(float a, MotionState movementType){
  switch(movementType){
    case idling:
      return;
    case walking:
      takeStep(a, WALKING_THRESHOLD);
      break;
    case running:
      takeStep(a, RUNNING_THRESHOLD);
      break;
    case sprinting:
      takeStep(a, SPRINTING_THRESHOLD);
      break;
  }
  return;
}

void takeStep(float a, int threshold){
  if (a>threshold+500 && !stepping){
      steps = steps+1;
      stepping = true;
    }
    if (a<threshold+500){
      stepping = false;
    }
    return;
}