#pragma once

#include <Arduino.h> 

// ADC
#define ADC_PIN A0
#define MAX_ADC 1023.0

// -> Temperature
#define ADC_CALIBRATE 30
#define BETA 3660.38 // K (Kelvin)
#define RESIST_ROOM_TEMP 10000.0 // Ohms
#define BALANCE_RESISTOR 56000.0 // Ohms 
const float rgADCtoTemp[]={100.3, 75.1, 52.7, 40.4, 32.1, 25.7, 20.4, 16.0, 12.1, 8.7, 5.5, 2.6}; // lookup table for adc => temperature

// Variables ==================

float fTemperature;   

// ============================

// Functions ==================

float toTemp(int raw); 

// ============================