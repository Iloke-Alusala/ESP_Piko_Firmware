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

// progress bar setup
int stepCount = 0;
const int MAX_STEPS = 100;

void drawProgressBar(int steps) {
  static int lastFillWidth = -1; // remember the last fill width

  int barWidth = 160;
  int barHeight = 20;
  int thickness = 2;
  int bottomPadding = 15;
  int x = (tft.width() - barWidth) / 2;
  int y = tft.height() - barHeight - bottomPadding;

  uint16_t barColor = ST77XX_WHITE;
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
  tft.fillScreen(ST77XX_WHITE);

  // Initialize GIF decoder
  gif.begin();  // No endian flag needed for Adafruit library

  tft.invertDisplay(false);
}

void loop() {
  if (gif.open((uint8_t *)piko_idle, sizeof(piko_idle), GIFDraw)) {
    Serial.printf("GIF opened: %d x %d\n", gif.getCanvasWidth(), gif.getCanvasHeight());

    while (gif.playFrame(true, NULL)) {
      yield();  // Keep WiFi/OS tasks alive on ESP32

      // simulate step updates
      stepCount = (stepCount + 1) % (MAX_STEPS + 1);

      // draw the progress bar after the full GIF frame is rendered
      drawProgressBar(stepCount);
    }

    gif.close();
  } else {
    Serial.println("Failed to open GIF");
  }
}
