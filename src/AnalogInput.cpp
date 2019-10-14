#include "AnalogInput.h"

CAnalogInput::CAnalogInput() {
  pinMode(SELECT_A, OUTPUT);
  pinMode(SELECT_B, OUTPUT);

  digitalWrite(SELECT_A, LOW); 
  digitalWrite(SELECT_B, LOW);
}

// s e t C h a n n e l ()
// ============================
// Switches multiplexer to correct channel (if needed)
//
void CAnalogInput::setChannel(int c) {
  if (m_iActiveChannel != c) {
    m_iActiveChannel = c;
    decToBin(c, m_arriMuxInput ); // modify array to contain correct bit-code
    setMux(m_arriMuxInput); // set multiplexer with bit-code
    miliDelay(MUX_SWITCH_TIME);
  }
}

// m e a s u r e T e m p ( ) 
// ============================
// Applies 'TEMP_REPEAT' amount of temperature measurements and
// takes the averages from the result
// isT1 selects which temperature is measured (Battery or Heatsink)
//
float CAnalogInput::measureTemp(bool isT1) {
  setChannel( (isT1 ? CHANNEL_T1 : CHANNEL_T2) ); // Channel for which we want to measure temperature

  float total_temp = 0;
  for(int i = 0; i < TEMP_REPEAT ; i++) { total_temp += toTemp(analogRead(ADC_PIN), isT1); miliDelay(MEASURE_TIME); }

  return (total_temp / (float)TEMP_REPEAT);
}

// m e a s u r e V o l t
// ============================
// Applies 'VOLT_REPEAT' amount of voltage measurements and
// takes the averages from the result
// isV1 selects which voltage is measured (Charger or Battery)
//
float CAnalogInput::measureVolt(bool isV1) {
  setChannel( (isV1 ? CHANNEL_V1 : CHANNEL_V2) ); // Channel for which we want to measure voltage

  float total_volt = 0;
  for(int i = 0; i < VOLT_REPEAT ; i++) { total_volt += toVolt(analogRead(ADC_PIN)); miliDelay(MEASURE_TIME); }

  return (total_volt / (float)VOLT_REPEAT);
}

// t o T e m p ( ) 
// ============================
// Conversion from ADC value to temperature (celcius)
// Uses linear interpolation with a lookup table
// INPUT: ADC value | OUTPUT: Corresponding temperature
//
float CAnalogInput::toTemp(int raw, bool isT1) {

  int raw_calibrate = raw + (isT1 ? ADC_CALIBRATE_T1 : ADC_CALIBRATE_T2); // Correct for formula / NTC-resistor mismatch
  int indexA = raw_calibrate / 100; // index of first array element
  int indexB = indexA + 1;
  
  float tempA = rgADCtoTemp_T2[indexA];
  float tempB = rgADCtoTemp_T2[indexB];  
  if (isT1) {
    tempA = rgADCtoTemp_T1[indexA];
    tempB = rgADCtoTemp_T1[indexB];
  }

  int adc1 = indexA * 100;
  if (indexA == 0) { // index == 0 is only index with adc value not being a multiple of 100
    adc1 = 50; 
  } 

  int adc2 = indexB *  100;

  float temp = tempA + (raw_calibrate - adc1) * (tempB - tempA) / (adc2 - adc1); // interpolate
  
  //Serial.printf("TEMP: %f, ADC: %i ---- FIRST: adc: %i, temp: %f ---- LAST: adc: %i, temp: %f\n", temp, raw_calibrate, first_adc, first_temp, last_adc, last_temp);

  return temp;
}

// t o V o l t ( ) 
// ============================
// Conversion from ADC value to input voltage (battery)
//
float CAnalogInput::toVolt(int raw) {
  float out_voltage = (float(raw) * REF_VOLTAGE) / MAX_ADC; // convert ADC value to voltage (output voltage)
  float in_voltage = ((BALANCE_RESISTOR_A + BALANCE_RESISTOR_B) / BALANCE_RESISTOR_B) * out_voltage; // calculate input voltage

  //Serial.printf("RAW: %i, OUT: %f, IN: %f\n", raw, out_voltage, in_voltage);

  return in_voltage;
}

// s e t M u x ( )
// ============================
// Switch multiplexer to 1 of the 4 option pins
// (E.g. 10 connects to option pin 2)
//
void CAnalogInput::setMux(int (&bits)[MUX_INPUT_LENGTH]) {
  digitalWrite(SELECT_A, bits[0]); 
  digitalWrite(SELECT_B, bits[1]); 
}

// d e c i m a l T o B i n a r y ( )
// ============================
// Conversion from decimal to binary
// bits[] will contain conversion to binary from decimal 'n'
//
void CAnalogInput::decToBin(int n, int (&bits)[MUX_INPUT_LENGTH]) {
  for (int i = MUX_INPUT_LENGTH-1; i >= 0; i--) {
    bits[i] = bitRead(n, i);
  }
}

// m i l i D e l a y ()
// ============================
// Conversion from decimal to binary
//
void CAnalogInput::miliDelay(unsigned long t) {
  unsigned long start = millis();
  while((millis() - start) < t);
}