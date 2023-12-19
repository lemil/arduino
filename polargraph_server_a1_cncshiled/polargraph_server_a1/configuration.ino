/**
   Polargraph Server. - CORE
   Written by Sandy Noble
   Released under GNU License version 3.
   http://www.polargraph.co.uk
   https://github.com/euphy/polargraph_server_a1

  Configuration.

  This is one of the core files for the polargraph server program.
  It sets up the motor objects (AccelSteppers), and has default
  values for the motor, sprocket and microstepping combinations used
  by polargraphs so far.
*/

#ifdef ADAFRUIT_MOTORSHIELD_V1
const int stepType = INTERLEAVE;
AF_Stepper afMotorA(motorStepsPerRev, 1);
AF_Stepper afMotorB(motorStepsPerRev, 2);

void forwarda() {
  afMotorA.onestep(FORWARD, stepType);
}
void backwarda() {
  afMotorA.onestep(BACKWARD, stepType);
}
void forwardb() {
  afMotorB.onestep(FORWARD, stepType);
}
void backwardb() {
  afMotorB.onestep(BACKWARD, stepType);
}

AccelStepper motorA(forwarda, backwarda);
AccelStepper motorB(forwardb, backwardb);
#endif

#ifdef ADAFRUIT_MOTORSHIELD_V2
const int stepType = MICROSTEP;

Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_StepperMotor *afMotorA = AFMS.getStepper(motorStepsPerRev, 1);
Adafruit_StepperMotor *afMotorB = AFMS.getStepper(motorStepsPerRev, 2);

void forwarda() {
  afMotorA->onestep(FORWARD, stepType);
}
void backwarda() {
  afMotorA->onestep(BACKWARD, stepType);
}
void forwardb() {
  afMotorB->onestep(FORWARD, stepType);
}
void backwardb() {
  afMotorB->onestep(BACKWARD, stepType);
}
AccelStepper motorA(forwarda, backwarda);
AccelStepper motorB(forwardb, backwardb);
#endif

/* Motor setup if you are using serial stepper drivers
  (EasyDrivers, stepsticks, Pololu etc).

  If you are wiring up yourself, just put the pin numbers in here.

  Note that the pen lift servo usually lives on pin 9, so avoid
  that if you can. If you can't, then you know how to change it.
*/


/* D-ARDUINO ==  #-WEMOS


  CNC{XST,YST,???,XDIR,YDIR,???,ENA} = A{2,3,4,5,6,7,8} = W{D0,D1,D2,D3,???,D5,D6};  //??? No disponibles

  Podemos usar Canales X e Y y Z con Arduino. Solo necesitamos 2.

  No Podemos usar Canal Y con Wemos (falta pines para YDIR)
  Si POdemos usar Canales X y Z con Wemos

  CNC    ARDUINO  WEMOS (GPIO)
  ---------------------
  XST    D2       D0    (16)
  XDIR   D5       D3    (0)
  ZST    D4       D2    (4)
  ZDIR   D7       D5    (14)
  ENA    D8       D6    (12)

*/



#ifdef SERIAL_STEPPER_DRIVERS

#if MICROCONTROLLER == MC_UNO
//Eje X
#define MOTOR_A_ENABLE_PIN 8   //Arduino D8  / Wemos D6
#define MOTOR_A_STEP_PIN 2     //Arduino D2  / Wemos D0
#define MOTOR_A_DIR_PIN 5      //Arduino D5  / Wemos D3
//Eje Y
#define MOTOR_B_ENABLE_PIN 8   //Arduino D8  / Wemos D6
#define MOTOR_B_STEP_PIN 3      //Arduino D3  / Wemos D2
#define MOTOR_B_DIR_PIN 6      //Arduino D6  / Wemos D5
#endif

#if MICROCONTROLLER == MC_ESP8266
//Eje X
#define MOTOR_A_ENABLE_PIN 12   //Arduino D8  / Wemos D6
#define MOTOR_A_STEP_PIN 16     //Arduino D2  / Wemos D0
#define MOTOR_A_DIR_PIN 0       //Arduino D5  / Wemos D3
//Eje Z
#define MOTOR_B_ENABLE_PIN 12   //Arduino D8  / Wemos D6
#define MOTOR_B_STEP_PIN 4      //Arduino D3  / Wemos D2
#define MOTOR_B_DIR_PIN 14      //Arduino D6  / Wemos D5
#endif

AccelStepper motorA(1, MOTOR_A_STEP_PIN, MOTOR_A_DIR_PIN);
AccelStepper motorB(1, MOTOR_B_STEP_PIN, MOTOR_B_DIR_PIN);
#endif


#ifdef UNL2003_DRIVER
// Contributed by @j0nson
// Initialize ULN2003 stepper driver
// first number is type of stepper motor, 4 for a normal 4 wire step motor, 8 for a halfstepped normal 4 wire motor
//Connection Directions
// MotorA
//ULN2003  Arduino  AcceStepper Init
//IN1      2        2
//IN2      3        4
//IN3      4        3
//IN4      5        5
// MotorB
//ULN2003  Arduino  AcceStepper Init
//IN1      6        6
//IN2      7        8
//IN3      8        7
//IN4      9        9

//for a 28YBJ-48 Stepper, change these parameters above
//Step angle (8-step) 5.625deg, 64 steps per rev
//Step angle (4-step) 11.25deg, 32 steps per rev
//gear reduction ratio 1/63.68395

// motorStepsPerRev = 32 * 63.68395 = 2038; //for 4 step sequence
// motorStepsPerRev = 64 * 63.68395 = 4076; //for 8 step sequence

// motorStepsPerRev = 4076;
// mmPerRev = 63;
// DEFAULT_STEPS_PER_REV = 4076;
// DEFAULT_MM_PER_REV = 63;

AccelStepper motorA(8, 6, 8, 7, 9);
AccelStepper motorB(8, 2, 4, 3, 5);
#endif

void configuration_motorSetup()
{
#ifdef SERIAL_STEPPER_DRIVERS

  pinMode(MOTOR_A_ENABLE_PIN, OUTPUT);
  digitalWrite(MOTOR_A_ENABLE_PIN, LOW);
  pinMode(MOTOR_B_ENABLE_PIN, OUTPUT);
  digitalWrite(MOTOR_B_ENABLE_PIN, LOW);
  motorA.setEnablePin(MOTOR_A_ENABLE_PIN);
  motorB.setEnablePin(MOTOR_B_ENABLE_PIN);

#if MICROCONTROLLER == MC_UNO
  motorA.setPinsInverted(false, false, true);
  motorB.setPinsInverted(true, false, true); // this one turns the opposite direction to A, hence inverted.
#endif

#if MICROCONTROLLER == MC_ESP8266

#endif

#endif

}

void configuration_setup()
{
  mmPerStep = mmPerRev / multiplier(motorStepsPerRev);
  stepsPerMM = multiplier(motorStepsPerRev) / mmPerRev;

#ifdef ADAFRUIT_MOTORSHIELD_V2
  AFMS.begin();  // create with the default frequency 1.6KHz
#endif
  delay(500);

}
// end of Adafruit motorshield definition
// =================================================================
