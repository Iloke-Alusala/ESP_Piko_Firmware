#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <AnimatedGIF.h>
#include "piko_idle.h"  // Replace with your actual .h file

// Define your TFT control pins
#define TFT_CS     5
#define TFT_RST    6
#define TFT_DC     7

// Initialize the TFT object
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// Create GIF object
AnimatedGIF gif;

// Callback function for drawing each GIF line
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
}


void setup() {
  Serial.begin(115200);

  // Initialize display
  tft.init(240, 320);  // Use your screen resolution
  tft.setRotation(2);  // Adjust rotation if needed
  tft.fillScreen(ST77XX_BLACK);
  // Added some important stuff here


  tft.invertDisplay(false);


  // Initialize GIF decoder
  gif.begin();  // No endian flag needed for Adafruit library
}

void loop() {
  //#########IMPORTANT##########
  // Replace the name "pixel_swamp_240_320" with the name that your data array in the .h file"
  //############################
  if (gif.open((uint8_t *)piko_idle, sizeof(piko_idle), GIFDraw)) {
    Serial.printf("GIF opened: %d x %d\n", gif.getCanvasWidth(), gif.getCanvasHeight());

    while (gif.playFrame(true, NULL)) {
      yield();  // Keep WiFi/OS tasks alive on ESP32
    }

    gif.close();
  } else {
    Serial.println("Failed to open GIF");
  }

}
