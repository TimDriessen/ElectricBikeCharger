#include "Main.h"

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT); 
  digitalWrite(LED_PIN, LOW);

  tStateMachine.attach(STATE_TICK, stateMachineRun); // Start state-machine ticker
  resetCharger(); // Setup initial state
}  

void loop() {
  
}

// t i m e U p d a t e ()
// ============================
// Updates 'time_var' and if then 'limit' is exceed it calls the function 'call'
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
  fdT1dt = ((t1 - fTemperatureT1) / (float)CHECK_END) * 60;
  fTemperatureT1 = t1;

  fTemperatureT2 = aiAnalog.measureTemp(false);
  fVoltageV1 = aiAnalog.measureVolt(true);

  float v2 = aiAnalog.measureVolt(false); 
  fdV2 = v2 - fVoltageV2;
  fVoltageV2 = v2; // TODO: CHARGER SHOULD BE TURNED OFF TO BE ABLE TO MEASURE BATTERY VOLTAGE!!! (Or should it?)*/
}

// s t a r t C h a r g e r ()
// ============================
// Start charging process (State 0 -> 1)
//
void startCharger() {
  iGlobalState = 1;
  resetTimers();
  setLed(true); // Turn led on
  Serial.printf("state: %i \n", iGlobalState);

  // TODO: Start fan
}

// s t o p C h a r g e r ()
// ============================
// Stop charging process (State 1 -> 2)
//
void stopCharger() {
  iGlobalState = 2;
  resetTimers();
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

  // TODO: Stop fan
}

// t i m e O u t ()
// ============================
// Called if charger-time is finished
//
void timeOut() {
  Serial.println("Stopping Charger - Reason: Timer");
  stopCharger();
}


// c h e c k S t a r t ()
// ============================
// Check charger for starting conditions (battery connected && switch on)
//
void checkStart() {
  startCharger(); // temporary

  // TODO: Check toggle switch
  // TODO: Check battery connected
  
  // If battery and switch => startCharger();
}

// c h e c k S t o p ()
// ============================
// Check charger for stopping conditions (temperatures / voltages)
//
void checkStop() {
  stopCharger(); // temporary
  
  // TODO: Measure temperature and voltage and calculate all dT/dt and dV values
  // TODO: Use these values to check for stopping conditions
}

// c h e c k R e s e t ()
// ============================
// Check charger for reset conditions (battery disconnect || switch)
//
void checkReset() {
  resetCharger(); // temporary

  // TODO: Check toggle switch
  // TODO: Check battery disconnected
}

// l e d S w a p
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