#include "Main.h"

void setup() {
  Serial.begin(115200);

  // Setup output pins
  pinMode(CHARGER_PIN, OUTPUT);
  digitalWrite(CHARGER_PIN, HIGH); // High means off

  pinMode(FAN_PIN, OUTPUT);
  digitalWrite(FAN_PIN, LOW);

  pinMode(LED_PIN, OUTPUT); 
  digitalWrite(LED_PIN, LOW);

  tStateMachine.attach(STATE_TICK, stateMachineRun); // Start state-machine ticker
  resetCharger(); // Setup initial state

  if (ENABLE_DATA_WIFI) { wWifi.initWifi(); } // Initialise wifi connection
}  

/**
 * Loop is only used for sending data: If sending data call is called somewhere in the state-tick-cyclcus it doesn't have enough time to send the data. 
*/
void loop() {
  if (iSendState == 1) { // Send normal data
    wWifi.sendNormal(fTemperatureT1, fTemperatureT2, fVoltageV1, fVoltageV2);
    iSendState = 0;
  } else if (iSendState == 2) { // Send final data (stopcode)
    wWifi.sendFinal(iStopCode);
    iSendState = 0;
  }
}

// t i m e U p d a t e ()
// ============================
// Updates 'time_var' and if then 'limit' is exceeded it calls the function 'call'
//
void timeUpdate(float &time_var, float limit, void (*call)()) {
  time_var += STATE_TICK;
  if (time_var >= limit) {
    time_var = 0;
    (*call)();
  }
}

// t i m e U p d a t e R e a l ()
// ============================
// Same as timeUpdate() but updates timer-variable with real time difference instead of 'STATE_TICK' constant
// Corrects for time-inconsistency due to miliDelay() calls
//
// This function is used for very time sensitivie tasks
//
void timeUpdateReal(float &time_var, float limit, void (*call)(), unsigned long &prev_time) {
  time_var += (float)(millis() - prev_time);
  prev_time = millis();
  if (time_var >= limit) {
    time_var = 0;
    (*call)();
  }
}

// r e s e t T i m e r s ()
// ============================
// Reset all state-timers from the state-machine
//
void resetTimers() { 
  fTimeState1 =  fTimeState2_end = fTimeState2_timeout = fTimeState3_led = fTimeState3_end = 0; 
}

// s t a t e M a c h i n e R u n ()
// ============================
// State Machine called by State-ticker 
//
void stateMachineRun() {
  switch(iGlobalState) {
    case 0:
      timeUpdate(fTimeState1, CHECK_START, checkStart);
      break;
    case 1:
      timeUpdate(fTimeState2_end, CHECK_END, checkStop);
      if (ENABLE_DATA_WIFI) {timeUpdate(fTimeState2_server, SEND_DATA_WIFI, sendNormal); } // Only send data if wifi is enabled
      checkPulsate(); // Pulsate charger if needed
      timeUpdateReal(fTimeState2_timeout, (TIMEOUT * 3600 * 1000), timeOut, ulTimeoutPrev); // We use timeUpdateReal(), since for timeOut() we can't tolerate time-inconsistencies
      break;
    case 2:
      timeUpdate(fTimeState3_led, LED_SWITCH, ledSwap); // Note to myself: If you place this line below the CHECK_RESET line, led could fail => checkReset turns it off, but ledSwap can still turn it on
      timeUpdate(fTimeState3_end, CHECK_RESET, checkReset);
      break;
  }
}

// m e a s u r e A n a l o g ()
// ============================
// Measure temperatures and voltages and calculate dT1/dt and dV2
//
void measureAnalog() {
  float t1 = aiAnalog.measureTemp(true);
  fdT1dt = ((t1 - fTemperatureT1) / (float)CHECK_END) * 60; // Being measured every 'CHECK_END' seconds
  fTemperatureT1 = t1;

  fTemperatureT2 = aiAnalog.measureTemp(false);
  fVoltageV1 = aiAnalog.measureVolt(true);

  float v2 = aiAnalog.measureVolt(false); 
  fdV2 = v2 - fVoltageV2;
  fVoltageV2 = v2; // TODO: Turn charger off to correctly measure voltage (Or should it?)*/

  //printMeasurements();
}

// p r i n t M e a s u r e m e n t s ()
// ============================
// Print temperature/voltage information
//
void printMeasurements() {
  Serial.println("Measurements:");
  Serial.printf("T1: %f \n", fTemperatureT1);
  Serial.printf("T2: %f \n", fTemperatureT2);
  Serial.printf("V1: %f \n", fVoltageV1);
  Serial.printf("V2: %f \n", fVoltageV2);
}

// s t a r t C h a r g e r ()
// ============================
// Start charging process (State 0 -> 1)
//
void startCharger() {
  iGlobalState = 1;
  resetTimers();

  iPulsateCounter = 100; // Trigger pulsate calculation

  setLed(true); // Turn led on
  digitalWrite(CHARGER_PIN, LOW); // Turn charger on

  Serial.printf("state: %i \n", iGlobalState);
}

// s t o p C h a r g e r ()
// ============================
// Stop charging process (State 1 -> 2)
// Sends stop_code to Thingspeak-server
//
void stopCharger(int stop_code) {
  if (ENABLE_DATA_WIFI) { iStopCode = stop_code; iSendState = 2; } // Send stop-code only if wifi is enabled

  iGlobalState = 2;
  resetTimers();
  aiAnalog.setChannel(CHANNEL_V2); // Put Multiplexer in battery-voltage-state (only measurement needed in State 0, 2)

  iPulsateCounter = 0; // Reset pulsate counter

  digitalWrite(FAN_PIN, LOW); // Turn fan off
  bFanRunning = false;
  digitalWrite(CHARGER_PIN, HIGH); // Turn charger off

  Serial.printf("state: %i \n", iGlobalState);
}

// r e s e t C h a r g e r ()
// ============================
// Reset charger (State 2 -> 0)
//
void resetCharger() {
  iGlobalState = 0;
  resetTimers();

  setLed(false); // Turn led off

  Serial.printf("state: %i \n", iGlobalState);
}

// c a l c u l a t e P u l s a t e P a u s e ()
// ============================
// Returns number between 0 and 100 indicating for what percentage the charger should be turned on (100 = no pause = always on)
//
int calculatePulsatePause() {
  if (fTemperatureT2 <= T2_PULSATE_START) { // No pulsating needed
    return 100;
  } else { // Else, calculate pulsate percentage
    float percentage = (int) (((fTemperatureT2 - T2_PULSATE_START) / (T2_END - T2_PULSATE_START)) * 100);
    return (100 - percentage); // The closer to T2_END the smaller the percentage should be
  }
}


// t i m e O u t ()
// ============================
// Called if charger-time is finished
//
void timeOut() {
  stopCharger(9);
}

 
// c h e c k S t a r t ()
// ============================
// Check charger for starting conditions (switch on && battery connected)
//
void checkStart() {
  if (sSwitch.getState() && aiAnalog.measureVolt(false) >= VBATT_TRESHOLD) { // switch on && battery connected
    startCharger();
  }
}

// c h e c k S t o p ()
// ============================
// Check charger for stopping conditions (temperatures / voltages)
//
void checkStop() {
  measureAnalog(); // Measure temperatures and voltages 
  checkFan(); // Check if fan should be turned on/off

  if (!sSwitch.getState()) { // Switch open
    stopCharger(1);
  }

  if (fVoltageV2 <= VZERO_TRESHOLD) { // Battery disconnect TODO REMOVE THIS ONE DOESN'T WORK => To make it work charger must be turned off and then measure
    stopCharger(2);
  }

  if (fVoltageV1 <= VZERO_TRESHOLD && fVoltageV2 >= VBATT_TRESHOLD) { // Charger-circuit not working
    stopCharger(3);
  }

  if (fTemperatureT1 > T1_END) { // Battery temperature exceeded max temperature
    stopCharger(8);    
  }

  if (fTemperatureT2 >= T2_END) { // Heatsink overheat
    stopCharger(4);
  }

  if (fdT1dt >= DT_T1_END) {
    fT1dur += CHECK_END;

    if (fT1dur >= DELTA_T1_DUR) { // Battery temperature rising too fast
      stopCharger(5);
    }
  } else {
    fT1dur = 0;
  }

  if (fdV2 <= DV_V2_END)  {
    fV2dur += CHECK_END;

    if (fV2dur >= DELTA_V2_DUR) { // Negative battery voltage drop exceeded min value, indicating full charge
      stopCharger(6);
    }
  } else {
    fV2dur = 0;
  }

  if (fVoltageV2 >= V2_END) { // Battery voltage exceeded max voltage
    stopCharger(7);  
  }
}

// c h e c k R e s e t ()
// ============================
// Check charger for reset conditions (switch off | battery disconnect)
//
void checkReset() {
  if (!sSwitch.getState() || aiAnalog.measureVolt(false) <= VZERO_TRESHOLD) {
    resetCharger();
  }
}

// c h e c k P u l s a t e ()
// ============================
// Turn charging on/off based on pulsating pattern
//
void checkPulsate() {
  iPulsateCounter++;
  if (iPulsateCounter >= 100) { // cyclus finished
    digitalWrite(CHARGER_PIN, LOW); // Rember: 'CHARGER_PIN = LOW' means the charger is on
    iPulsatePause = calculatePulsatePause(); // Re-calculate pulsate-pause moment after a complete pulsate cyclus
    iPulsateCounter = 0; // Reset counter
    
  } else if (iPulsateCounter == iPulsatePause) { // Pulse pause moment has arrived
    digitalWrite(CHARGER_PIN, HIGH); // Turn charger off
  }
}

// l e d S w a p ()
// ============================
// Swap led state from on to off and vice versa
//
void ledSwap() { 
  setLed(!bLedState); 
}

// s e t L e d ()
// ============================
// Set led to 'led_state'
//
void setLed(bool led_state) {
  bLedState = led_state;
  digitalWrite(LED_PIN, led_state);
}

// c h e c k F a n ()
// ============================
// Checks if fan should be turned off or on based on current heatsink temperature (T2)
//
void checkFan() {
  if (fTemperatureT2 >= FAN_START_T2 && !bFanRunning) {
    digitalWrite(FAN_PIN, HIGH); // Turn fan on
    bFanRunning = true;
  }
  if (fTemperatureT2 <= FAN_STOP_T2 && bFanRunning) {
    digitalWrite(FAN_PIN, LOW); // Turn fan off
    bFanRunning = false;
  }
}

// s e n d N o r m a l ()
// ============================
// Enable loop() to send temperature/voltage data to Thingspeak-server
//
void sendNormal() {
  iSendState = 1;
}