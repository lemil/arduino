#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
 
// Set WiFi credentials
#define WIFI_SSID "Electrolux 2.4GHz"
#define WIFI_PASS "victoriacondolores"
 
// Create a new web server
ESP8266WebServer webserver(80);


String parse_body(String str_json){
  String retval = "";
  StaticJsonDocument<200> doc;
  
  char json[51];
  str_json.toCharArray(json,51);
  
  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, json);

  // Test if parsing succeeds.
  if (error) {
    out.print(F("deserializeJson() failed: "));
    out.println(error.f_str());
  } else {
    retval = doc["cmd"].as<String>();
  }
  return retval;
}

void loop_withcmd(String cmd){
  cmd_return = "";
  cmd.toCharArray(lastCommand,51);
  out.println(lastCommand);
  comms_parseAndExecuteCommand(lastCommand);
}


void handle_cmd() { 
  //Bad Request
  if (webserver.hasArg("plain")== false){ //Check if body received
        webserver.send(400, "text/plain", "Bad request");
        return;
  }

  //Request OK
  String cmd = parse_body(webserver.arg("plain"));
  out.print("Command recieved:");
  out.println(cmd);

  //Send to main logic
  loop_withcmd(cmd);

  //message = cmd_return;

  //Send response
  webserver.send(200, "text/plain", cmd_return);
}
 
// Handle 404
void notfoundPage(){ 
  webserver.send(404, "text/plain", "404: Not found"); 
}
 
// Handle Root
void handle_root() {
  webserver.send(200, "text/plain", "Hello"); 
}


void setup_webserver()
{

  //Begin WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) { delay(100); }
 
  // WiFi Connected
  out.print("Connected! IP address: ");
  out.println(WiFi.localIP().toString());
 
  // Start Web Server
  webserver.on("/", handle_root);
  webserver.on("/cmd", handle_cmd);
  webserver.onNotFound(notfoundPage);
  webserver.begin();
 

}


 
// Listen for HTTP requests
void loop_webserver(void){ 
  webserver.handleClient(); 
}
