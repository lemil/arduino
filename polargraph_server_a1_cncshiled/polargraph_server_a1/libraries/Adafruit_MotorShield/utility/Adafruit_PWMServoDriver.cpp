/*************************************************** 
  This is a library for our Adafruit 16-channel PWM & Servo driver

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products/815

  These displays use I2C to communicate, 2 pins are required to  
  interface. For Arduino UNOs, thats SCL -> Analog 5, SDA -> Analog 4

  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include <Adafruit_PWMServoDriver.h>
#include <Wire.h>
#ifdef __AVR__
 #define WIRE Wire
#else // Arduino Due
 #define WIRE Wire1
#endif

Adafruit_PWMServoDriver::Adafruit_PWMServoDriver(uint8_t addr) {
  _i2caddr = addr;
}

void Adafruit_PWMServoDriver::begin(void) {
 WIRE.begin();
 reset();
}


void Adafruit_PWMServoDriver::reset(void) {
 write8(PCA9685_MODE1, 0x0);
}

void Adafruit_PWMServoDriver::setPWMFreq(float freq) {
  //out.print("Attempting to set freq ");
  //out.println(freq);
  
  float prescaleval = 25000000;
  prescaleval /= 4096;
  prescaleval /= freq;
  prescaleval -= 1;
  out.print("Estimated pre-scale: "); out.println(prescaleval);
  uint8_t prescale = floor(prescaleval + 0.5);
  out.print("Final pre-scale: "); out.println(prescale);  
  
  uint8_t oldmode = read8(PCA9685_MODE1);
  uint8_t newmode = (oldmode&0x7F) | 0x10; // sleep
  write8(PCA9685_MODE1, newmode); // go to sleep
  write8(PCA9685_PRESCALE, prescale); // set the prescaler
  write8(PCA9685_MODE1, oldmode);
  delay(5);
  write8(PCA9685_MODE1, oldmode | 0xa1);  //  This sets the MODE1 register to turn on auto increment.
                                          // This is why the beginTransmission below was not working.
  //  out.print("Mode now 0x"); out.println(read8(PCA9685_MODE1), HEX);
}

void Adafruit_PWMServoDriver::setPWM(uint8_t num, uint16_t on, uint16_t off) {
  //out.print("Setting PWM "); out.print(num); out.print(": "); out.print(on); out.print("->"); out.println(off);

  WIRE.beginTransmission(_i2caddr);
#if ARDUINO >= 100
  WIRE.write(LED0_ON_L+4*num);
  WIRE.write(on);
  WIRE.write(on>>8);
  WIRE.write(off);
  WIRE.write(off>>8);
#else
  WIRE.send(LED0_ON_L+4*num);
  WIRE.send((uint8_t)on);
  WIRE.send((uint8_t)(on>>8));
  WIRE.send((uint8_t)off);
  WIRE.send((uint8_t)(off>>8));
#endif
  WIRE.endTransmission();
}

uint8_t Adafruit_PWMServoDriver::read8(uint8_t addr) {
  WIRE.beginTransmission(_i2caddr);
#if ARDUINO >= 100
  WIRE.write(addr);
#else
  WIRE.send(addr);
#endif
  WIRE.endTransmission();

  WIRE.requestFrom((uint8_t)_i2caddr, (uint8_t)1);
#if ARDUINO >= 100
  return WIRE.read();
#else
  return WIRE.receive();
#endif
}

void Adafruit_PWMServoDriver::write8(uint8_t addr, uint8_t d) {
  WIRE.beginTransmission(_i2caddr);
#if ARDUINO >= 100
  WIRE.write(addr);
  WIRE.write(d);
#else
  WIRE.send(addr);
  WIRE.send(d);
#endif
  WIRE.endTransmission();
}
