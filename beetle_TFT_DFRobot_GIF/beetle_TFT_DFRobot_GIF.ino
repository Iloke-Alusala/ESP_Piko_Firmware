#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <AnimatedGIF.h>
//#include "piko_idle_v0.h"  // Replace with your actual .h file


// actual piko gifs
// #include "idle_480.h"
// #include "walk_480.h"
// #include "jog_480.h"
// #include "sprint_480.h"
// #include "sleep_480.h"

#include "piko_idle.h"
#include "piko_walk.h"
#include "piko_jog.h"
#include "piko_sprint.h"
#include "piko_sleep.h"

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
  if (pDraw->y >= tft.height() - 37) return;  // leaves enough height to keep progress bar visible
  //if (pDraw->y >= tft.height()) return;

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


// progress bar setup
int stepCount = 0;
const int MAX_STEPS = 100;

void drawProgressBar(int steps) {
  static int lastFillWidth = -1; // remember the last fill width

  int barWidth = 160;
  int barHeight = 18;
  int thickness = 2;
  int bottomPadding = 15;
  int x = (tft.width() - barWidth) / 2;
  int y = tft.height() - barHeight - bottomPadding;

  uint16_t barColor = tft.color565(200, 200, 200);
  // piko's og colour inverted: tft.color565(216, 217, 217)
  // inverted default: ST77XX_WHITE 
  // piko's OG colour: tft.color565(39, 38, 38)

  int fillInset = thickness;
  int fillWidth = map(steps, 0, MAX_STEPS, 0, barWidth - 2 * fillInset);

  // ✅ Only redraw if the fill width changed
  if (fillWidth == lastFillWidth) return;

  lastFillWidth = fillWidth;

  // Draw thicker outline via multiple rectangles
  for (int i = 0; i < thickness; i++) {
    tft.drawRect(x - i, y - i, barWidth + 2 * i, barHeight + 2 * i, barColor);
  }

  // Clear previous fill area
  tft.fillRect(x + fillInset, y + fillInset, barWidth - 2 * fillInset, barHeight - 2 * fillInset, ST77XX_BLACK);

  // Draw current fill
  tft.fillRect(x + fillInset, y + fillInset, fillWidth, barHeight - 2 * fillInset, barColor);
}


void setup() {
  Serial.begin(115200);

  // Initialize display
  tft.init(240, 240);  // Use your screen resolution
  tft.setRotation(2);  // Adjust rotation if needed
  tft.fillScreen(ST77XX_BLACK);

  // Initialize GIF decoder
  gif.begin();  // No endian flag needed for Adafruit library

  tft.invertDisplay(false);
}

// scroll through different piko gifs
int currentGif = 3;

void loop() {
  const uint8_t* gifs[] = { idle_v2, walk_v2, jog_v2, sprint_v2, sleep_v2 };
  size_t sizes[] = { sizeof(idle_v2), sizeof(walk_v2), sizeof(jog_v2), sizeof(sprint_v2), sizeof(sleep_v2) };
  // const uint8_t* gifs[] = { idle_480, walk_480, jog_480, sprint_480, sleep_480 };
  // size_t sizes[] = { sizeof(idle_480), sizeof(walk_480), sizeof(jog_480), sizeof(sprint_480), sizeof(sleep_480) };

  if (gif.open((uint8_t *)gifs[currentGif], sizes[currentGif], GIFDraw)) {
      Serial.printf("GIF opened: %d x %d\n", gif.getCanvasWidth(), gif.getCanvasHeight());

      int frameCount = 2;

      while (gif.playFrame(true, NULL)) {
        yield();
        frameCount++;

        // Progress bar behaviour per state
        switch (currentGif) {
          case 0: // idle
            // do nothing, bar remains constant
            stepCount = 40;
            break;

          case 1: // walk
            if (frameCount % 5 == 0) stepCount++;  // slower increment
            break;

          case 2: // jog
            if (frameCount % 2 == 0) stepCount++;  // moderate increment
            break;

          case 3: // sprint
            stepCount++;  // fastest increment
            break;

          case 4: // sleep
            if (frameCount % 4 == 0 && stepCount > 0) stepCount--;  // slow decrement
            break;
        }

        // Clamp stepCount between 0 and MAX_STEPS
        stepCount = constrain(stepCount, 0, MAX_STEPS);

        drawProgressBar(stepCount);
      }

      gif.close();

      // For demo purposes, cycle manually using button or just uncomment:
      // currentGif = (currentGif + 1) % 5;  
    } else {
      Serial.println("Failed to open GIF");
    }

  //   if (gif.open((uint8_t *)gifs[currentGif], sizes[currentGif], GIFDraw)) {
  //   Serial.printf("GIF opened: %d x %d\n", gif.getCanvasWidth(), gif.getCanvasHeight());
  //   // simulate step updates
  //     stepCount = (stepCount + 1) % (MAX_STEPS + 1);
      
  //   while (gif.playFrame(true, NULL)) {
  //     yield();  // Keep WiFi/OS tasks alive on ESP32
      
  //   }
  //   // draw the progress bar after the full GIF frame is rendered
  //     drawProgressBar(stepCount);
    
  //   // hard-coded progress bar:
  //   // idle -> progress bar stops updating
  //   // walk -> progress bar updates slowly
  //   // jog -> progress bar updates quicker
  //   // sprint -> progress bar updates fastest
  //   // sleep -> progress bar decreases


  //   gif.close();
  //   //currentGif = (currentGif + 1) % 5;  // Rotate between GIFs
  // } else {
  //   Serial.println("Failed to open GIF");
  // }

  
}

