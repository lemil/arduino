/*
  ESP8266 Blink by Simon Peter
  Blink the blue LED on the ESP-01 module
  This example code is in the public domain

  The blue LED on the ESP-01 module is connected to GPIO1
  (which is also the TXD pin; so we cannot use Serial.print() at the same time)

  Note that this sketch uses LED_BUILTIN to find the pin with the internal LED
*/


void blink(int blinks){
  for(int i = 0; i < blinks; i++){
  digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on (Note that LOW is the voltage level
  delay(100);                      // Wait for a second
  digitalWrite(LED_BUILTIN,   HIGH);  // Turn the LED off by making the voltage HIGH
  delay(100);
  }
}



void setup() {
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  blink(5);
}

// the loop function runs over and over again forever
void loop() {
  blink(1);
  delay(2000);                      // Wait for two seconds (to demonstrate the active low LED)
}
