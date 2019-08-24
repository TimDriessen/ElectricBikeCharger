#pragma once

#include <Arduino.h>

// ADC
#define ADC_PIN A0
#define SELECT_A 16                         // We only need SELECT_A since 2 analog inputs are used
#define SELECT_B 14
#define SELECT_C 12

#define MAX_ADC 1023.0
#define REF_VOLTAGE 1.0                     // Maximum voltage when you measure analog signal (can measure 0V ~ AREF voltage by using ADC port)

// -> Temperature
#define TEMP_REPEAT 3                       // Amount of measurements to take average of
#define ADC_CALIBRATE 30
#define BETA 3660.38                        // K (Kelvin)
#define RESIST_ROOM_TEMP 10000.0            // Ohms
#define BALANCE_RESISTOR_T1 56000.0         // Ohms 
const float rgADCtoTemp[]={100.3, 75.1, 
52.7, 40.4, 32.1, 25.7, 20.4, 16.0, 12.1, 
8.7, 5.5, 2.6};                             // lookup table for adc => temperature

// -> Voltage
#define VOLT_REPEAT 2                       // Amount of measurements to take average of
#define BALANCE_RESISTOR_V1 200000.0        // Ohms
#define BALANCE_RESISTOR_V2 5100            // Ohms

// C A n a l o g I n p u t
// =============
// Used for managing analog measurements
//
class CAnalogInput
{
  public:
    CAnalogInput();
    float measureTemp();
    float measureVolt();

  private:
    float toTemp(int raw);
    float toVolt(int raw);
};