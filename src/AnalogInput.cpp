#include "AnalogInput.h"

CAnalogInput::CAnalogInput() {
  pinMode(SELECT_A, OUTPUT);
  pinMode(SELECT_B, OUTPUT);
  pinMode(SELECT_C, OUTPUT);

  digitalWrite(SELECT_A, LOW); 
  digitalWrite(SELECT_B, LOW);
  digitalWrite(SELECT_C, LOW);
}

// m e a s u r e T e m p ( ) 
// ============================
// Applies 'TEMP_REPEAT' amount of temperature measurements and
// takes the averages from the result
//
float CAnalogInput::measureTemp() {

  digitalWrite(SELECT_A, HIGH); 

  float total_temp = 0;
  for(int i = 0; i < TEMP_REPEAT ; i++) {
    total_temp += toTemp(analogRead(ADC_PIN));
  }
  float average_temp = total_temp / (float)TEMP_REPEAT;

  Serial.printf("Total: %f -- Average: %f\n", total_temp, average_temp);

  return average_temp;
}

// m e a s u r e V o l t
// ============================
// Applies 'VOLT_REPEAT' amount of voltage measurements and
// takes the averages from the result
//
float CAnalogInput::measureVolt() {

  digitalWrite(SELECT_A, LOW); 

  float total_volt = 0;
    for(int i = 0; i < VOLT_REPEAT ; i++) {
    total_volt += toVolt(analogRead(ADC_PIN));
  }
  float average_volt = total_volt / (float)VOLT_REPEAT;

  Serial.printf("Total: %f -- Average: %f\n", total_volt, average_volt);

  return average_volt;
}


// t o T e m p ( ) 
// ============================
// Conversion from ADC value to temperature (celcius)
// Uses linear interpolation with a lookup table
//
float CAnalogInput::toTemp(int raw) {
  int raw_calibrate = raw + ADC_CALIBRATE; // Correct for formula / NTC-resistor mismatch
  int first_index = raw_calibrate / 100; // index of first array element
  
  float first_temp = rgADCtoTemp[first_index];
  float last_temp = rgADCtoTemp[first_index+1];

  int first_adc;
  if (first_index == 0) { // index == 0 is only index with adc value not being a multiple of 100
    first_adc = 50; 
  } else {
    first_adc = first_index * 100;
  }
  int last_adc = (first_index + 1) *  100;

  float temp = first_temp + (raw_calibrate - first_adc) * (last_temp - first_temp) / (last_adc - first_adc); // interpolate
  
  //Serial.printf("TEMP: %f, ADC: %i ---- FIRST: adc: %i, temp: %f ---- LAST: adc: %i, temp: %f\n", temp, raw_calibrate, first_adc, first_temp, last_adc, last_temp);

  return temp;
}

// t o V o l t ( ) 
// ============================
// Conversion from ADC value to input voltage (battery)
//
float CAnalogInput::toVolt(int raw) {
  float out_voltage = (float(raw) * REF_VOLTAGE) / MAX_ADC; // convert ADC value to voltage (output voltage)
  float in_voltage = ((BALANCE_RESISTOR_V1 + BALANCE_RESISTOR_V2) / BALANCE_RESISTOR_V2) * out_voltage; // calculate input voltage

  //Serial.printf("RAW: %i, OUT: %f, IN: %f\n", raw, out_voltage, in_voltage);

  return in_voltage;
}