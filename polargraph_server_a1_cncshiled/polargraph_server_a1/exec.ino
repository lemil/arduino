/**
*  Polargraph Server. - CORE
*  Written by Sandy Noble
*  Released under GNU License version 3.
*  http://www.polargraph.co.uk
*  https://github.com/euphy/polargraph_server_a1

Exec.

This is one of the core files for the polargraph server program.  
Purposes are getting a little more blurred here.  This file contains
the basic decision tree that branches based on command.

It has a set of the most general-purpose drawing commands, but only
methods that are directly called - none of the geometry or conversion
routines are here.

*/
/**  This method looks only for the basic command set
*/
boolean exec_executeBasicCommand(String &com)
{
  boolean executed = true;
  if (com.startsWith(CMD_CHANGELENGTH))
    exec_changeLength();
#ifdef VECTOR_LINES
  else if (com.startsWith(CMD_CHANGELENGTHDIRECT))
    exec_changeLengthDirect();
#endif
  else if (com.startsWith(CMD_CHANGEPENWIDTH))
    exec_changePenWidth();
  else if (com.startsWith(CMD_SETMOTORSPEED))
    exec_setMotorSpeed();
  else if (com.startsWith(CMD_SETMOTORACCEL))
    exec_setMotorAcceleration();
#ifdef PIXEL_DRAWING
  else if (com.startsWith(CMD_DRAWPIXEL))
    pixel_drawSquarePixel();
  else if (com.startsWith(CMD_DRAWSCRIBBLEPIXEL))
    pixel_drawScribblePixel();
  else if (com.startsWith(CMD_CHANGEDRAWINGDIRECTION))
    pixel_changeDrawingDirection();
  else if (com.startsWith(CMD_TESTPENWIDTHSQUARE))
    pixel_testPenWidth();
#endif
  else if (com.startsWith(CMD_SETPOSITION))
    exec_setPosition();
#ifdef PENLIFT
  else if (com.startsWith(CMD_PENDOWN))
    penlift_penDown();
  else if (com.startsWith(CMD_PENUP))
    penlift_penUp();
  else if (com.startsWith(CMD_SETPENLIFTRANGE))
    exec_setPenLiftRange();
#endif
  else if (com.startsWith(CMD_SETMACHINESIZE))
    exec_setMachineSizeFromCommand();
  else if (com.startsWith(CMD_SETMACHINEMMPERREV))
    exec_setMachineMmPerRevFromCommand();
  else if (com.startsWith(CMD_SETMACHINESTEPSPERREV))
    exec_setMachineStepsPerRevFromCommand();
  else if (com.startsWith(CMD_SETMACHINESTEPMULTIPLIER))
    exec_setMachineStepMultiplierFromCommand();
  else if (com.startsWith(CMD_GETMACHINEDETAILS))
    exec_reportMachineSpec();
  else if (com.startsWith(CMD_RESETEEPROM))
    eeprom_resetEeprom();
  else
    executed = false;

  return executed;
}
void exec_reportMachineSpec()
{
  eeprom_dumpEeprom();
  
  out.print(F("PGSIZE,"));
  out.print(machineWidth);
  out.print(COMMA);
  out.print(machineHeight);
  out.println(CMD_END);

  out.print(F("PGMMPERREV,"));
  out.print(mmPerRev);
  out.println(CMD_END);

  out.print(F("PGSTEPSPERREV,"));
  out.print(motorStepsPerRev);
  out.println(CMD_END);
  
  out.print(F("PGSTEPMULTIPLIER,"));
  out.print(stepMultiplier);
  out.println(CMD_END);

  out.print(F("PGLIFT,"));
  out.print(downPosition);
  out.print(COMMA);
  out.print(upPosition);
  out.println(CMD_END);

  out.print(F("PGSPEED,"));
  out.print(currentMaxSpeed);
  out.print(COMMA);
  out.print(currentAcceleration);
  out.println(CMD_END);



}

void exec_setMachineSizeFromCommand()
{
  long pwidth = atoi(inParam1);
  long pheight = atoi(inParam2);


  // load to get current settings
  long c_val_w = -1;
  EEPROM_readAnything(EEPROM_MACHINE_WIDTH, c_val_w); 
  if (c_val_w != pwidth){
    out.print("Current width=");
    out.println(c_val_w);
    if (pwidth > 1)
    {
        EEPROM_writeAnything(EEPROM_MACHINE_WIDTH, pwidth);
        out.print("Set Machine Size to width=");
        out.println(pwidth);
        out.print("Saved width in EEPROM=");
        EEPROM_readAnything(EEPROM_MACHINE_WIDTH, c_val_w); 
        machineWidth = pwidth;
        out.println(c_val_w);
     
    }
  }

  long c_val_h = -1;
  EEPROM_readAnything(EEPROM_MACHINE_HEIGHT, c_val_h);
  if (c_val_h != pheight){
    out.print("Current height=");
    out.println(c_val_h);
    if (pheight > 1)
    {
      EEPROM_writeAnything(EEPROM_MACHINE_HEIGHT, pheight);
      out.print("Set Machine Size to height=");
      out.println(pheight);
      out.print("Saved height in EEPROM=");
      EEPROM_readAnything(EEPROM_MACHINE_HEIGHT, c_val_h); 
      out.println(c_val_h);
      machineHeight = pheight;
    }
  }
  
  
  // reload 
  eeprom_loadMachineSize();
}

void exec_setMachineMmPerRevFromCommand()
{
  EEPROM_writeAnything(EEPROM_MACHINE_MM_PER_REV, (float)atof(inParam1));
  eeprom_loadMachineSpecFromEeprom();
}
void exec_setMachineStepsPerRevFromCommand()
{
  EEPROM_writeAnything(EEPROM_MACHINE_STEPS_PER_REV, (int)atoi(inParam1));
  eeprom_loadMachineSpecFromEeprom();
}
void exec_setMachineStepMultiplierFromCommand()
{
  EEPROM_writeAnything(EEPROM_MACHINE_STEP_MULTIPLIER, (int)atoi(inParam1));
  eeprom_loadMachineSpecFromEeprom();
}

void exec_setPenLiftRange()
{
  int down = atoi(inParam1);
  int up = atoi(inParam2);

#ifdef DEBUG_PENLIFT
  out.print(F("Down: "));
  out.println(down);
  out.print(F("Up: "));
  out.println(up);
#endif 
  if (inNoOfParams == 3) 
  {
    // 3 params (C45,<downpos>,<uppos>,1,END) means save values to EEPROM
    EEPROM_writeAnything(EEPROM_PENLIFT_DOWN, down);
    EEPROM_writeAnything(EEPROM_PENLIFT_UP, up);
    eeprom_loadPenLiftRange();
  }
  else if (inNoOfParams == 2)
  {
    // 2 params (C45,<downpos>,<uppos>,END) means just do a range test
    penlift_movePen(up, down, penLiftSpeed);
    delay(200);
    penlift_movePen(down, up, penLiftSpeed);
    delay(200);
    penlift_movePen(up, down, penLiftSpeed);
    delay(200);
    penlift_movePen(down, up, penLiftSpeed);
    delay(200);
  }
}

/* Single parameter to set max speed, add a second parameter of "1" to make it persist.
*/
void exec_setMotorSpeed()
{
  exec_setMotorSpeed((float)atof(inParam1));
  if (inNoOfParams == 2 && atoi(inParam2) == 1)
    EEPROM_writeAnything(EEPROM_MACHINE_MOTOR_SPEED, currentMaxSpeed);
}

void exec_setMotorSpeed(float speed)
{
  currentMaxSpeed = speed;
  motorA.setMaxSpeed(currentMaxSpeed);
  motorB.setMaxSpeed(currentMaxSpeed);
  out.print(F("New max speed: "));
  out.println(currentMaxSpeed);

}

/* Single parameter to set acceleration, add a second parameter of "1" to make it persist.
*/
void exec_setMotorAcceleration()
{
  exec_setMotorAcceleration((float)atof(inParam1));
  if (inNoOfParams == 2 && atoi(inParam2) == 1)
    EEPROM_writeAnything(EEPROM_MACHINE_MOTOR_ACCEL, currentAcceleration);
}
void exec_setMotorAcceleration(float accel)
{
  currentAcceleration = accel;
  motorA.setAcceleration(currentAcceleration);
  motorB.setAcceleration(currentAcceleration);
  out.print(F("New accel: "));
  out.println(currentAcceleration);
}

void exec_changePenWidth()
{
  penWidth = atof(inParam1);
  out.print(F("Changed Pen width to "));
  out.print(penWidth);
  out.println(F("mm"));
  
}

void exec_setPosition()
{
  long targetA = multiplier(atol(inParam1));
  long targetB = multiplier(atol(inParam2));
  motorA.setCurrentPosition(targetA);
  motorB.setCurrentPosition(targetB);
  
  impl_engageMotors();
  
  reportPosition();
}

void exec_changeLengthRelative()
{
  long lenA = multiplier(atol(inParam1));
  long lenB = multiplier(atol(inParam2));
  
  changeLengthRelative(lenA, lenB);
}  

void exec_changeLength()
{
  float lenA = multiplier((float)atof(inParam1));
  float lenB = multiplier((float)atof(inParam2));
  
  changeLength(lenA, lenB);
}

#ifdef VECTOR_LINES
void exec_changeLengthDirect()
{
  float endA = multiplier((float)atof(inParam1));
  float endB = multiplier((float)atof(inParam2));
  int maxSegmentLength = atoi(inParam3);

  float startA = motorA.currentPosition();
  float startB = motorB.currentPosition();

  if (endA < 20 || endB < 20 || endA > getMaxLength() || endB > getMaxLength())
  {
    out.print(MSG_E_STR);
    out.println(F("This point falls outside the area of this machine. Skipping it."));
    //

  }
  else
  {
//    Serial.println("step exec_drawBetweenPoints");
    exec_drawBetweenPoints(startA, startB, endA, endB, maxSegmentLength);
//    Serial.println("end exec_drawBetweenPoints");
  }
}  

/**
This moves the gondola in a straight line between p1 and p2.  Both input coordinates are in 
the native coordinates system.  

The fidelity of the line is controlled by maxLength - this is the longest size a line segment is 
allowed to be.  1 is finest, slowest.  Use higher values for faster, wobblier.
*/
void exec_drawBetweenPoints(float p1a, float p1b, float p2a, float p2b, int maxSegmentLength)
{
  // ok, we're going to plot some dots between p1 and p2.  Using maths. I know! Brave new world etc.
  
  // First, convert these values to cartesian coordinates
  // We're going to figure out how many segments the line
  // needs chopping into.
  float c1x = getCartesianXFP(p1a, p1b);
  float c1y = getCartesianYFP(c1x, p1a);
  
  float c2x = getCartesianXFP(p2a, p2b);
  float c2y = getCartesianYFP(c2x, p2a);
  
  // test to see if it's on the page
  // AND ALSO TO see if the current position is on the page.
  // Remember, the native system can easily specify points that can't exist,
  // particularly up at the top.
  if (c2x > 20 
    && c2x<pageWidth-20 
    && c2y > 20 
    && c2y <pageHeight-20
    && c1x > 20 
    && c1x<pageWidth-20 
    && c1y > 20 
    && c1y <pageHeight-20 
    )
    {
    reportingPosition = false;
    float deltaX = c2x-c1x;    // distance each must move (signed)
    float deltaY = c2y-c1y;
//    float totalDistance = sqrt(sq(deltaX) + sq(deltaY));


//    Serial.println("1111");
             
    long linesegs = 1;            // assume at least 1 line segment will get us there.
    if (abs(deltaX) > abs(deltaY))
    {
      // slope <=1 case    
      while ((abs(deltaX)/linesegs) > maxSegmentLength)
      {
        linesegs++;
      }
    }
    else
    {
      while ((abs(deltaY)/linesegs) > maxSegmentLength)
      {
        linesegs++;
      }
    }
    
    // reduce delta to one line segments' worth.
    deltaX = deltaX/linesegs;
    deltaY = deltaY/linesegs;
  
    // render the line in N shorter segments
    long runSpeed = 0;
    usingAcceleration = false;
    while (linesegs > 0)
    {
//      out.print("Line segment: " );
//      out.println(linesegs);
    // compute next new location
      c1x = c1x + deltaX;
      c1y = c1y + deltaY;
      
//      Serial.println(" compute next new location");
        
      // convert back to machine space
      float pA = getMachineA(c1x, c1y);
      float pB = getMachineB(c1x, c1y);

//      Serial.println(" convert back to machine space");
    
      // do the move
      runSpeed = desiredSpeed(linesegs, runSpeed, currentAcceleration*4);

//      out.print("Setting speed:");
//      out.println(runSpeed);

//      Serial.println("change length");
      
      motorA.setSpeed(runSpeed);
      motorB.setSpeed(runSpeed);
      changeLength(pA, pB);
  
      // one line less to do!
      linesegs--;
    }
    // reset back to "normal" operation
    reportingPosition = true;
    usingAcceleration = true;
    reportPosition();
//    Serial.println("end function exec_drawBetweenPoints");
  }
  else
  {
    out.print(MSG_E_STR);
    out.println(F("Line is not on the page. Skipping it."));
    reportPosition();
  }
//  outputAvailableMemory();
}

// Work out and return a new speed.
// Subclasses can override if they want
// Implement acceleration, deceleration and max speed
// Negative speed is anticlockwise
// This is called:
//  after each step
//  after user changes:
//   maxSpeed
//   acceleration
//   target position (relative or absolute)
float desiredSpeed(long distanceTo, float currentSpeed, float acceleration)
{
    float requiredSpeed;

    if (distanceTo == 0)
	return 0.0f; // We're there

    // sqrSpeed is the signed square of currentSpeed.
    float sqrSpeed = sq(currentSpeed);
    if (currentSpeed < 0.0)
      sqrSpeed = -sqrSpeed;
      
    float twoa = 2.0f * acceleration; // 2ag
    
    // if v^^2/2as is the the left of target, we will arrive at 0 speed too far -ve, need to accelerate clockwise
    if ((sqrSpeed / twoa) < distanceTo)
    {
	// Accelerate clockwise
	// Need to accelerate in clockwise direction
	if (currentSpeed == 0.0f)
	    requiredSpeed = sqrt(twoa);
	else
	    requiredSpeed = currentSpeed + fabs(acceleration / currentSpeed);

	if (requiredSpeed > currentMaxSpeed)
	    requiredSpeed = currentMaxSpeed;
    }
    else
    {
	// Decelerate clockwise, accelerate anticlockwise
	// Need to accelerate in clockwise direction
	if (currentSpeed == 0.0f)
	    requiredSpeed = -sqrt(twoa);
	else
	    requiredSpeed = currentSpeed - fabs(acceleration / currentSpeed);
	if (requiredSpeed < -currentMaxSpeed)
	    requiredSpeed = -currentMaxSpeed;
    }
    
    //out.println(requiredSpeed);
    return requiredSpeed;
}
#endif
