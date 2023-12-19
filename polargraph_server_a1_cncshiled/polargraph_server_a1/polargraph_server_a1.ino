/**
*  Polargraph Server for Arduino UNO and MEGA compatible boards.
*  Written by Sandy Noble
*  Released under GNU License version 3.
*  http://www.polargraph.co.uk
*  https://github.com/euphy/polargraph_server_a1


CONFIGURATION!! Read this! Really.
==================================

Kung fu is like a game of chess. You must think first! Before you move.

This is a unified codebase for a few different versions of Polargraph Server.

You can control how it is compiled by changing the #define lines below.

There are five config sections:
1. Specify what kind of controller board you are using
2. Add some libraries if you have a MEGA
3. Specify what kind of motor driver you are using:
  i. Adafruit Motorshield v1
  ii. Adafruit Motorshield v2
  iii. Discrete stepper drivers (eg EasyDriver, stepstick, Pololu gear).*
  iv. Signal amplifier like a UNL2003*
4.  Turn on some debugging code
5.  Disable program features if you need to free up space

* For motor drivers iii and iv, you will need to change the values in
  configuration.ino to set the exact pins the drivers are wired up to.
  
*/



// 1. Specify what kind of controller board you are using
// ======================================================
// UNO or MEGA. Uncomment the line for the kind of board you have.
// Some microcontroller's names
#define MC_UNO 1
#define MC_MEGA 2
#define MC_ESP8266 3

#ifndef MICROCONTROLLER
#define MICROCONTROLLER MC_ESP8266
//#define MICROCONTROLLER MC_UNO
//#define MICROCONTROLLER MC_MEGA
#endif



// 2. Add some libraries if you have a MEGA
// ========================================
// Uncomment the SPI and SD lines below if you have a MEGA.  
// http://forum.arduino.cc/index.php?topic=173584.0
//#include <SPI.h>
//#include <SD.h>


// 3. Specify what kind of motor driver you are using
// ==================================================
// Only ONE set of lines below should be uncommented.

//   i. Adafruit Motorshield v1. The original, and still the best.
//   -------------------------------------------------------------
//#define ADAFRUIT_MOTORSHIELD_V1
//#include <AFMotor.h>

//   ii. Adafruit Motorshield v2. It's all squealy.
//   ----------------------------------------------
//#define ADAFRUIT_MOTORSHIELD_V2
//#include <Wire.h>
//#include <Adafruit_MotorShield.h>
//#include "utility/Adafruit_PWMServoDriver.h"

//   iii. Using discrete stepper drivers? (eg EasyDriver, stepstick, Pololu gear)
//   ----------------------------------------------------------------------------
//   Don't forget to define your pins in 'configuration.ino'.
#define SERIAL_STEPPER_DRIVERS 

//   iv. Using a signal amplifier like a UNL2003? 
//   --------------------------------------------
//   Don't forget to define your pins in 'configuration.ino'.
//   #define UNL2003_DRIVER


// 4.  Turn on some debugging code if you want horror
// =====================================  ============
//#define DEBUG
//#define DEBUG_COMMS
//#define DEBUG_PENLIFT
//#define DEBUG_PIXEL


// 5.  Disable program features if you need to free up space
// ========================================================
#define PIXEL_DRAWING
#define PENLIFT
#define VECTOR_LINES


// 6.  SETUP COMM CHANNEL AS TCP, SERIAL
// ========================================================
#define COMM_TCP    0
#define COMM_TCP_SERIAL 1
#define COMM_SERIAL 2
#define COMM_STACK COMM_TCP_SERIAL

/*  ===========================================================  
    These variables are common to all polargraph server builds
=========================================================== */    



#if MICROCONTROLLER == MC_ESP8266
  #define EEPROM_BEGIN 512
#endif


#include <AccelStepper.h> // from http://www.airspayce.com/mikem/arduino/AccelStepper/
#include <Servo.h>
#include <EEPROM.h>
#include "EEPROMAnything.h"

const String FIRMWARE_VERSION_NO = "1.2.1";

//  EEPROM addresses
#if MICROCONTROLLER == MC_ESP8266

const byte EEPROM_MACHINE_WIDTH = 0;  // 4 bytes
const byte EEPROM_MACHINE_HEIGHT = 4;  // 4 bytes
const byte EEPROM_MACHINE_MM_PER_REV = 8; // 8 bytes (float)
const byte EEPROM_MACHINE_STEPS_PER_REV = 16; // 4 bytes
const byte EEPROM_MACHINE_STEP_MULTIPLIER = 20; // 4 bytes

const byte EEPROM_MACHINE_MOTOR_SPEED = 24; // 8 bytes float
const byte EEPROM_MACHINE_MOTOR_ACCEL = 32; // 8 bytes float
const byte EEPROM_MACHINE_PEN_WIDTH = 40; // 8 bytes float

const byte EEPROM_MACHINE_HOME_A = 48; // 8 bytes
const byte EEPROM_MACHINE_HOME_B = 56; // 8 bytes

const byte EEPROM_PENLIFT_DOWN = 64; // 4 bytes
const byte EEPROM_PENLIFT_UP = 68; // 4 bytes

#else

const byte EEPROM_MACHINE_WIDTH = 0;
const byte EEPROM_MACHINE_HEIGHT = 2;
const byte EEPROM_MACHINE_MM_PER_REV = 14; // 4 bytes (float)
const byte EEPROM_MACHINE_STEPS_PER_REV = 18;
const byte EEPROM_MACHINE_STEP_MULTIPLIER = 20;

const byte EEPROM_MACHINE_MOTOR_SPEED = 22; // 4 bytes float
const byte EEPROM_MACHINE_MOTOR_ACCEL = 26; // 4 bytes float
const byte EEPROM_MACHINE_PEN_WIDTH = 30; // 4 bytes float

const byte EEPROM_MACHINE_HOME_A = 34; // 4 bytes
const byte EEPROM_MACHINE_HOME_B = 38; // 4 bytes

const byte EEPROM_PENLIFT_DOWN = 42; // 2 bytes
const byte EEPROM_PENLIFT_UP = 44; // 2 bytes

#endif

// Pen raising servo
Servo penHeight;

#if MICROCONTROLLER == MC_ESP8266
const byte PEN_HEIGHT_SERVO_PIN = 15; //Y-LIMIT Wemos D8 (GPIO 15)
const int DEFAULT_DOWN_POSITION = 40;
const int DEFAULT_UP_POSITION = 180;
static int penLiftSpeed = 1; // ms between steps of moving motor
#else
const byte PEN_HEIGHT_SERVO_PIN = 9; //UNL2003 driver uses pin 9
const int DEFAULT_DOWN_POSITION = 90;
const int DEFAULT_UP_POSITION = 180;
static int penLiftSpeed = 3; // ms between steps of moving motor
#endif

static int upPosition = DEFAULT_UP_POSITION; // defaults
static int downPosition = DEFAULT_DOWN_POSITION;



boolean isPenUp = false;

// Machine specification defaults
const int DEFAULT_MACHINE_WIDTH = 1190;
const int DEFAULT_MACHINE_HEIGHT = 730;
const int DEFAULT_MM_PER_REV = 95;
const int DEFAULT_STEPS_PER_REV = 400;
const int DEFAULT_STEP_MULTIPLIER = 1;

// working machine specification
static int motorStepsPerRev = DEFAULT_STEPS_PER_REV;
static float mmPerRev = DEFAULT_MM_PER_REV;
static byte stepMultiplier = DEFAULT_STEP_MULTIPLIER;
static int machineWidth = DEFAULT_MACHINE_WIDTH;
static int machineHeight = DEFAULT_MACHINE_HEIGHT;


static float currentMaxSpeed = 800.0;
static float currentAcceleration = 400.0;
static boolean usingAcceleration = true;

int startLengthMM = 800;

float mmPerStep = 0.0F;
float stepsPerMM = 0.0F;

long pageWidth = machineWidth * stepsPerMM;
long pageHeight = machineHeight * stepsPerMM;
long maxLength = 0;

//static char rowAxis = 'A';
const int INLENGTH = 50;
const char INTERMINATOR = 10;
const char SEMICOLON = ';';

float penWidth = 0.8F; // line width in mm

boolean reportingPosition = true;
boolean acceleration = true;

extern AccelStepper motorA;
extern AccelStepper motorB;

boolean currentlyRunning = true;


static String cmd_return = "";
static const String MSG_CR_LF = "\r\n";

static char inCmd[10];
static char inParam1[14];
static char inParam2[14];
static char inParam3[14];
static char inParam4[14];

static byte inNoOfParams;

char lastCommand[INLENGTH+1];
boolean commandConfirmed = false;

int rebroadcastReadyInterval = 5000;
long lastOperationTime = 0L;
long motorIdleTimeBeforePowerDown = 600000L;
boolean automaticPowerDown = true;
boolean powerIsOn = false;

long lastInteractionTime = 0L;

#ifdef PIXEL_DRAWING
static boolean lastWaveWasTop = true;

//  Drawing direction
const static byte DIR_NE = 1;
const static byte DIR_SE = 2;
const static byte DIR_SW = 3;
const static byte DIR_NW = 4;

static int globalDrawDirection = DIR_NW;

const static byte DIR_MODE_AUTO = 1;
const static byte DIR_MODE_PRESET = 2;
static byte globalDrawDirectionMode = DIR_MODE_AUTO;
#endif

#if MICROCONTROLLER == MC_MEGA
  #define READY_STR "READY_100"
#else
  #define READY_STR "READY"
#endif



#define RESEND_STR "RESEND"
#define DRAWING_STR "DRAWING"
#define OUT_CMD_SYNC_STR "SYNC,"

char MSG_E_STR[] = "MSG,E,";
char MSG_I_STR[] = "MSG,I,";
char MSG_D_STR[] = "MSG,D,";

const static char COMMA[] = ",";
const static char CMD_END[] = ",END";
const static String CMD_CHANGELENGTH = "C01";
const static String CMD_CHANGEPENWIDTH = "C02";
#ifdef PIXEL_DRAWING
const static String CMD_DRAWPIXEL = "C05";
const static String CMD_DRAWSCRIBBLEPIXEL = "C06";
const static String CMD_CHANGEDRAWINGDIRECTION = "C08";
const static String CMD_TESTPENWIDTHSQUARE = "C11";
#endif
const static String CMD_SETPOSITION = "C09";
#ifdef PENLIFT
const static String CMD_PENDOWN = "C13";
const static String CMD_PENUP = "C14";
const static String CMD_SETPENLIFTRANGE = "C45";
#endif
#ifdef VECTOR_LINES
const static String CMD_CHANGELENGTHDIRECT = "C17";
#endif
const static String CMD_SETMACHINESIZE = "C24";
const static String CMD_GETMACHINEDETAILS = "C26";
const static String CMD_RESETEEPROM = "C27";
const static String CMD_SETMACHINEMMPERREV = "C29";
const static String CMD_SETMACHINESTEPSPERREV = "C30";
const static String CMD_SETMOTORSPEED = "C31";
const static String CMD_SETMOTORACCEL = "C32";
const static String CMD_SETMACHINESTEPMULTIPLIER = "C37";


class DualPrint 
{
public:
    
#if COMM_STACK == COMM_TCP
    DualPrint() : send_serial(false), buffer_cmd(true) {}
#endif
#if COMM_STACK == COMM_SERIAL
    DualPrint() : send_serial(true), buffer_cmd(false) {}
#endif
#if COMM_STACK == COMM_TCP_SERIAL
    DualPrint() : send_serial(true), buffer_cmd(true) {}
#endif

    virtual size_t print(const String &c)                    { if (send_serial) { Serial.print(c); }     if (buffer_cmd) { cmd_return += c; }        return 1; }
    //virtual size_t print(const char[] c)                   { if (send_serial) { Serial.print(c); }     if (buffer_cmd) { cmd_return += c; }        return 1; }
    virtual size_t print(char c)                             { if (send_serial) { Serial.print(c); }     if (buffer_cmd) { cmd_return += c; }        return 1; }
    virtual size_t print(unsigned char c, int i = DEC)       { if (send_serial) { Serial.print(c); }     if (buffer_cmd) { cmd_return += c; }        return 1; }
    virtual size_t print(int c, int i = DEC)                 { if (send_serial) { Serial.print(c); }     if (buffer_cmd) { cmd_return += c; }        return 1; }
    virtual size_t print(unsigned int c, int i = DEC)        { if (send_serial) { Serial.print(c); }     if (buffer_cmd) { cmd_return += c; }        return 1; }
    virtual size_t print(long c, int i = DEC)                { if (send_serial) { Serial.print(c); }     if (buffer_cmd) { cmd_return += c; }        return 1; }
    virtual size_t print(unsigned long c, int i = DEC)       { if (send_serial) { Serial.print(c); }     if (buffer_cmd) { cmd_return += c; }        return 1; }
    virtual size_t print(double c, int = 2)                  { if (send_serial) { Serial.print(c); }     if (buffer_cmd) { cmd_return += c; }        return 1; }

    virtual size_t println(const String &c)                  { if (send_serial) { Serial.println(c); }   if (buffer_cmd) { cmd_return += c; cmd_return += MSG_CR_LF; }    return 1; }
    virtual size_t println(char c)                           { if (send_serial) { Serial.println(c); }   if (buffer_cmd) { cmd_return += c; cmd_return += MSG_CR_LF; }    return 1; }
    virtual size_t println(unsigned char c, int i = DEC)     { if (send_serial) { Serial.println(c); }   if (buffer_cmd) { cmd_return += c; cmd_return += MSG_CR_LF; }    return 1; }
    virtual size_t println(int c, int i = DEC)               { if (send_serial) { Serial.println(c); }   if (buffer_cmd) { cmd_return += c; cmd_return += MSG_CR_LF; }    return 1; }
    virtual size_t println(unsigned int c, int i = DEC)      { if (send_serial) { Serial.println(c); }   if (buffer_cmd) { cmd_return += c; cmd_return += MSG_CR_LF; }    return 1; }
    virtual size_t println(long c, int i = DEC)              { if (send_serial) { Serial.println(c); }   if (buffer_cmd) { cmd_return += c; cmd_return += MSG_CR_LF; }    return 1; }
    virtual size_t println(unsigned long c, int i = DEC)     { if (send_serial) { Serial.println(c); }   if (buffer_cmd) { cmd_return += c; cmd_return += MSG_CR_LF; }    return 1; }
    virtual size_t println(double c, int i = 2)              { if (send_serial) { Serial.println(c); }   if (buffer_cmd) { cmd_return += c; cmd_return += MSG_CR_LF; }    return 1; }
    //virtual size_t println(const Printable& c)             { if (send_serial) { Serial.println(c); }   if (buffer_cmd) { cmd_return += c; cmd_return += MSG_CR_LF; }    return 1; }
    virtual size_t println(void)                             { if (send_serial) { Serial.println(""); }  if (buffer_cmd) { cmd_return += MSG_CR_LF; }    return 1; }

    bool send_serial, buffer_cmd;
} out; 




void setup() 
{
  Serial.begin(57600);           // set up Serial library at 57600 bps

  out.println("POLARGRAPH ON!");
  out.print("Hardware: ");
  out.println(MICROCONTROLLER);
  
  #if MICROCONTROLLER == MC_MEGA
  out.println("MC_MEGA");
  #elif MICROCONTROLLER == MC_UNO
  out.println("MC_UNO");
  #elif MICROCONTROLLER == MC_ESP8266
  out.println("MC_ESP8266");
  #else
  out.println("No MC");
  #endif
  //out.println("debug1");
  configuration_motorSetup();
  //out.println("debug2");
  eeprom_loadMachineSpecFromEeprom();
  //out.println("debug3");
  configuration_setup();
  //out.println("debug4");

  motorA.setMaxSpeed(currentMaxSpeed);
  motorA.setAcceleration(currentAcceleration);  
  motorB.setMaxSpeed(currentMaxSpeed);
  motorB.setAcceleration(currentAcceleration);
  
  
  //out.println("debug5");
  
  float startLength = ((float) startLengthMM / (float) mmPerRev) * (float) motorStepsPerRev;
  motorA.setCurrentPosition(startLength);
  motorB.setCurrentPosition(startLength);
  for (int i = 0; i<INLENGTH; i++) {
    lastCommand[i] = 0;
  }    
  comms_ready();

#ifdef PENLIFT
  penlift_penUp();
#endif
  delay(500);
  
  //DuEBUG SHIT
  // pinMode(8, OUTPUT);
  // digitalWrite(8, LOW);
  configuration_motorSetup();
  impl_releaseMotors();


#if COMM_STACK == COMM_TCP
  setup_webserver();
#elif COMM_STACK == COMM_TCP_SERIAL
  setup_webserver();
#endif
}

  
#if COMM_STACK == COMM_TCP
void loop()
{
  //Webserver
  loop_webserver();
}
#endif


#if COMM_STACK == COMM_TCP_SERIAL
void loop()
{
  //Webserver
  loop_webserver();
}
#endif


#if COMM_STACK == COMM_SERIAL
void loop(){

  if (comms_waitForNextCommand(lastCommand)) 
  {
    out.println(lastCommand);
#ifdef DEBUG_COMMS    
    //out.print(F("Last comm: "));
    out.print(lastCommand);
    //out.println(F("..."));
#endif
    comms_parseAndExecuteCommand(lastCommand);
  }

  delay(100);
}
#endif


const static String CMD_TESTPENWIDTHSCRIBBLE = "C12";
const static String CMD_DRAWSAWPIXEL = "C15,";
const static String CMD_DRAWCIRCLEPIXEL = "C16";
const static String CMD_SET_ROVE_AREA = "C21";
const static String CMD_DRAWDIRECTIONTEST = "C28";
const static String CMD_MODE_STORE_COMMANDS = "C33";
const static String CMD_MODE_EXEC_FROM_STORE = "C34";
const static String CMD_MODE_LIVE = "C35";
const static String CMD_RANDOM_DRAW = "C36";
const static String CMD_START_TEXT = "C38";
const static String CMD_DRAW_SPRITE = "C39";
const static String CMD_CHANGELENGTH_RELATIVE = "C40";
const static String CMD_SWIRLING = "C41";
const static String CMD_DRAW_RANDOM_SPRITE = "C42";
const static String CMD_DRAW_NORWEGIAN = "C43";
const static String CMD_DRAW_NORWEGIAN_OUTLINE = "C44";


#if MICROCONTROLLER == MC_MEGA
/*  End stop pin definitions  */
const int ENDSTOP_X_MAX = 17;
const int ENDSTOP_X_MIN = 16;
const int ENDSTOP_Y_MAX = 15;
const int ENDSTOP_Y_MIN = 14;

long ENDSTOP_X_MIN_POSITION = 130;
long ENDSTOP_Y_MIN_POSITION = 130;

// size and location of rove area
long rove1x = 1000;
long rove1y = 1000;
long roveWidth = 5000;
long roveHeight = 8000;

boolean swirling = false;
String spritePrefix = "";
int textRowSize = 200;
int textCharSize = 180;

boolean useRoveArea = false;

int commandNo = 0;
int errorInjection = 0;

boolean storeCommands = false;
boolean drawFromStore = false;
String commandFilename = "";

// sd card stuff
const int chipSelect = 53;
boolean sdCardInit = false;

// set up variables using the SD utility library functions:
File root;
boolean cardPresent = false;
boolean cardInit = false;
boolean echoingStoredCommands = false;

// the file itself
File pbmFile;

// information we extract about the bitmap file
long pbmWidth, pbmHeight;
float pbmScaling = 1.0;
int pbmDepth, pbmImageoffset;
long pbmFileLength = 0;
float pbmAspectRatio = 1.0;

volatile int speedChangeIncrement = 100;
volatile int accelChangeIncrement = 100;
volatile float penWidthIncrement = 0.05;
volatile int moveIncrement = 400;

boolean currentlyDrawingFromFile = false;
String currentlyDrawingFilename = "";

static float translateX = 0.0;
static float translateY = 0.0;
static float scaleX = 1.0;
static float scaleY = 1.0;
static int rotateTransform = 0;

#endif
