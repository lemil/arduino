/*
 * 
 * RELAY TANQUE
 * ========================================
*/

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

ESP8266WiFiMulti WiFiMulti;

HTTPClient http;

int ledPin = 2;


void setup() {

  Serial.begin(115200);
  // Serial.setDebugOutput(true);

  pinMode(ledPin,OUTPUT);

  Serial.println();
  Serial.println();
  Serial.println();

  for (uint8_t t = 4; t > 0; t--) {
    Serial.print(".");
    shortFlash(1);
    Serial.flush();
    delay(1000);
  }
  Serial.println("");

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("FundacionFLM", "thousandsend1615");
  shortFlash(3);

  http.setReuse(false);
}

void loop() {
  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {

    //Long flash
    longFlash();

    //Start pump
    WiFiClient client;
    shortFlash(1);
    http.begin(client, "http://192.168.68.115/update?output=0&state=1");
    int httpCode = http.GET();
    if (httpCode > 0) {
      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        Serial.println("HTTP Response: OK");
        
        //Short flash
        shortFlash(3);
      } else {
        longFlash();
        Serial.printf("HTTP error: %d %s\n", httpCode, http.errorToString(httpCode).c_str());
      }
    } else {
      longFlash();
      Serial.printf("HTTP error: %d %s\n", httpCode, http.errorToString(httpCode).c_str());
    }
    http.end();
  }

  //Sleep 4s
  delay(4000);
}

void longFlash(){
  //Long flash
  digitalWrite(ledPin,0);
  delay(1000);
  digitalWrite(ledPin,1);
}

void shortFlash(int cant){
  //Short flash 
  int i ;
  for (i = 0; i < cant; i++){
    digitalWrite(ledPin,0);
    delay(300);
    digitalWrite(ledPin,1);
  delay(300);
  }
  
}
