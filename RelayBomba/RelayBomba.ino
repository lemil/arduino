/*
 * 
 * RELAY BOMBA
 * ========================================
*/

#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebSrv.h>

#ifndef STASSID
#define STASSID "FundacionFLM"
#define STAPSK  "thousandsend1615"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);


#include "CountDown.h"
CountDown CD;  //  default millis
uint8_t lines = 0;


int ledPin = 2;
int relayPin = 0;

const char* PARAM_INPUT_1 = "output";
const char* PARAM_INPUT_2 = "state";

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>RELAY BOMBA</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    h2 {font-size: 3.0rem;}
    p {font-size: 3.0rem;}
    body {max-width: 600px; margin:0px auto; padding-bottom: 25px;}
    .switch {position: relative; display: inline-block; width: 120px; height: 70px} 
    .switch input {display: none}
    .slider {position: relative;top: 0;left: 0;right: 0; bottom: 0;background-color: #ccc;border-radius: 6px; height: 68px; width: 122px;border: 1px solid black; display: inline-block; border: 1px solid black;}
    .slider:before {position: absolute; content: ""; height: 52px; width: 52px; left: 8px; bottom: 7px; background-color: #fff; -webkit-transition: .4s; transition: .4s; border-radius: 3px; border: 1px solid black;}
    input[type="checkbox"] {display:none}
    input:checked+.slider {background-color: #b30000;}
    input:checked+.slider:before {-webkit-transform: translateX(52px); -ms-transform: translateX(52px); transform: translateX(52px)}
  </style>
</head>
<body>
  <h2>RELAY BOMBA</h2>
  %BUTTONPLACEHOLDER%
<script>function toggleCheckbox(element) {
  var xhr = new XMLHttpRequest();
  if(element.checked){ xhr.open("GET", "/update?output="+element.id+"&state=1", true); }
  else { xhr.open("GET", "/update?output="+element.id+"&state=0", true); }
  xhr.send();
}
</script>
</body>
</html>
)rawliteral";


// Replaces placeholder with button section in your web page
String processor(const String& var){
  //Serial.println(var);
  if(var == "BUTTONPLACEHOLDER"){
    String buttons = "";
    buttons += "<h4>RELAY - GPIO 0</h4><label class=\"RELAY\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"0\" " + outputState(0) + "><span class=\"slider\"></span></label>";
    buttons += "<h4>LED - GPIO 2</h4><label class=\"LED\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"2\" " + outputState(2) + "><span class=\"slider\"></span></label>";
    return buttons;
  }
  return String();
}

String outputState(int output){
  int val; 
  val = digitalRead(output);
  Serial.print("GPIO-");
  Serial.print(output);
  Serial.print(" = ");
  Serial.println(val);
  if(val == 0){
    return "checked";
  } else {
    return "";
  }
}

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
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor); 
  });

  // Send a GET request to <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
  server.on("/update", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage1;
    String inputMessage2;
    // GET input1 value on <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
    if (request->hasParam(PARAM_INPUT_1) && request->hasParam(PARAM_INPUT_2)) {
      inputMessage1 = request->getParam(PARAM_INPUT_1)->value();
      inputMessage2 = request->getParam(PARAM_INPUT_2)->value();

      //
      if(inputMessage1== "0"){ //Relay = GPIO-0
        if(inputMessage2 == "1"){
          //Reset timer
          resetTimer();
            
          // Start pump
          startPump(); 
        } else {
          // Stop pump
          stopPump(); 
        }
      }
      
      if(inputMessage1== "2"){ //LED = GPIO-2
        if(inputMessage2 == "1"){
          // Led ON
          switchLed(true); 
        } else {
          // Led OFF
          switchLed(false); 
        }
      }
    }
    else {
      inputMessage1 = "No message sent";
      inputMessage2 = "No message sent";
    }
    
    Serial.print("GPIO: ");
    Serial.print(inputMessage1);
    Serial.print(" - Set to: ");
    Serial.println(inputMessage2);
    request->send(200, "text/plain", "OK");
  });

  // Start the server
  server.begin();
  
  //Countdown
  CD.setResolution(CountDown::SECONDS);


  digitalWrite(ledPin, 1);
  digitalWrite(relayPin, 1);

}


void startPump(){
  Serial.println("Starting pump...");
  digitalWrite(relayPin, 0);
}


void stopPump(){
  Serial.println("Stopping pump...");
  int val;
  digitalWrite(relayPin, 1);

  //
  CD.stop();
}


void resetTimer(){
  Serial.println("Timer : Set 45s");
  if(CD.isRunning() == false){
    CD.start(15);
  } else {
    CD.restart();
  }
}

void switchLed(boolean bState){
  if(bState){
    digitalWrite(ledPin, 0);    
  } else {
    digitalWrite(ledPin, 1);
  }
}


void loop() {
  
  // Print timer status 
  if(CD.isRunning()){
    Serial.print("Pumpimg water, remaining ");
    int rem = CD.remaining();
    Serial.print(rem);
    Serial.println("s");

    // Timer elapsed and not started (not req.indexOf(F("/on")))
    if(CD.isStopped() or rem == 1){
      //Stop pump
      stopPump();

      //Go to sleep
      Serial.println("Standy-by mode");
    } 
  }

  delay(1000);
  
}


void statusOK(WiFiClient pclient){
  pclient.print(F("OK"));
}
