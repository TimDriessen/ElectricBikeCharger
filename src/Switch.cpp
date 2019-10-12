#include "Switch.h"

/*
  NOTE:
  - Because this class uses an interrupt, which needs to be used in static context,
    this class (in the current form) should only be used with one object.
*/

static CSwitch *instance;

// C S w i t c h ()
// ============================
//
CSwitch::CSwitch(int inputPin) {
  instance = this;

  m_iInputPin = inputPin;

  m_iUnstableState = 1; // Since input_pullup
  m_ulLastDebounceTime = 0;

  pinMode(m_iInputPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(m_iInputPin), ISRFunc, CHANGE); // Interrupt on state change
}


// s t a t e C h a n g e ()
// ============================
// Called by interrupt on state change
//
void CSwitch::stateChange() {
  m_iUnstableState = digitalRead(m_iInputPin); // save the reading
  m_ulLastDebounceTime = millis(); // At this time we experienced a (possibly unstable) state-change
}

// g e t S t a t e ()
// ============================
// Get current state of switch (Switch on = 1 | Switch off = 0)
//
bool CSwitch::getState() {
  if (m_iUnstableState != m_iStableState) {
    if ((millis() - m_ulLastDebounceTime) > DEBOUNCE_DELAY) { // No state change occured for at least debounceDelay milliseconds 
      m_iStableState = m_iUnstableState; // state is considered stable after debounceDelay
      if (m_iStableState == LOW) { // is pressed
        return true; 
      }
    }
  }
  return !m_iStableState; // Negated since with INPUT_PULLUP, HIGH = 0 and LOW = 1
}

// Forward to non-static member function.
static void ICACHE_RAM_ATTR ISRFunc() {
    instance->stateChange();
}