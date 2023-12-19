#include "simple_matrix.h"  //Import the library

/*
 * This is where you can copy and paste the array generated by the python software 
 * found here: https://github.com/Electro707/LED_Matrix_Generator
 */
uint8_t TEST_BITMAP[] = {
};

/*
Initialize the library. The 4 means that the CS pin is connected to pin D4.
You can change this to whatever you want, just make sure to connect the CS
pin to the pin number.

The disp means that any future function calls to the library uses "disp" as the
library's object name. For example, the library has a function called
"setIntensity", you need to write "disp.setIntensity" to call that function.

If you notice that the display is upside-down per display, change 
simpleMatrix disp(4); to simpleMatrix disp(4, true);
*/
simpleMatrix disp(4);

// This code will run only once when the Arduino is turned on.
void setup(){
  // Starts the library
  disp.begin();
  // Set the LED's intensity. This value can be anywhere between 0 and 15.
  disp.setIntensity(0x02);
  // Send the custom bitmap that is 32 columns wide (4 matrices * 8 columns each) starting at position #0
  disp.sendColumnBuffer(TEST_BITMAP, 4*8, 0); 
}

// After void setup(), this code will run and loop forever.
void loop(){
}
