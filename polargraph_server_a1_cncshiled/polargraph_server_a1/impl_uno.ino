/**
*  Polargraph Server. - IMPLEMENTATION
*  Written by Sandy Noble
*  Released under GNU License version 3.
*  http://www.polargraph.co.uk
*  https://github.com/euphy/polargraph_server_a1

Implementation of executeCommand for UNO-sized boards 
without "store" features. Doesn't actually do anything except
pass the command to the real executeCommand.  */
String impl_processCommand(String com)
{
  String retval = "";
#if MICROCONTROLLER == MC_UNO
  retval = impl_executeCommand(com);
#endif

#if MICROCONTROLLER == MC_ESP8266
  retval = impl_executeCommand(com);
#endif

#if MICROCONTROLLER == MC_MEGA
  // check for change mode commands
  if (com.startsWith(CMD_MODE_STORE_COMMANDS)
  || com.startsWith(CMD_MODE_LIVE))
  {
    out.println("Changing mode.");
    retval = impl_executeCommand(com);
  }
  // else execute / store the command
  else if (storeCommands)
  {
    out.print(F("Storing command:"));
    out.println(com);
    sd_storeCommand(com);
  }
  else
  {
    retval = impl_executeCommand(com);
  }
#endif
  return retval;
}

String impl_executeCommand(String &com)
{
  String retval = "OK";

  boolean exec_result = exec_executeBasicCommand(com);
  
  if (exec_result)
  {
    // that's nice, it worked
#ifdef DEBUG
    out.println("Basic Command executed.");
#endif
  }
#if MICROCONTROLLER == MC_MEGA
  else if (com.startsWith(CMD_DRAWCIRCLEPIXEL))
    curves_pixel_drawCircularPixel();
//  else if (com.startsWith(CMD_TESTPATTERN))
//    testPattern();
//  else if (com.startsWith(CMD_TESTPENWIDTHSCRIBBLE))
//    impl_pixel_testPenWidthScribble();
//  else if (com.startsWith(CMD_DRAWSAWPIXEL))
//    impl_pixel_drawSawtoothPixel();
//  else if (com.startsWith(CMD_DRAWDIRECTIONTEST))
//    impl_exec_drawTestDirectionSquare();
  else if (com.startsWith(CMD_MODE_STORE_COMMANDS))
    impl_exec_changeToStoreCommandMode();
  else if (com.startsWith(CMD_MODE_LIVE))
    impl_exec_changeToLiveCommandMode();
  else if (com.startsWith(CMD_MODE_EXEC_FROM_STORE))
    impl_exec_execFromStore();
//  else if (com.startsWith(CMD_RANDOM_DRAW))
//    drawRandom();
  else if (com.startsWith(CMD_SET_ROVE_AREA))
    rove_setRoveArea();
  else if (com.startsWith(CMD_START_TEXT))
    rove_startText();
  else if (com.startsWith(CMD_DRAW_SPRITE))
    sprite_drawSprite();
  else if (com.startsWith(CMD_DRAW_RANDOM_SPRITE))
    sprite_drawRandomPositionedSprite();
  else if (com.startsWith(CMD_CHANGELENGTH_RELATIVE))
    exec_changeLength();
//  else if (com.startsWith(CMD_SWIRLING))
//    rove_controlSwirling();
  else if (com.startsWith(CMD_DRAW_NORWEGIAN))
    rove_drawNorwegianFromFile();
  else if (com.startsWith(CMD_DRAW_NORWEGIAN_OUTLINE))
    rove_drawRoveAreaFittedToImage();
#endif
  else
  {
    retval = com;
    comms_unrecognisedCommand(com);
    comms_ready();
  }
  return retval;
}

void impl_runBackgroundProcesses()
{
  long motorCutoffTime = millis() - lastOperationTime;
  if ((automaticPowerDown) && (powerIsOn) && (motorCutoffTime > motorIdleTimeBeforePowerDown))
  {
    //out.print(MSG_I_STR);
    //out.println(F("Powering down."));
    impl_releaseMotors();
  }
}

void impl_loadMachineSpecFromEeprom()
{}

#if MICROCONTROLLER == MC_MEGA
void impl_exec_execFromStore()
{
  String fileToExec = inParam1;
  if (fileToExec != "")
  {
    currentlyDrawingFromFile = true;
    out.print("Filename to read from: ");
    out.println(fileToExec);
    commandFilename = fileToExec;
    impl_exec_execFromStore(commandFilename);
    currentlyDrawingFromFile = true;
  }
  else
  {
    out.println("No filename supplied to read from.");
  }
  
}

void impl_exec_execFromStore(String inFilename)
{
//  if (inFilename != "")
//  {
//    String noBlanks = "";
//    // remove blanks
//    for (int i = 0; i<inFilename.length(); i++)
//    {
//      if (inFilename[i] != ' ')
//        noBlanks = noBlanks + inFilename[i];
//    }
//    
//    char filename[noBlanks.length()+1];
//    noBlanks.toCharArray(filename, noBlanks.length()+1);
//#ifdef DEBUG_SD    
//    out.print("Array to read from: ");
//    out.println(filename);
//#endif
//    File readFile = SD.open(filename, FILE_READ);
//    if (readFile)
//    {
//      out.print("Opened file:");
//      out.println(noBlanks);
//      String command = "";
//      while (readFile.available() && currentlyDrawingFromFile)
//      {
//#ifdef DEBUG_SD        
//        out.println("Reading...");
//        // poll for input
//#endif
//        char ch = readFile.read();
//#ifdef DEBUG_SD        
//        out.print(".");
//        out.print(ch);
//        out.print("-");
//#endif
//        if (ch == 13 || ch == 10)
//        {
//#ifdef DEBUG_SD        
//          out.println("New line");
//#endif
//          // execute the line
//          command.trim();
//          boolean commandParsed = comms_parseCommand(command);
//          if (commandParsed)
//          {
//#ifdef DEBUG_SD        
//            out.println("Stored command parsed.");
//#endif
//            out.print(F("Executing command:"));
//            out.println(command);
//            if (echoingStoredCommands) lcd_echoLastCommandToDisplay(command, inFilename+": ");
//            impl_executeCommand(command);
//          }
//#ifdef DEBUG_SD        
//          else out.println("Stored command WAS NOT parsed.");
//#endif            
//          command = "";
//          lcd_checkForInput();
//        }
//        else
//          command += ch;
//
//#ifdef DEBUG_SD        
//        out.print("Command building:");
//        out.println(command);
//#endif
//      }
//      out.println("Finished with the file.");
//      currentlyDrawingFromFile = false;
//      readFile.close();
//    }
//    else
//    {
//      out.println("Couldn't find that file, btw.");
//      currentlyDrawingFromFile = false;
//    }
//  }
//  else
//  {
//    out.println("No filename supplied to read from.");
//    currentlyDrawingFromFile = false;
//  }
//  
}

void impl_exec_changeToStoreCommandMode()
{
  String newfilename = inParam1;
  String newFile = inParam2;
  if (newfilename != "")
  {
    out.print("Filename for command store: ");
    out.println(newfilename);
    storeCommands = true;
    commandFilename = newfilename;
    if (newFile.equals("R"))
    {
      // delete file if it exists
      char filename[newfilename.length()+1];
      newfilename.toCharArray(filename, newfilename.length()+1);
      
      if (SD.exists(filename))
      {
        // file exists
        out.println(F("File already exists."));
        boolean removed = SD.remove(filename);
        if (removed)
          out.println(F("File removed."));
        
      }
    }
  }
  else
  {
    out.println("No filename supplied to write to.");
  }
}

void impl_exec_changeToLiveCommandMode()
{
  out.println(F("Changing back to live mode."));
  storeCommands = false;
}
#endif

void impl_engageMotors()
{
  motorA.enableOutputs();
  motorB.enableOutputs();
  powerIsOn = true;
  motorA.runToNewPosition(motorA.currentPosition()+4);
  motorB.runToNewPosition(motorB.currentPosition()+4);
  motorA.runToNewPosition(motorA.currentPosition()-4);
  motorB.runToNewPosition(motorB.currentPosition()-4);
}

void impl_releaseMotors()
{
  motorA.disableOutputs();
  motorB.disableOutputs();  
#ifdef ADAFRUIT_MOTORSHIELD_V2
  afMotorA->release();
  afMotorB->release();
#endif
#ifdef PENLIFT 
  penlift_penUp();
#endif
  powerIsOn = false;  
}
