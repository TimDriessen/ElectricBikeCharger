#pragma once

#include <Arduino.h> 
#include <Ticker.h>
#include "AnalogInput.h"

#define SWITCH_PIN //TODO: Choose pin       // Pin used for switch
#define LED_PIN 4                           // Pin used for state-indicating led

#define NUM_CELLS 20                        // Number of cells inside battery pack

#define STATE_TICK 0.1                      // Tick-time for state-machine (seconds)

#define CHECK_START 3//0.5                  // Tick-time: Checking start (seconds)
#define CHECK_END 3//2                      // Tick-time: Checking stop (seconds)
#define TIMEOUT 10                          // Tick-time: Timeout (hours)
#define LED_SWITCH 0.2                      // Tick-time: Led switching (seconds)
#define CHECK_RESET 3//0.5                  // Tick-time: Checking reset (seconds)

#define DT_T1_END 0.6                       // Max dT1/dt
#define DT_V2_END -0.005*NUM_CELLS          // Min dV2
#define V2_END 1.6*NUM_CELLS                // Max V2            
#define T1_END 50                           // Max T1
#define T2_END 100                          // Max T2

#define VZERO_TRESHOLD 0.2                  // <= VZERO_TRESHOLD, implies no voltage
#define VBATT_TRESHOLD 18                   // >= VBATT_TRESHOLD, implies battery connected

#define DELTA_V2_DUR 10                     // Amount of seconds 'dV2' should last, to trigger stop
#define DELTA_T1_DUR 10                     // Amount of seconds 'dT1/dt should last, to trigger stop

// Variables ==================

int iGlobalState = 0;                       // 0: Waiting (Led off) - 1: Charging (Led blinking) - 2: Charge stopped (Led on)
bool bLedState = false;             

float fTemperatureT1;                       // Battery temperature
float fTemperatureT2;                       // Heatsink temperature
float fVoltageV1;                           // Charger voltage 
float fVoltageV2;                           // Battery voltage

float fdT1dt, fdV2;                         // Deltas from temperature / voltage
float fV2dur, fT1dur = 0;                   // Duration of meeting condition

float fTimeState1 = 0;                      // Time-state variables
float fTimeState2_end, 
fTimeState2_timeout, fTimeState2_led = 0;
float fTimeState3 = 0;

Ticker tStateMachine;                       // Ticker for State Machine
CAnalogInput aiAnalog;                      // Calls default (empty) constructor

// ============================

// Functions ==================

void timeUpdate(float &time_var, float limit, void (*call)());
void resetTimers();
void stateMachineRun();

void measureAnalog();

void checkStart();
void checkStop();
void checkReset();

void startCharger();
void stopCharger();
void resetCharger();
void timeOut();

void setLed(bool led_state);
void ledSwap();

// ============================