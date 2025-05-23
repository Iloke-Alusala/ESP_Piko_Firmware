#include <FiltersFromGit.h>
#include <DFRobot_LIS.h>
#include "PikoAccelerate.h"


int32_t ax, ay, az;
float a, afiltered, a_ave, a_std;
//bool stepping = false;

static volatile int totalsteps = 0;
SemaphoreHandle_t totalstepsMutex;
volatile MotionState motionType = idling;
SemaphoreHandle_t motionTypeMutex;

// Task handles (optional, for debugging or control)
TaskHandle_t accelTaskHandle;
//TaskHandle_t displayTaskHandle;

//Objects
DFRobot_LIS331HH_I2C acce(&Wire, I2C_ACCE_ADDRESS); //creates an accelerometer object that communicates via I2C
FilterOnePole myAccelerationFilter(LOWPASS, fc); //creates the filter object for accelerometer data
RunningStatistics myAccelerationStats;//creates an object that continously monitors acceleration mean and std

// Task running on Core 0: Sample accelerometer and update motionState
void accelTask(void *pvParameters) {
  steps = 0;
  while (true) {
    //Acceleration Logic
    ax = acce.readAccX();
    ay = acce.readAccY();
    az = acce.readAccZ();
    a = getMagnitude(ax,ay,az)-1000;

    myAccelerationFilter.input(a);
    afiltered = myAccelerationFilter.output();

    myAccelerationStats.input(afiltered);
    a_ave = myAccelerationStats.mean();
    a_std = myAccelerationStats.sigma();

    MotionState newState = determineMovementType(a_ave, a_std);
    countSteps(afiltered, newState);
    Serial.print("steps: "); Serial.println(steps);

    // Protect motionState with mutex
    if (xSemaphoreTake(motionTypeMutex, portMAX_DELAY)) {
      motionType = newState;
      xSemaphoreGive(motionTypeMutex);
      Serial.println("I changed the motionType");
    }
    if(xSemaphoreTake(totalstepsMutex, portMAX_DELAY)){
      totalsteps = totalsteps+steps;
      Serial.println("I changed the total steps");
      xSemaphoreGive(totalstepsMutex);
    }
    Serial.println("Task Executed!");
    vTaskDelay(20 / portTICK_PERIOD_MS);  // Sample rate: 20ms
  }
}

// Task running on Core 1: Display motionState on LCD
// void displayTask(void *pvParameters) {
//   while (true) {
//     String stateCopy;

//     // Safely read shared state
//     if (xSemaphoreTake(motionStateMutex, portMAX_DELAY)) {
//       stateCopy = motionState;
//       xSemaphoreGive(motionStateMutex);
//     }

//     updateLCD("State: " + stateCopy);  // Your display function

//     vTaskDelay(500 / portTICK_PERIOD_MS);  // Update display every 500ms
//   }
// }

void setup() {
  //Serial set
  Serial.begin(115200);
  while(!Serial){};
  while(!acce.begin()){
    Serial.println("Initialization failed, please check the connection and I2C address (/accelerometer switch) - must be correct address");
  }

  //take statistics averages/std's set-up
  myAccelerationStats.setWindowSecs(WINDOW);
  motionType = idling;

  //accelerometer set up
  Serial.print("chip id : ");
  Serial.println(acce.getID(),HEX);
  acce.setRange(/*range = */DFRobot_LIS::eLis331hh_12g);
  acce.setAcquireRate(/*rate = */DFRobot_LIS::eNormal_50HZ);

  // Create mutex
  totalstepsMutex  =xSemaphoreCreateMutex();
  motionTypeMutex = xSemaphoreCreateMutex();

  // Create tasks pinned to specific cores
  xTaskCreatePinnedToCore(
    accelTask,           // Task function
    "AccelTask",         // Name
    2048,                // Stack size
    NULL,                // Parameter
    1,                   // Priority
    &accelTaskHandle,    // Handle
    0                    // Core 0
  );

  // xTaskCreatePinnedToCore(
  //   displayTask,
  //   "DisplayTask",
  //   2048,
  //   NULL,
  //   1,
  //   &displayTaskHandle,
  //   1                    // Core 1
  // );
}

void loop(){



}