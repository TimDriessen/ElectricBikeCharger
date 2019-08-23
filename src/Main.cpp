#include "Main.h"

void setup() {
  Serial.begin(115200);
} 

void loop() {
  fTemperature = aiAnalog.measureTemp();
  fVoltage = aiAnalog.measureVolt();
}