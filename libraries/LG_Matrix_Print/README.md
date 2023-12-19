# LG Matrix Print

The library supports LED-matrix displays using MAX7219 controller. The library allows to print any text to the LED-matrix on a very simple way. Long strings can be outputted in ticker mode where text moves from right to left. The maximum string length is 32000. The library uses hardware SPI. The library is based on the LEDMatrixDriver library by Bartosz Bielawski.

The following functions are implemented:


- **LG_Matrix_Print(uint8_t N, uint8_t csPin, uint8_t flags = 0)**
constructor of the class. **N** is the number of 8x8 LED matrix modules (max. 255). **csPin** is the GPIO number of the pin used for chip select. **flags** is a bit field, you can invert pixel order in x direction within a single 8x8 matrix (bit0), you can invert the order of 8x8 matrices in x direction (bit1) and finally the order of rows in y direction (bit2).
- **void setEnabled(bool enabled)** the MAX7219 will be enbabled
- **void setIntensity(uint8_t level)** the intensity of the LEDs can be controlled. 0 means dark, 15 means the maximum intensity.
- **void display()** this function is required to move the content of the buffer memory to the display. This function will be called implicit, if the ticker function will be called.
- **void setPixel(int16_t x, int16_t y, bool enabled)** the function sets or clears a certain pixel within the buffer.
- **void clear()** the complete buffer will be set to 0. To clear the display itself, the function display() has to be called.
- **void drawCharacter( uint8_t* bitmap, int x,int width )** with this function, you can print a special character not in the supplied font. The **bitmap** points to an array of 8 bytes. The bits will be displayed (1 = led on) from left to right starting on position **x** until **width** is reached. In the y direction bytes from the array will be used from top to down.
- int **pixelLength(String text)** since the font uses different character width, this function returns the number of pixels in x-direction for the given **text**.
- **int printText(int start, String text, boolean isUTF8 = true)** the function writes the given **text** starting at position **start** into the buffer. If **isUTF8** is true (default) the text can contain UTF8 characters like German umlauts.
- **void ticker(String message, uint16_t wait, int16_t start)** the given **message** will be sent to the display. the start position will be decremented every **wait** seconds until the complete **message** was outputted. The position **start** defines the position for the first output.
- **void ticker(String message, uint16_t wait)** the same function, but for the first output, the right end of the display will be used.
- **boolean updateTicker()** this function has to be called in the loop function, if ticker should be used. The function does the decrement of the position and the resend to the display. **false** will be returned if the complete message was outputted.
- **void stopTicker()** this function stops ticker immediately independent if the **message** was complete. 
