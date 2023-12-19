//This example sends contnously the test text to the display
#include <LG_Matrix_Print.h>

#define LEDMATRIX_CS_PIN 16

// Number of 8x8 segments you are connecting
#define LEDMATRIX_SEGMENTS 4

#define TESTTEXT "the quick brown fox jumps over the lazy dog"

// The LEDMatrixDriver class instance
LG_Matrix_Print lmd(LEDMATRIX_SEGMENTS, LEDMATRIX_CS_PIN);

void setup() {
  Serial.begin(115200);
  lmd.setEnabled(true);
  lmd.setIntensity(2);   // 0 = low, 15 = high
  lmd.clear();
  lmd.ticker(TESTTEXT,100);
}

void loop() {
  if (!lmd.updateTicker()) lmd.ticker(TESTTEXT,100);
}
