//IMPORTS
#include <DFRobot_LIS.h> //used to communicate with the accelerometer
#include <FiltersFromGit.h> //used for preprocessing of acceleration values
#include <Wire.h> //used to facilitate I2C communication
#include "PikoAccelerate.h"

DFRobot_LIS331HH_I2C acce(&Wire, I2C_ACCE_ADDRESS); //creates an accelerometer object that communicates via I2C
FilterOnePole myAccelerationFilter(LOWPASS, fc); //creates the filter object for accelerometer data
RunningStatistics myAccelerationStats;//creates an object that continously monitors acceleration mean and std


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
  // Serial.print("Ave:");
  // Serial.println(a_ave);
  // Serial.print(",");
  // Serial.print("Sigma:");
  // Serial.println(a_std);

  determineMovementType(a_ave, a_std);
}