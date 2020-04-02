#include "Wifi.h"

// C W i f i ()
// ============================
//
CWifi::CWifi() { }

// i n i t W i f i ()
// ============================
// Initialise wifi-connection. If after 'CONNECT_WAIT_TIME' seconds no connection was made, then it aborts trying to connect
//
void CWifi::initWifi() {
  WiFiClient client;
  m_bIsConnected = true; // Will be set to false if no connection has been made
  WiFi.begin(cSsid, cPassword);
  Serial.print("Connecting");

  int counter = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    counter++;
    Serial.print(".");

    if (counter >= ((float)(CONNECT_WAIT_TIME*1000)/500)) { // We've waited 'CONNECT_WAIT_TIME' seconds but still no connection has been made
      m_bIsConnected = false;
      break; // Abort waiting
    }
  }

  Serial.println("");
  if (m_bIsConnected) {
    Serial.println("WiFi Connected");
  } else {
    Serial.println("WiFi Connection failed");
  }
}

// s e n d D a t a ( )
// ============================
// Takes in the data of all Thingspeak-server fields and sends them to the Thingspeak-server
// field1: T1 - field2: T2 - field3: V1 - field4: V2
//
void CWifi::sendNormal(float field1, float field2, float field3, float field4) {
  if (m_bIsConnected) { // Only attempt to send something, when a wifi connection is established
    if(client.connect(cServer,80))
    {
      // This will convert the double variable into a string
      String postStr = cApiKey;

      postStr +="&field1=";
      postStr += String(field1);
      postStr +="&field2=";
      postStr += String(field2);
      postStr +="&field3=";
      postStr += String(field3);
      postStr +="&field4=";
      postStr += String(field4);
      postStr +="\r\n\r\n";
        
      client.print("POST /update HTTP/1.1\n");
      client.print("Host: api.thingspeak.com\n");
      client.print("Connection: close\n");
      client.print("X-THINGSPEAKapiKey: "+String(cApiKey)+"\n");
      client.print("Content-Type: application/x-www-form-urlencoded\n");
      client.print("Content-Length: ");
        
      client.print(postStr.length());
      client.print("\n\n");

      client.print(postStr);
    }
    Serial.println("Waiting..."); 
    client.stop();
  }
  Serial.println("Send complete");
}

// s e n d F i n a l ( )
// ============================
// Sends reason for stopping to Thingspeak-server
// field5: stop_code
//
void CWifi::sendFinal(int stop_code) {
  if (m_bIsConnected) { // Only attempt to send something, when a wifi connection is established
    if(client.connect(cServer,80))
    {
      // This will convert the double variable into a string
      String postStr = cApiKey;

      postStr +="&field5=";
      postStr += String(stop_code);
      postStr +="\r\n\r\n";
      
      client.print("POST /update HTTP/1.1\n");
      client.print("Host: api.thingspeak.com\n");
      client.print("Connection: close\n");
      client.print("X-THINGSPEAKapiKey: "+String(cApiKey)+"\n");
      client.print("Content-Type: application/x-www-form-urlencoded\n");
      client.print("Content-Length: ");
      
      client.print(postStr.length());
      client.print("\n\n");

      client.print(postStr);
    }
    Serial.println("Waiting..."); 
    client.stop();
  }
  Serial.println("Send complete");
}