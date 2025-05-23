#include <FiltersFromGit.h>
#include <DFRobot_LIS.h>
//#include <DFRobot_LIS2DH12.h>
//#include <DFRobot_LIS2DW12.h>
#include "PikoAccelerate.h"
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <AnimatedGIF.h>
#include "piko_idle.h"  // Replace with your actual .h file

// Define your MACROS
#define TFT_CS     5
#define TFT_RST    6
#define TFT_DC     7

//Function declarations:
void GIFDraw(GIFDRAW *pDraw);


//Object initilisations
DFRobot_LIS331HH_I2C acce(&Wire, I2C_ACCE_ADDRESS); //creates an accelerometer object that communicates via I2C
FilterOnePole myAccelerationFilter(LOWPASS, fc); //creates the filter object for accelerometer data
RunningStatistics myAccelerationStats;//creates an object that continously monitors acceleration mean and std
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
AnimatedGIF gif;

//Global vars
char* overlayText = "0";

void setup() {
  //Serial set
  Serial.begin(115200);
  while(!Serial){};
  while(!acce.begin()){
    Serial.println("Initialization failed, please check the connection and I2C address - must be");
  }

  //take statistics averages/std's set-up
  myAccelerationStats.setWindowSecs(WINDOW);
  motionType = idling;

  //accelerometer set up
  Serial.print("chip id : ");
  Serial.println(acce.getID(),HEX);
  acce.setRange(/*range = */DFRobot_LIS::eLis331hh_12g);
  acce.setAcquireRate(/*rate = */DFRobot_LIS::eNormal_50HZ);

    // Initialize display
  tft.init(240, 240);  // Use your screen resolution
  tft.setRotation(2);  // Adjust rotation if needed
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE);       // Choose your text color
  tft.setTextSize(2);                   // Adjust as needed
  tft.setCursor(10, 10);                // X, Y position
  // Added some important stuff here


  tft.invertDisplay(false);


  // Initialize GIF decoder
  gif.begin();  // No endian flag needed for Adafruit library
}

void loop() {

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
  
  determineMovementType(a_ave, a_std);

  //#########IMPORTANT##########
  // Replace the name "pixel_swamp_240_320" with the name that your data array in the .h file"
  //############################
  if (gif.open((uint8_t *)piko_idle, sizeof(piko_idle), GIFDraw)) {
    Serial.printf("GIF opened: %d x %d\n", gif.getCanvasWidth(), gif.getCanvasHeight());

    while (gif.playFrame(true, NULL)) {
      yield();  // Keep WiFi/OS tasks alive on ESP32
      getacceleration();
    }

    gif.close();
  } else {
    Serial.println("Failed to open GIF");
  }
}



/********************************************************************************************************************/
/************************************************Function Definitions************************************************/
/********************************************************************************************************************/





void GIFDraw(GIFDRAW *pDraw) {
  if (pDraw->y >= tft.height()) return;

  static uint16_t lineBuffer[320];  // Enough for full width

  uint8_t *s = pDraw->pPixels;
  uint8_t *pal = (uint8_t *)pDraw->pPalette;

  for (int x = 0; x < pDraw->iWidth; x++) {
    if (pDraw->ucHasTransparency && *s == pDraw->ucTransparent) {
      lineBuffer[x] = tft.color565(0, 0, 0);  // Optional: treat as black 
      s++;
      continue;
    }
    uint8_t index = *s++;
    lineBuffer[x] = tft.color565(pal[index * 3], pal[index * 3 + 1], pal[index * 3 + 2]);
  }

  tft.drawRGBBitmap(pDraw->iX, pDraw->iY + pDraw->y, lineBuffer, pDraw->iWidth, 1);
  if (pDraw->y == (pDraw->iHeight - 1)) {
    tft.setTextColor(ST77XX_WHITE, ST77XX_WHITE); // Optional: erase previous text background
    tft.setTextSize(2);
    tft.setCursor(10, 10);
    tft.print(String(steps));
    // tft.drawChar(10, 10, 'P', ST77XX_WHITE, ST77XX_WHITE, 2);
  }            // Your text here

}