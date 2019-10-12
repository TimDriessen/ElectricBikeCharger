#pragma once

#include <Arduino.h>

#define DEBOUNCE_DELAY 30 // the debounce time (ms); increase if the output flickers

static void ISRFunc();

// C S w i t c h
// =============
// Switch logic solving switch-bouncing
//
class CSwitch
{
  public:
    CSwitch(int inputPin);
    void stateChange();
    bool getState();

  private:
    int m_iInputPin; 
    int m_iUnstableState;        
    int m_iStableState; 
    unsigned long m_ulLastDebounceTime;     // Last time switch-state changed
};