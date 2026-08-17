// PikoExample.ino
//
// Test sketch for the new PikoHeart + PikoMBS classes.
// NOT using the Arduino "library" mechanism yet (no library.properties /
// keywords.txt / examples folder) - this just #includes the .h/.cpp files
// directly so we can test the classes before packaging them properly.
//
// To run this: put PikoHeart.h, PikoHeart.cpp, PikoMBS.h, PikoMBS.cpp in
// the SAME folder as this .ino file (the Arduino IDE will compile any
// .h/.cpp files sitting alongside the sketch automatically).

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

  // Register the same GIFs, for the same states, as MainSketch.ino used
  piko.setGif(PikoHeart::idling,    idle_v2,   sizeof(idle_v2));
  piko.setGif(PikoHeart::walking,   walk_v2,   sizeof(walk_v2));
  piko.setGif(PikoHeart::running,   jog_v2,    sizeof(jog_v2));
  piko.setGif(PikoHeart::sprinting, sprint_v2, sizeof(sprint_v2));
  piko.setGif(PikoHeart::sleeping,  sleep_v2,  sizeof(sleep_v2));

  if (!piko.begin()) {
    Serial.println("Piko failed to start - check wiring!");
  }

  // Header row for the Arduino Serial Plotter, which uses the first
  // "label:value" pairs it sees to name each line in the legend.
  Serial.println("a_ave:0,a_std:0,state:0");
}

void loop() {
  piko.update();

  // Pull the live signal-processing values straight out of PikoHeart so
  // hackathon participants can see what "average" and "std" acceleration
  // actually look like as they move the watch around.
  float a_ave = piko.pikoH.getAAve();
  float a_std = piko.pikoH.getAStd();

  i++;
  // Serial Plotter format: "label:value,label:value" - open
  // Tools > Serial Plotter in the Arduino IDE to see this as a live graph.
  if(i%10000 ==0){
  Serial.print("a_ave:");
  Serial.print(a_ave);
  Serial.print(",a_std:");
  Serial.print(a_std);
  Serial.print(",state:");
  Serial.println((int)piko.currentState * 500);
  }
}
