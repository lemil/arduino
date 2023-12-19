/************************************
 * Version 1.0
 * Author: Gerald Lechner
 *************************************/

#include "LG_Matrix_Print.h"
#include <Arduino.h>

LG_Matrix_Print::LG_Matrix_Print(uint8_t N, uint8_t csPin, uint8_t flags):
N(N),
flags(flags),
spiSettings(5000000, MSBFIRST, SPI_MODE0),
csPin(csPin)
{
  buffer = new uint8_t[N*8];
  pinMode(csPin,OUTPUT);
  digitalWrite(csPin,1);
  SPI.begin();
#ifdef ESP8266
	SPI.setHwCs(false);
#endif

	setEnabled(false);
	setIntensity(0);
	_sendCommand(LG_Matrix_Print::TEST);			//no test
	_sendCommand(LG_Matrix_Print::DECODE);			//no decode
	_sendCommand(LG_Matrix_Print::SCAN_LIMIT | 7);	//all lines
}

//a helper function used to reverse bits in a byte
static void reverse(uint8_t& b) {
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
}

void LG_Matrix_Print::setEnabled(bool enabled)
{
	_sendCommand(ENABLE | (enabled ? 1: 0));
}

/**
 * Set display intensity
 *
 * level:
 * 	0 - lowest (1/32)
 * 15 - highest (31/32)
 */

void LG_Matrix_Print::setIntensity(uint8_t level)
{
	//maximum intensity is 0xF;
	if (level > 0xF) level = 0xF;
	_sendCommand(INTENSITY | level);
}

void LG_Matrix_Print::display()
{
	for (uint8_t y = 0; y < 8; y++)
	{
		_displayRow(y);
	}
}


void LG_Matrix_Print::setPixel(int16_t x, int16_t y, bool enabled)
{
	uint8_t* p = _getBufferPtr(x, y);
	if (!p)
		return;

	uint16_t b = 7 - (x & 7);		//bit

	if (enabled)
		*p |=  (1<<b);
	else
		*p &= ~(1<<b);
}

/**
 * This draws a sprite to the given position using the width and height supplied (usually 8x8)
 */
void LG_Matrix_Print::drawCharacter( uint8_t* bitmap, int x,int width )
{
  // The mask is used to get the column bit from the bitmap row
  uint8_t mask = B10000000;

  for( int iy = 0; iy < 8; iy++ )
  {
    for( int ix = 0; ix < width; ix++ )
    {
      setPixel(x + ix, iy, (bool)(bitmap[iy] & mask ));

      // shift the mask by one pixel to the right
      mask = mask >> 1;
    }

    // reset column mask
    mask = B10000000;
  }
}

int LG_Matrix_Print::pixelLength(String text) {
  int l = 0;
  char c;
  uint8_t index;
  int len = text.length();
  for (uint8_t i = 0; i<len; i++) l += font[text[i]-32][8];
  return l;
}

int LG_Matrix_Print::printText(int start, String text, boolean isUTF8) {
  uint8_t n,w;
  if (isUTF8) text = _encodeUnicode(text);
  uint8_t len = text.length();
  int x = start;
  for (uint8_t i = 0; i<len; i++) {
    n = text[i]-32;
    w = font[n][8];
    if ((x < N*8) && ((x+w) >= 0)) drawCharacter(font[n],x,font[n][8]);
    x+=w;
  }
  return x;
}
void LG_Matrix_Print::ticker(String message, uint16_t wait) {
  ticker(message,wait,N*8);
}


void LG_Matrix_Print::ticker(String message, uint16_t wait, int16_t startPos) {
  _tickerMode = true;
  _wait = wait;
  _last = millis();
  _message = _encodeUnicode(message);
  _tickerPos = startPos;
  clear();
  printText(_tickerPos,_message,false);
  display();
}

boolean LG_Matrix_Print::updateTicker() {
  if (_tickerMode && ((millis()-_last) > _wait)) {
    _tickerPos--;
    _last = millis();
    printText(_tickerPos,_message,false);
    _tickerMode = !((pixelLength(_message)+_tickerPos)<0);
    display();
  }
  return _tickerMode;
}

void LG_Matrix_Print::stopTicker() {
  _tickerMode = false;
}




void LG_Matrix_Print::_sendCommand(uint16_t command)
{
	SPI.beginTransaction(spiSettings);
	digitalWrite(csPin, 0);
	//send the same command to all segments
	for (uint8_t i = 0; i < N; ++i)
	{
		SPI.transfer16(command);
	}
	digitalWrite(csPin, 1);
	SPI.endTransaction();
}

uint8_t* LG_Matrix_Print::_getBufferPtr(int16_t x, int16_t y) const
{
	if ((y < 0) or (y >= 8))
		return nullptr;
	if ((x < 0) or (x >= (8*N)))
		return nullptr;

	uint16_t B = x >> 3;		//byte

	return buffer + y*N + B;
}

void LG_Matrix_Print::_displayRow(uint8_t row)
{
	//calculates row address based on flags
	uint8_t address_row = flags & INVERT_Y ? 7 - row: row;

	bool display_x_inverted = flags & INVERT_DISPLAY_X;
	bool segment_x_inverted = flags & INVERT_SEGMENT_X;

	//for x inverted display change iterating order
	//inverting segments may still be needed!
	int16_t from = display_x_inverted ? N-1:  0;		//start from ...
	int16_t to =   display_x_inverted ? -1  : N;		//where to stop
	int16_t step = display_x_inverted ? -1 :  1;		//directon

	SPI.beginTransaction(spiSettings);
	digitalWrite(csPin, 0);

	for (int16_t d = from; d != to; d += step)
	{
		uint8_t data = buffer[d + row*N];
		if (segment_x_inverted)
			reverse(data);
		uint16_t cmd = ((address_row + 1) << 8) | data;
		SPI.transfer16(cmd);
	}

	digitalWrite(csPin, 1);
	SPI.endTransaction();
}

//encode extra character from unicode for display
String LG_Matrix_Print::_encodeUnicode(String text){
  String res = "";
  uint8_t i = 0;
  char c;
  while (i<text.length()) {
    c=text[i];
    if (c==195) { //UTF8 characters German Umlaute
      i++;
      switch (text[i]) {
        case 164: c=131; break; //ä
        case 182: c=132; break; //ö
        case 188: c=133; break; //ü
        case 159: c=127; break; //ß
        case 132: c=128; break; //Ä
        case 150: c=129; break; //Ö
        case 156: c=130; break; //Ü
        default: c=0;
      }
    } else if (c == 194) { //UTF8 char for Degrees symbol
      i++;
      if (text[i] == 176) c=134; else c=0;
    } else if (c > 128) { //normal characters unchanged
      c=0;
    }
    if (c>0) res.concat(c);
    i++;
  }
  return res;
}
