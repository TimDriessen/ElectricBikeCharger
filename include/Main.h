#pragma once

#include <Arduino.h> 
#include <Ticker.h>
#include "AnalogInput.h"
#include "Switch.h"

#define CHARGER_PIN 5                       // Pin used for turning on and off charger (HIGH = Charger off | LOW = Charger on)
#define SWITCH_PIN 12                       // Pin used for switch
#define FAN_PIN 13                          // Pin used for fan
#define LED_PIN 4                           // Pin used for state-indicating led

#define NUM_CELLS 20                        // Number of cells inside battery pack

#define STATE_TICK 0.1                      // Tick-time for state-machine (seconds)

#define CHECK_START 0.3//0.5                // Tick-time: Checking start (seconds)
#define CHECK_END 0.4//2                    // Tick-time: Checking stop (seconds)
#define TIMEOUT 12                          // Tick-time: Timeout (hours)
#define LED_SWITCH 0.5//0.2                 // Tick-time: Led switching (seconds)
#define CHECK_RESET 0.3//0.5                // Tick-time: Checking reset (seconds)
#define CHECK_PULSATE 0.5                   // Tick-time: Turning charger on/off because of pulsating mode

#define DT_T1_END 0.6                       // Max dT1/dt (Celcius / Minute)
#define DV_V2_END -0.005*NUM_CELLS          // Min dV2 (Volt)
#define V2_END 2.0*NUM_CELLS                // Max V2 (Volt)            
#define T1_END 42                           // Max T1 (Celcius)
#define T2_END 70                           // Max T2 (Celcius)
#define T2_PULSATE_START 55                 // T2 for which we enter pulsating charge mode (Celcius)

#define VZERO_TRESHOLD 2.5                  // <= VZERO_TRESHOLD, implies battery disconnected (and charger off)
#define VBATT_TRESHOLD 18                   // >= VBATT_TRESHOLD, implies battery connected

#define DELTA_V2_DUR 10                     // Amount of seconds 'dV2' should last, to trigger stop
#define DELTA_T1_DUR 10                     // Amount of seconds 'dT1/dt should last, to trigger stop

#define FAN_START_T2 48                     // Temperature of heatsink at which fan should start 
#define FAN_STOP_T2 32                      // Temperature of heatsink at which fan should stop

// Variables ==================

int iGlobalState = 0;                       // 0: Waiting (Led off) - 1: Charging (Led on) - 2: Charging stopped (Led blinking)
bool bLedState = false;   
bool bFanRunning = false;    
bool bPulsatingCharge = false; 

int iPulsateCounter = 100;                  // Counter for pulsating (0 - 100)
int iPulsatePause = 0;                      // Percentage at which we stop charger (E.g. 'iPulsatePause = 20', means we charge for 20 counts and then wait for 80 counts (total of 100 counts))

float fTemperatureT1;                       // Battery temperature
float fTemperatureT2;                       // Heatsink temperature
float fVoltageV1;                           // Charger voltage 
float fVoltageV2;                           // Battery voltage

float fdT1dt, fdV2;                         // Deltas from temperature / voltage
float fV2dur, fT1dur = 0;                   // Duration for meeting stop-condition

                                            // Time-state variables
float fTimeState1 = 0;                      
float fTimeState2_end, fTimeState2_timeout = 0;
float fTimeState3_end, fTimeState3_led = 0;

unsigned long ulTimeoutPrev = 0;            // Previous time-update call, for timeUpdateReal() call

Ticker tStateMachine;                       // Ticker for State Machine
CAnalogInput aiAnalog;                      // Calls default (empty) constructor
CSwitch sSwitch(SWITCH_PIN);

// ============================

// Functions ==================

void timeUpdate(float &time_var, float limit, void (*call)());
void timeUpdateReal(float &time_var, float limit, void (*call)(), unsigned long &prev_time);
void resetTimers();
void stateMachineRun();

void measureAnalog();
void printMeasurements();

void checkStart();
void checkStop();
void checkReset();
void checkPulsate();

void startCharger();
void stopCharger();
void resetCharger();

int calculatePulsatePause();

void timeOut();

void setLed(bool led_state);
void ledSwap();

void checkFan();

// ============================