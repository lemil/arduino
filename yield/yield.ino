/*
    This sketch demonstrates how to set up a simple HTTP-like server.
    The server will set a GPIO pin depending on the request
      http://server_ip/gpio/0 will set the GPIO2 low,
      http://server_ip/gpio/1 will set the GPIO2 high
    server_ip is the IP address of the ESP8266 module, will be
    printed to Serial when the module is connected.
*/

#include <ESP8266WiFi.h>

#ifndef STASSID
#define STASSID "FundacionFLM"
#define STAPSK  "thousandsend1615"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

int ledPin = 2;
int relayPin = 0;

void setup() {
  Serial.begin(115200); 

  // prepare LED
  pinMode(ledPin, OUTPUT);
  pinMode(relayPin, OUTPUT);
  
  digitalWrite(ledPin, 1);
  digitalWrite(relayPin, 1);

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print(F("Connecting to "));
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println();
  Serial.println(F("WiFi connected"));

  // Start the server
  server.begin();
  Serial.println(F("Server started"));
  Serial.println(WiFi.localIP());
}


void loop() {
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  
  client.setTimeout(5000); // default is 1000

  String req = client.readStringUntil('\r');

  if (req.indexOf(F("/favicon.ico")) != -1) {
    yield();
  }
  
  Serial.println(F("request: "));
  Serial.println(req);

  int val;
  if (req.indexOf(F("/on")) != -1) {
    val = 0;
    digitalWrite(relayPin, val);
  } else if (req.indexOf(F("/off")) != -1) {
    val = 1;
    digitalWrite(relayPin, val);
  } else if (req.indexOf(F("/ledon")) != -1) {
    val = 0;
    digitalWrite(ledPin, val);
  } else if (req.indexOf(F("/ledoff")) != -1) {
    val = 1;
    digitalWrite(ledPin, val);
  } else {
    Serial.println(F("invalid request"));
    val = digitalRead(relayPin);
  }

  while (client.available()) {
    client.read();
  }

  client.print(F("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nRELAY is now "));
  client.print((val) ? F("high") : F("low"));
  
  client.print(F("<br><br>RELAY <a href='http://"));
  client.print(WiFi.localIP());
  client.print(F("/on'>ON</a> <a href='http://"));
  client.print(WiFi.localIP());
  client.print(F("/off'>OFF</a>"));

  client.print(F("<br><br>LED <a href='http://"));
  client.print(WiFi.localIP());
  client.print(F("/ledon'>ON</a> <a href='http://"));
  client.print(WiFi.localIP());
  client.print(F("/ledoff'>OFF</a>"));
 
  client.print(F("</html>"));
  client.flush();
}
