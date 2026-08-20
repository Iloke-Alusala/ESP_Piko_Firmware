// PikoExample.ino

#include "PikoMBS.h"

// Same GIF data used in MainSketch.ino
#include "piko_sleep.h"
#include "piko_idle.h"
#include "piko_walk.h"
#include "piko_jog.h"
#include "piko_sprint.h"

PikoMBS piko;
int i = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial) {};

  if (!piko.begin()) {
    Serial.println("Piko failed to start - check wiring!");
  }
}

void loop() {
}
