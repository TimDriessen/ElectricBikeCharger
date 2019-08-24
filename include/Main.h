#pragma once

#include <Arduino.h> 
#include <Ticker.h>
#include "AnalogInput.h"

#define MEASURE_UPDATE 2                    // Seconds
#define CHARGER_STOP 10                     // Hours (With minimum of 1 hour)

// Variables ==================

int iStopCounter = CHARGER_STOP;            // We count every hour since Ticker doesn't go further than +- 1.9 hours

bool bChargerConnected = false;

float fTemperature;                         // battery temperature
float fVoltage;                             // battery voltage

Ticker tChargerStop;
Ticker tMeasure;
CAnalogInput aiAnalog;                      // Calls default (empty) constructor

// ============================

// Functions ==================

void measureAnalog();
void checkCharger();
void stopCharger();

// ============================