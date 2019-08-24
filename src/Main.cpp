#include "Main.h"

void setup() {
  Serial.begin(115200);
} 

void loop() {

}

// m e a s u r e A n a l o g ()
// ============================
// Measure temperature and voltage
//
void measureAnalog() {
  fTemperature = aiAnalog.measureTemp();
  fVoltage = aiAnalog.measureVolt(); 
}


// c h e c k C h a r g e r ()
// ============================
// Checks state of charger (connected / disconnected)
//
void checkCharger() {
  //TO-DO: Check charger

  // If charger is connected
  iStopCounter = CHARGER_STOP; // reset counter
  tChargerStop.attach(3600, stopCharger); // 1 hour
  tMeasure.attach(2, measureAnalog); // Do analog measurements

  // If charger is disconnected
  tChargerStop.detach();
  tMeasure.detach();
}

// s t o p C h a r g e r ()
// ============================
// Called by Ticker to stop charging process
//
void stopCharger() {
  iStopCounter--;

  //Serial.printf("Hours left: %i\n", iStopCounter);

  if (iStopCounter == 0) {
    tChargerStop.detach();
    tMeasure.detach();
    
    //Serial.println("Stopping Charger");
    //TO-DO: Stop charging process
  }
}