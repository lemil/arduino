/**
*  Polargraph Server. - CORE
*  Written by Sandy Noble
*  Released under GNU License version 3.
*  http://www.polargraph.co.uk
*  https://github.com/euphy/polargraph_server_a1

EEPROM.

This is one of the core files for the polargraph server program.  
Has a couple of little helper methods for reading and writing
ints and floats to EEPROM using the EEPROM library.

But mostly it contains the loadMachineSpecFromEeprom method, which is
used to retrieve the machines saved values when it restarts, or 
whenever a value is written to the EEPROM.

*/

void eeprom_resetEeprom()
{

#if MICROCONTROLLER == MC_ESP8266
    EEPROM.begin(EEPROM_BEGIN);  //Initialize EEPROM
#endif   

  for (int i = 0; i <80; i++)
  {
    EEPROM.write(i, 0);
  }


  eeprom_loadMachineSpecFromEeprom();
  
#if MICROCONTROLLER == MC_ESP8266
    EEPROM.commit();
#endif
}
void eeprom_dumpEeprom()
{
  
#if MICROCONTROLLER == MC_ESP8266
    EEPROM.begin(EEPROM_BEGIN);  //Initialize EEPROM
#endif  

  for (int i = 0; i <80; i++)
  {
    out.print(i);
    out.print(". ");
    out.println( EEPROM.read(i) );
  }
}  

void eeprom_loadMachineSize()
{
  out.print(F("Current machineWidth:"));
  out.println(machineWidth);


  //EEPROM_readAnything(EEPROM_MACHINE_WIDTH, machineWidth);
  if (machineWidth < 1)
  {
    out.println("Width not found, using default");

    machineWidth = DEFAULT_MACHINE_WIDTH;
  }
  out.print(F("Loaded width:"));
  out.println(machineWidth); 

  
  //EEPROM_readAnything(EEPROM_MACHINE_HEIGHT, machineHeight);
  if (machineHeight < 1)
  {
    machineHeight = DEFAULT_MACHINE_HEIGHT;
  }
  out.print(F("Loaded height:"));
  out.println(machineHeight);

}

void eeprom_loadSpoolSpec()
{
  
  if (mmPerRev < 1)
  {
    mmPerRev = DEFAULT_MM_PER_REV;
  }
  out.print(F("Loaded mmPerRev:"));
  out.println(mmPerRev);

  //EEPROM_readAnything(EEPROM_MACHINE_STEPS_PER_REV, motorStepsPerRev);
  if (motorStepsPerRev < 1)
  {
    motorStepsPerRev = DEFAULT_STEPS_PER_REV;
  }
  out.print(F("Loaded steps per rev:"));
  out.println(motorStepsPerRev);
}  

void eeprom_loadPenLiftRange()
{
  //EEPROM_readAnything(EEPROM_PENLIFT_DOWN, downPosition);
  if ((downPosition < 0) || (downPosition > 360))
  {
    downPosition = DEFAULT_DOWN_POSITION;
  }
  out.print(F("Loaded down pos:"));
  out.println(downPosition);

  //EEPROM_readAnything(EEPROM_PENLIFT_UP, upPosition);
  if ((upPosition < 0) || (upPosition > 360))
  {
    upPosition = DEFAULT_UP_POSITION;
  }
  out.print(F("Loaded up pos:"));
  out.println(upPosition);
}  

void eeprom_loadStepMultiplier()
{
  //EEPROM_readAnything(EEPROM_MACHINE_STEP_MULTIPLIER, stepMultiplier);
  if (stepMultiplier < 1)
  {
    stepMultiplier = DEFAULT_STEP_MULTIPLIER;
  }
  out.print(F("Loaded step multiplier:"));
  out.println(stepMultiplier);  
}  

void eeprom_loadSpeed()
{
  // load speed, acceleration
  //EEPROM_readAnything(EEPROM_MACHINE_MOTOR_SPEED, currentMaxSpeed);
  
  // not sure why this requires a cast to int for the comparision, but a 
  // if (currentMaxSpeed < 1.0) wasn't catching cases where 
  // currentMaxSpeed == 0.00, ODD.
  if (int(currentMaxSpeed) < 1) {
    currentMaxSpeed = 800.0;
  }
    
  //EEPROM_readAnything(EEPROM_MACHINE_MOTOR_ACCEL, currentAcceleration);
  if (int(currentAcceleration) < 1) {
    currentAcceleration = 800.0;
  }
}

void eeprom_loadMachineSpecFromEeprom()
{
  impl_loadMachineSpecFromEeprom();

  eeprom_loadMachineSize();
  eeprom_loadSpoolSpec();
  eeprom_loadStepMultiplier();
  eeprom_loadPenLiftRange();
  eeprom_loadSpeed();

  // load penwidth
  //EEPROM_readAnything(EEPROM_MACHINE_PEN_WIDTH, penWidth);
  if (penWidth < 0.0001)
    penWidth = 0.8;

  mmPerStep = mmPerRev / multiplier(motorStepsPerRev);
  stepsPerMM = multiplier(motorStepsPerRev) / mmPerRev;

  out.print(F("Recalc mmPerStep ("));
  out.print(mmPerStep);
  out.print(F("), stepsPerMM ("));
  out.print(stepsPerMM);
  out.println(F(")"));

  pageWidth = machineWidth * stepsPerMM;
  out.print(F("Recalc pageWidth in steps ("));
  out.print(pageWidth);
  out.println(F(")"));
  pageHeight = machineHeight * stepsPerMM;

  out.print(F("Recalc "));
  out.print(F("pageHeight in steps ("));
  out.print(pageHeight);
  out.print(F(")"));
  out.println();

  maxLength = 0;
}
