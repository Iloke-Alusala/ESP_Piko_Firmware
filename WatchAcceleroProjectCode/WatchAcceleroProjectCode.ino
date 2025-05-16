//IMPORTS
#include <DFRobot_LIS.h> //used to communicate with the accelerometer
#include <FiltersFromGit.h> //used for preprocessing of acceleration values
#include <Wire.h> //used to facilitate I2C communication

//Preprocessor DEFINES
#define EXERCISING_THRESHOLD 1000
#define WALKING_THRESHOLD 1500
#define RUNNING_THRESHOLD 5000
#define SPRINTING_THRESHOLD 8000
#define fc 15
#define WINDOW 3
#define I2C_ACCE_ADDRESS 0x18

//Variable Decalrations
int32_t ax, ay, az;
float a, afiltered, a_ave, a_std;
int steps; bool stepping;

//Function declarations
float getMagnitude(int32_t x, int32_t y, int32_t z);
void determineMovementType(float ave, float std);

DFRobot_LIS331HH_I2C acce(&Wire, I2C_ACCE_ADDRESS); //creates an accelerometer object that communicates via I2C
FilterOnePole myAccelerationFilter(LOWPASS, fc); //creates the filter object for accelerometer data
RunningStatistics myAccelerationStats;//creates an object that continously monitors acceleration mean and std

enum MotionState {
  idling,
  walking,
  running,
  sprinting,
};

MotionState motionType = idling; 

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while(!Serial){};
  while(!acce.begin()){
    Serial.println("Initialization failed, please check the connection and I2C address - must be");
  }

  //take statistics averages/std's over the course of 5s
  myAccelerationStats.setWindowSecs(WINDOW);
  motionType = idling;

  //Get chip id
  Serial.print("chip id : ");
  Serial.println(acce.getID(),HEX);
  
  /**
    set range:Range(g)
              eLis331hh_6g = 6,/<±6g>/
              eLis331hh_12g = 12,/<±12g>/
              eLis331hh_24g = 24/<±24g>/
  */
  acce.setRange(/*range = */DFRobot_LIS::eLis331hh_12g);

  /**
    Set data measurement rate：
      ePowerDown_0HZ = 0,
      eLowPower_halfHZ,
      eLowPower_1HZ,
      eLowPower_2HZ,
      eLowPower_5HZ,
      eLowPower_10HZ,
      eNormal_50HZ,
      eNormal_100HZ,
      eNormal_400HZ,
      eNormal_1000HZ,
  */
  acce.setAcquireRate(/*rate = */DFRobot_LIS::eNormal_50HZ);
}

void loop() {
  ax = acce.readAccX();
  ay = acce.readAccY();
  az = acce.readAccZ();

  a = getMagnitude(ax,ay,az)-1000;

  //Serial.print(",");
  // Serial.print("AccelerationSig:");
  // Serial.println(a);

  myAccelerationFilter.input(a);
  afiltered = myAccelerationFilter.output();
  // Serial.print(",");
  // Serial.print("AcceFilterSig:");
  // Serial.println(afiltered);

  myAccelerationStats.input(afiltered);
  a_ave = myAccelerationStats.mean();
  a_std = myAccelerationStats.sigma();

  // Serial.print(",");
  Serial.print("Ave:");
  Serial.println(a_ave);
  Serial.print(",");
  Serial.print("Sigma:");
  Serial.println(a_std);

  determineMovementType(a_ave, a_std);
}

float getMagnitude(int32_t x, int32_t y, int32_t z){
  return sqrt((float)x*x + (float)y*y + (float)z*z);
}

void determineMovementType(float ave, float std){

  if(std>EXERCISING_THRESHOLD){
    if(ave>SPRINTING_THRESHOLD){
      motionType = sprinting;
      Serial.println("you are sprinting")
      return;
    }
    else if(ave>RUNNING_THRESHOLD){
      motionType = running;
      Serial.println("you are running")
      return;
    }
    else if(ave>WALKING_THRESHOLD){
      motionType = walking;
      Serial.println("you are walking")
      return;
    }
    else{
      Serial.println("Slow poke");
    }
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
    case running:
      takeStep(a, RUNNING_THRESHOLD);
    case sprinting:
      takeStep(a, SPRINTING_THRESHOLD);
  }
  Serial.println("movement undefined");
}

void takeStep(float a, int threshold){
  if (a>threshold+500 && !stepping){
      steps = steps+1;
      stepping = true;
    }
    if (a<threshold+500){
      stepping = false;
    }
}