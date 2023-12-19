#include <AccelStepper.h>

//Eje X
#define MOTOR_A_ENABLE_PIN_X 12   //Arduino D8  / Wemos D6
#define MOTOR_A_STEP_PIN_X 16     //Arduino D2  / Wemos D0
#define MOTOR_A_DIR_PIN_X 0       //Arduino D5  / Wemos D3

//Eje Z
#define MOTOR_B_ENABLE_PIN_Z 12   //Arduino D8  / Wemos D6
#define MOTOR_B_STEP_PIN_Z 4      //Arduino D3  / Wemos D2
#define MOTOR_B_DIR_PIN_Z 14      //Arduino D6  / Wemos D5

//AccelStepper Xaxis(1, 2, 5); // pin 2 = step, pin 5 = direction
AccelStepper Xaxis(1, MOTOR_A_STEP_PIN_X, MOTOR_A_DIR_PIN_X); // pin 2 = step, pin 5 = direction
AccelStepper Zaxis(1, MOTOR_B_STEP_PIN_Z, MOTOR_B_DIR_PIN_Z); // pin 2 = step, pin 5 = direction


//const byte enablePin = 8;
const byte enablePinX = MOTOR_A_ENABLE_PIN_X;
const byte enablePinZ = MOTOR_B_ENABLE_PIN_Z;

void setup()
{
   pinMode(enablePinX, OUTPUT);
   digitalWrite(enablePinX, LOW);

   Xaxis.setMaxSpeed(12800);
   Xaxis.setSpeed(1000); // had to slow for my motor

   pinMode(enablePinZ, OUTPUT);
   digitalWrite(enablePinZ, LOW);
   
   Zaxis.setMaxSpeed(12800);
   Zaxis.setSpeed(1000); // had to slow for my motor
}

void loop()
{
   // Xaxis.runSpeed();
   Zaxis.runSpeed();
}
