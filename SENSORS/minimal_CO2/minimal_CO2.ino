#include "MQ135.h"
MQ135 gasSensor = MQ135(A0);

#define RZERO 900 //76.63

void setup() {
  Serial.begin(115200);
}

void loop() {
float ppm = gasSensor.getPPM();
  Serial.println(ppm);
  delay(500);      
}
