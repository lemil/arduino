/**
*  Polargraph Server. - CORE
*  Written by Sandy Noble
*  Released under GNU License version 3.
*  http://www.polargraph.co.uk
*  https://github.com/euphy/polargraph_server_a1

Util.

This is one of the core files for the polargraph server program.  

This has all the methods that let the rest actually work, including
the geometry routines that convert from the different coordinates
systems, and do transformations.

*/
long multiplier(int in)
{
  return multiplier((long) in);
}
long multiplier(long in)
{
  return in * stepMultiplier;
}
float multiplier(float in)
{
  return in * stepMultiplier;
}
long divider(long in)
{
  return in / stepMultiplier;
}




//void changeLength(long tA, long tB)
//{
//  changeLength((float)tA, (float)tB);
//}

void changeLength(long tAl, long tBl)
{
  float tA = float(tAl);
  float tB = float(tBl);
//  out.println("changeLenth-float");
  lastOperationTime = millis();

//  impl_transform(tA,tB);
  
  float currSpeedA = motorA.speed();
  float currSpeedB = motorB.speed();
  
//  out.print("A pos: ");
//  out.print(motorA.currentPosition());
//  out.print(", A target: ");
//  out.println(tA);
//  out.print("B pos: ");
//  out.print(motorB.currentPosition());
//  out.print(", B target: ");
//  out.println(tB);
  
  
  motorA.setSpeed(0.0);
  motorB.setSpeed(0.0);
  motorA.moveTo(tA);
  motorB.moveTo(tB);
  
  
  if (!usingAcceleration)
  {
    // The moveTo() function changes the speed in order to do a proper
    // acceleration. This counteracts it. Ha.
    
    if (motorA.speed() < 0)
      currSpeedA = -currSpeedA;
    if (motorB.speed() < 0)
      currSpeedB = -currSpeedB;

//    out.print("Setting A speed ");
//    out.print(motorA.speed());
//    out.print(" back to ");
//    out.println(currSpeedA);
//    out.print("Setting B speed ");
//    out.print(motorB.speed());
//    out.print(" back to ");
//    out.println(currSpeedB);
      
    motorA.setSpeed(currSpeedA);
    motorB.setSpeed(currSpeedB);
  }
  
  
  while (motorA.distanceToGo() != 0 || motorB.distanceToGo() != 0)
  {
//    out.print("dA:");
//    out.print(motorA.distanceToGo());
//    out.print(", dB:");
//    out.println(motorB.distanceToGo());
    impl_runBackgroundProcesses();
    if (currentlyRunning)
    {
      if (usingAcceleration)
      {
        motorA.run();
        motorB.run();
      }
      else
      {
//        out.print("Run speed..");
//        out.println(motorA.speed());
        motorA.runSpeedToPosition();
        motorB.runSpeedToPosition();
      }
    }
  }
  
  reportPosition();
}

void changeLengthRelative(float tA, float tB)
{
  changeLengthRelative((long) tA, (long)tB);
}
void changeLengthRelative(long tA, long tB)
{
  lastOperationTime = millis();
  motorA.move(tA);
  motorB.move(tB);
  
  while (motorA.distanceToGo() != 0 || motorB.distanceToGo() != 0)
  {
    //impl_runBackgroundProcesses();
    if (currentlyRunning)
    {
      if (usingAcceleration)
      {
        motorA.run();
        motorB.run();
      }
      else
      {
        motorA.runSpeedToPosition();
        motorB.runSpeedToPosition();
      }
    }
  }
  
  reportPosition();
}

long getMaxLength()
{
  if (maxLength == 0)
  {
//    float length = getMachineA(pageWidth, pageHeight);
    maxLength = long(getMachineA(pageWidth, pageHeight)+0.5);
    out.print(F("maxLength: "));
    out.println(maxLength);
  }

//  Serial.println("OK1");
  return maxLength;
}


float getMachineA(float cX, float cY)
{
  float a = sqrt(sq(cX)+sq(cY));
  return a;
}
float getMachineB(float cX, float cY)
{
  float b = sqrt(sq((pageWidth)-cX)+sq(cY));
  return b;
}

void moveAxis(AccelStepper &m, int dist)
{
  m.move(dist);
  while (m.distanceToGo() != 0)
  {
    impl_runBackgroundProcesses();
    if (currentlyRunning)
      m.run();
  }
  lastOperationTime = millis();
}

void reportPosition()
{
  if (reportingPosition)
  {
    out.print(OUT_CMD_SYNC_STR);
    out.print(divider(motorA.currentPosition()));
    out.print(COMMA);
    out.print(divider(motorB.currentPosition()));
    out.println(CMD_END);

  }
}




//void engageMotors()
//{
//  impl_engageMotors();
//}
//
//void releaseMotors()
//{
//  impl_releaseMotors();
//}
//
//
float getCartesianXFP(float aPos, float bPos)
{
  float calcX = (sq((float)pageWidth) - sq((float)bPos) + sq((float)aPos)) / ((float)pageWidth*2.0);
  return calcX;  
}
float getCartesianYFP(float cX, float aPos) 
{
  float calcY = sqrt(sq(aPos)-sq(cX));
  return calcY;
}

long getCartesianX(float aPos, float bPos)
{
  long calcX = long((pow(pageWidth, 2) - pow(bPos, 2) + pow(aPos, 2)) / (pageWidth*2));
  return calcX;  
}

long getCartesianX() {
  long calcX = getCartesianX(motorA.currentPosition(), motorB.currentPosition());
  return calcX;  
}

long getCartesianY() {
  return getCartesianY(getCartesianX(), motorA.currentPosition());
}
long getCartesianY(long cX, float aPos) {
  long calcY = long(sqrt(pow(aPos,2)-pow(cX,2)));
  return calcY;
}
