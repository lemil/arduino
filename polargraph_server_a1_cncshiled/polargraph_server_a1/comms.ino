/**
*  Polargraph Server. - CORE
*  Written by Sandy Noble
*  Released under GNU License version 3.
*  http://www.polargraph.co.uk
*  https://github.com/euphy/polargraph_server_a1

Comms.

This is one of the core files for the polargraph server program.  
Comms can mean "communications" or "commands", either will do, since
it contains methods for reading commands from the serial port.

*/

boolean comms_waitForNextCommand(char *buf)
{
  // send ready
  // wait for instruction
  long idleTime = millis();
  int bufPos = 0;
  for (int i = 0; i<INLENGTH; i++) {
    buf[i] = 0;
  }  
  long lastRxTime = 0L;

  // loop while there's there isn't a terminated command.
  // (Note this might mean characters ARE arriving, but just
  //  that the command hasn't been finished yet.)
  boolean terminated = false;
  while (!terminated)
  {
#ifdef DEBUG_COMMS    
    out.print(F("."));
#endif    
    long timeSince = millis() - lastRxTime;
    
    // If the buffer is being filled, but hasn't received a new char in less than 100ms,
    // just cancel it. It's probably just junk.
    if (bufPos != 0 && timeSince > 100)
    {
#ifdef DEBUG_COMMS
      out.print(F("Timed out:"));
      out.println(timeSince);
#endif
      // Clear the buffer and reset the position if it took too long
      for (int i = 0; i<INLENGTH; i++) {
        buf[i] = 0;
      }
      bufPos = 0;
    }
    
    // idle time is mostly spent in this loop.
    impl_runBackgroundProcesses();
    timeSince = millis() - idleTime;
    if (timeSince > rebroadcastReadyInterval)
    {
      // issue a READY every 5000ms of idling
#ifdef DEBUG_COMMS      
      out.println("");
#endif
      comms_ready();
      idleTime = millis();
    }
    
    // And now read the command if one exists.
    if (Serial.available() > 0)
    {
      // Get the char
      char ch = Serial.read();
#ifdef DEBUG_COMMS
      out.print(F("ch: "));
      out.println(ch);
#endif
      
      // look at it, if it's a terminator, then lets terminate the string
      if (ch == INTERMINATOR || ch == SEMICOLON) {
        buf[bufPos] = 0; // null terminate the string
        terminated = true;
#ifdef DEBUG_COMMS
        out.println(F("Term'd"));
#endif
        for (int i = bufPos; i<INLENGTH-1; i++) {
          buf[i] = 0;
        }
        
      } else {
        // otherwise, just add it into the buffer
        buf[bufPos] = ch;
        bufPos++;
      }
#ifdef DEBUG_COMMS
      out.print(F("buf: "));
      out.println(buf);
      out.print(F("Bufpos: "));
      out.println(bufPos);
#endif
      lastRxTime = millis();
    }
  }

  idleTime = millis();
  lastOperationTime = millis();
  lastInteractionTime = lastOperationTime;
#ifdef DEBUG_COMMS
  out.print(F("xbuf: "));
  out.println(buf);
#endif
  return true;
}

void comms_parseAndExecuteCommand(char *inS)
{
#ifdef DEBUG_COMMS
  out.print("3inS: ");
  out.println(inS);
#endif

  boolean commandParsed = comms_parseCommand(inS);
  if (commandParsed)
  {
    impl_processCommand(lastCommand);
    for (int i = 0; i<INLENGTH; i++) { inS[i] = 0; }  
    commandConfirmed = false;
    comms_ready();
  }
  else
  {
    out.print(MSG_E_STR);
    out.print(F("Comm ("));
    out.print(inS);
    out.println(F(") not parsed."));
  }
  inNoOfParams = 0;
  
}


boolean comms_parseCommand(char *inS)
{
#ifdef DEBUG_COMMS
  out.print(F("1inS: "));
  out.println(inS);
#endif
  // strstr returns a pointer to the location of ",END" in the incoming string (inS).
  char* sub = strstr(inS, CMD_END);
#ifdef DEBUG_COMMS
  out.print(F("2inS: "));
  out.println(inS);
#endif
  sub[strlen(CMD_END)] = 0; // null terminate it directly after the ",END"
#ifdef DEBUG_COMMS
  out.print(F("4inS: "));
  out.println(inS);
  out.print(F("2Sub: "));
  out.println(sub);
  out.println(strcmp(sub, CMD_END));
#endif
  if (strcmp(sub, CMD_END) == 0) 
  {
    comms_extractParams(inS);
    return true;
  }
  else
    return false;
}  

void comms_extractParams(char* inS) 
{
  char in[strlen(inS)];
  strcpy(in, inS);
  char * param;
  
#ifdef DEBUG_COMMS
  out.print(F("In: "));
  out.print(in);
//  out.println("...");
#endif  
  byte paramNumber = 0;
  param = strtok(in, COMMA);
  
  inParam1[0] = 0;
  inParam2[0] = 0;
  inParam3[0] = 0;
  inParam4[0] = 0;
  
  for (byte i=0; i<6; i++) {
      if (i == 0) {
        strcpy(inCmd, param);
      }
      else {
        param = strtok(NULL, COMMA);
        if (param != NULL) {
          if (strstr(CMD_END, param) == NULL) {
            // It's not null AND it wasn't 'END' either
            paramNumber++;
          }
        }
        
        switch(i)
        {
          case 1:
            if (param != NULL) strcpy(inParam1, param);
            break;
          case 2:
            if (param != NULL) strcpy(inParam2, param);
            break;
          case 3:
            if (param != NULL) strcpy(inParam3, param);
            break;
          case 4:
            if (param != NULL) strcpy(inParam4, param);
            break;
          default:
            break;
        }
      }
#ifdef DEBUG_COMMS
      out.print(F("P: "));
      out.print(i);
      out.print(F("-"));
      out.print(paramNumber);
      out.print(F(":"));
      out.println(param);
#endif
  }

  inNoOfParams = paramNumber;

#ifdef DEBUG_COMMS
    out.print(F("Command:"));
    out.print(inCmd);
    out.print(F(", p1:"));
    out.print(inParam1);
    out.print(F(", p2:"));
    out.print(inParam2);
    out.print(F(", p3:"));
    out.print(inParam3);
    out.print(F(", p4:"));
    out.println(inParam4);
    out.print(F("Params:"));
    out.println(inNoOfParams);  
#endif
}


void comms_ready()
{
  out.println(F(READY_STR));

}
void comms_drawing()
{
  out.println(F(DRAWING_STR));
}
void comms_requestResend()
{
  out.println(F(RESEND_STR));
}
void comms_unrecognisedCommand(String &com)
{
  out.print(MSG_E_STR);
  out.print(com);
  out.println(F(" not recognised."));
}  
