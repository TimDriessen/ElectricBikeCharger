#pragma once

#include <Arduino.h>
#include <ESP8266WiFi.h>

#define CONNECT_WAIT_TIME 10 // Maximal time in seconds before which a connection must be established

const char* const cSsid = "Your SSID";
const char* const cPassword = "Your WiFi password";
const char* const cServer = "api.thingspeak.com";
const char* const cApiKey = "Your API key (from ThingSpeak)";

// C W i f i
// =============
// Used for sending data to Thingspeak server
//
class CWifi
{
  public:
    CWifi();
    void initWifi();

    void sendNormal(float field1, float field2, float field3, float field4);
    void sendFinal(int stop_code);

  private:
    WiFiClient client;
    bool m_bIsConnected = true;
};