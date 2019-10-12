#pragma once

#include <Arduino.h>

// ADC
#define ADC_PIN A0
#define MUX_INPUT_LENGTH 2                      // Number of inputs of SN74HC4051      
#define MUX_SWITCH_TIME 100                     // Time for multiplexer to switch channels (milliseconds)       
#define SELECT_A 16                             
#define SELECT_B 14
                                                // Channel numbers of SN74HC4051 for analog inputs:
#define CHANNEL_T1 0                            // Battery temperature
#define CHANNEL_T2 3                            // Heatsink temperature
#define CHANNEL_V1 2                            // Charger voltage 
#define CHANNEL_V2 1                            // Battery voltage

#define MAX_ADC 1023.0
#define REF_VOLTAGE 1.0                         // Maximum voltage when you measure analog signal (can measure 0V ~ AREF voltage by using ADC port)

  // -> Temperature
  #define TEMP_REPEAT 3                         // Amount of measurements to take average of
  #define ADC_CALIBRATE 30
  #define RESIST_ROOM_TEMP 10000.0              // 25 degrees Ohms

    // -----> T1 (BETA = 3660.38K)
    #define BALANCE_RESISTOR_T1 56000.0         // Ohms 
    #define ADC_CALIBRATE_T1 30
    const float rgADCtoTemp_T1[]={100.3, 75.1, 
    52.7, 40.4, 32.1, 25.7, 20.4, 16.0, 12.1, 
    8.7, 5.5, 2.6};                             // lookup table for adc => temperature: ADC: 50, 100, 200,..., 1100

    // -----> T2 (BETA = 3518.39K)
    #define BALANCE_RESISTOR_T2 27500.0         // Ohms 
    #define ADC_CALIBRATE_T2 -38
    const float rgADCtoTemp_T2[]={129.3, 99.1,
    72.6, 58.4, 48.6, 41.2, 35.2, 30.2, 25.7, 
    21.8, 18.2, 14.9};                          // lookup table for adc => temperature: ADC: 50, 100, 200,..., 1100

  // -> Voltage
  #define VOLT_REPEAT 2                         // Amount of measurements to take average of
  #define BALANCE_RESISTOR_A 200000.0           // Ohms
  #define BALANCE_RESISTOR_B 5100               // Ohms

// C A n a l o g I n p u t
// =============
// Used for managing analog measurements
//
class CAnalogInput
{
  public:
    CAnalogInput();
    float measureTemp(bool isT1);
    float measureVolt(bool isV1);

  private:
    int m_arriMuxInput[MUX_INPUT_LENGTH];       // Input of SN74HC4051: bin:10 = dec:2 => [1] = 1; [0] = 0 
    int m_iActiveChannel;

    float toTemp(int raw, bool isT1);
    float toVolt(int raw);

    void decToBin(int n, int (&bits)[MUX_INPUT_LENGTH]);
    void setMux(int (&bits)[MUX_INPUT_LENGTH]);
};