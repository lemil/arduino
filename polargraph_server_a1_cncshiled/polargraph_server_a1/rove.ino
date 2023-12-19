/**
*  Polargraph Server for ATMEGA1280+ 
*  Written by Sandy Noble
*  Released under GNU License version 3.
*  http://www.polargraph.co.uk
*  https://github.com/euphy/polargraph_server_polarshield

Specific features for Polarshield / arduino mega.
Rove.

Commands that will set and modify the rove area, and the features that use
the rove area heavily.

*/

#if MICROCONTROLLER == MC_MEGA

void rove_setRoveArea()
{
  rove1x = stepsPerMM * atol(inParam1);
  rove1y = stepsPerMM * atol(inParam2);
  roveWidth = stepsPerMM * atol(inParam3);
  roveHeight = stepsPerMM * atol(inParam4);
  
  if (rove1x > pageWidth)
    rove1x = pageWidth / 2;
  else if (rove1x < 1)
    rove1x = 1;
    
  if (rove1y > pageHeight)
    rove1y = pageHeight / 2;
  else if (rove1y < 1)
    rove1y = 1;
  
  if (roveWidth+rove1x > pageWidth)
    roveWidth = pageWidth - rove1x;
    
  if (roveHeight+rove1y > pageHeight)
    roveHeight = pageHeight - rove1y;
  
  useRoveArea = true;

  out.println("Set rove area (steps):");
  out.print("X:");
  out.print(rove1x);
  out.print(",Y:");
  out.print(rove1y);
  out.print(",width:");
  out.print(roveWidth);
  out.print(",height:");
  out.println(roveHeight);

  out.println("Set rove area (mm):");
  out.print("X:");
  out.print(rove1x * mmPerStep);
  out.print("mm, Y:");
  out.print(rove1y * mmPerStep);
  out.print("mm, width:");
  out.print(roveWidth * mmPerStep);
  out.print("mm, height:");
  out.print(roveHeight * mmPerStep);
  out.println("mm.");

}
void rove_startText()
{
  if (useRoveArea)
  {
    long tA = multiplier(atol(inParam1));
    long tB = multiplier(atol(inParam2));
    inNoOfParams = 0;
    
    if (rove_inRoveArea(tA, tB))
    {
      out.println("Target position is in rove area.");
      penlift_penUp();
      changeLength(tA, tB);
      textRowSize = multiplier(atoi(inParam3));
      textCharSize = textRowSize * 0.8;
      globalDrawDirection = atoi(inParam4);
      out.println("Text started.");
    }
    else
    {
      out.print("Target position (");
      out.print(tA);
      out.print(",");
      out.print(tB);
      out.println(") not in rove area.");
    }
  }
  else
  {
    out.println("Rove area must be defined to start text.");
  }
}

boolean rove_inRoveArea(float a, float b)
{
  // work out cartesian position of pen
  float cX = getCartesianXFP(a, b);
  float cY = getCartesianYFP(cX, a);
  
//  out.print("Input cX: ");
//  out.println(cX);
//  out.print("Input cY: ");
//  out.println(cY);
//  
//  out.print("Rove origin: ");
//  out.print(rove1x);
//  out.print(", ");
//  out.println(rove1y);
//
//  out.print("Rove size: ");
//  out.print(roveWidth);
//  out.print(", ");
//  out.println(roveHeight);
  
  if (cX < rove1x || cX > rove1x+roveWidth || cY < rove1y || cY > rove1y+roveHeight)
    return false;
  else
    return true;
}


/**
Method that works out where the next line should start, based on pen position, line
width and rove area
*/
boolean rove_moveToBeginningOfNextTextLine()
{
  out.println("Move to beginning of next line.");
  out.print("Global draw direction is ");
  out.println(globalDrawDirection);
  long xIntersection;
  long yIntersection;
  boolean result = false;


  
  if (globalDrawDirection == DIR_SE) // 2
  {
    long nextLine = motorB.currentPosition() + textRowSize;
    out.print("Next line:");
    out.println(nextLine);

    // greater than the far corner or less than the near corner
    if (sq(nextLine) > sq(rove1y+roveHeight) + sq(pageWidth-rove1x)
      || sq(nextLine) < sq(rove1y) + sq(pageWidth-(rove1x+roveWidth)))
    {
      out.println("No space for lines!");
      // no lines left!
    }
    else if (sq(nextLine) <= sq(rove1y) + sq(pageWidth-rove1x))
    {
      out.println("On the top edge.");
      // measure on the top edge of the rove area
      xIntersection = pageWidth-sqrt(sq(nextLine) - sq(rove1y));
      yIntersection = rove1y;

      out.print("nextline:");
      out.print(nextLine * mmPerStep);
      out.print(",rove1x:");
      out.print(rove1x * mmPerStep);
      out.print(",rove1y:");
      out.println(rove1y * mmPerStep);

      result = true;
    }
    else
    {
      out.println("On the left edge.");
      // measure on the left edge of the rove area
      xIntersection = rove1x;
      yIntersection = sqrt(sq(nextLine) - sq(pageWidth - rove1x));
      result = true;
    }
  }
  else if (globalDrawDirection == DIR_NW) // 4
  {
  }
  else if (globalDrawDirection == DIR_SW) //3
  {
  }
  else //(drawDirection == DIR_NE) // default //1
  {
  }

  if (result)
  {
    long pA = getMachineA(xIntersection, yIntersection);
    long pB = getMachineB(xIntersection, yIntersection);
    changeLength(pA, pB);
  }
  
  return result;
}

/**
*  This is a good one - hoping to draw something like the
*  Norwegian Creations machine.  This uses a very short wavelength
*  and a relatively wide amplitude.  Using a wavelength this short
*  isn't practical for interactive use (too many commands)
*  so this first attempt will do it on-board.  In addition this 
*  should cut out an awful lot of the complexity involved in 
*  creating lists of commands, but will probably result in some
*  fairly dirty code.  Apologies in advance.
*/
void rove_drawNorwegianFromFile()
{
  if (useRoveArea)
  {
    // get parameters
    String filename = inParam1;
    int maxAmplitude = multiplier(atoi(inParam2));
    int wavelength = multiplier(atoi(inParam3));
    inNoOfParams = 0;
    
    // Look up file and open it
    if (!sd_openPbm(filename))
    {
      out.print("Couldn't open that file - ");
      out.println(filename);
      return;
    }
    else
    {
      out.print("image size "); 
      out.print(pbmWidth, DEC);
      out.print(", ");
      out.println(pbmHeight, DEC);
      out.print("(roveWidth:");
      out.print(roveWidth);
      out.println(")");
      pbmScaling = float(roveWidth) / float(pbmWidth);
      out.print("Scaling factor:");
      out.println(pbmScaling);
      out.print("Rove width:");
      out.println(roveWidth);
      out.print("Image offset:");
      out.println(pbmImageoffset);
    }
    
    // Pen up and move to start corner (top-right)
    penlift_penUp();
    // Move to top of first row:
    // x2 - amplitude

    // set roveHeight so that it is the same shape as the image.  
    roveHeight = roveWidth * pbmAspectRatio;
    long rove2x = rove1x + roveWidth;
    long rove2y = rove1y + roveHeight;
    
    // work out the distance from motor B to the closest corner of the rove area
    float row = getMachineB(rove2x,rove1y);
    
    // so the first row will be that value plus half of maxAmplitude
    row += (maxAmplitude / 2);
    
    changeLength(getMachineA(rove2x, rove1y), row);
    penlift_penDown();
    
    // and figure out where the arc with this radius intersects the top edge
    long xIntersection;
    long yIntersection;
    boolean finished = false;
  
    float tA = motorA.currentPosition();
    float tB = motorB.currentPosition();
    
  //  tA = getMachineA(rove2x, rove2y);
  //  tB = row;
    
    int pixels = 0;
    
    while (!finished)
    {
      if (!rove_inRoveArea(tA, tB))
      {
        out.println("Outside rove area. Making new line.");
        penlift_penUp();
        // increment row
        row += maxAmplitude;
        tB = row;
        
        // greater than the far corner or less than the near corner
        if (sq(row) > sq(rove2y) + sq(pageWidth-rove1x)
          || sq(row) < sq(rove1y) + sq(pageWidth-(rove2x)))
        {
          out.println("No space for rows!");
          // no lines left!
          finished = true;
        }
        else if (sq(row) <= sq(rove1y) + sq(pageWidth-rove1x))
        {
          out.println("On the top edge.");
          // measure on the top edge of the rove area
          xIntersection = pageWidth-sqrt(sq(row) - sq(rove1y));
          yIntersection = rove1y;
          
          out.print("New row starts at (mm) x:");
          out.print(rove1x * mmPerStep);
          out.print(",Y:");
          out.print(rove1y * mmPerStep);
      
          // move      
          tA = getMachineA(xIntersection, yIntersection);
          tB = getMachineB(xIntersection, yIntersection);
      
          finished = false;
        }
        else
        {
          out.println("On the left edge.");
          // measure on the left edge of the rove area
          xIntersection = rove1x;
          yIntersection = sqrt(sq(row) - sq(pageWidth - rove1x));
  
          out.print("New row starts at (mm) x:");
          out.print(rove1x * mmPerStep);
          out.print(",Y:");
          out.print(rove1y * mmPerStep);
    
          // move      
          tA = getMachineA(xIntersection, yIntersection);
          tB = getMachineB(xIntersection, yIntersection);
    
          finished = false;
        }      
  
        delay(1000);
      }
      else
      {
        out.println("In area.");
      }
      
      if (!finished)
      {
        changeLength(tA,tB);
        penlift_penDown();
        pixels++;
        out.print("Pixel ");
        out.println(pixels);
        // draw pixel
        // Measure cartesian position at that point.
        float cX = getCartesianXFP(tA, tB);
        float cY = getCartesianYFP(cX, tA);
        
        cX -= rove1x;
        cY -= rove1y;
        
        out.print("Drawing pixel on page at x:");
        out.print(cX); //* mmPerStep);
        out.print(", y:");
        out.println(cY);// * mmPerStep);
        // Scale down to cartesian position in bitmap
        cX = cX / pbmScaling;
        cY = cY / pbmScaling;

        out.print("Drawing pixel from file at pixel x:");
        out.print(cX);
        out.print(", y:");
        out.println(cY);
        
        if (int(cY) > pbmHeight || int(cX) > pbmWidth)
        {
          out.println("Out of pixels. Cancelling");
          finished = true;
        }
        else
        {
          // Get pixel brightness at that position
          byte brightness = sd_getBrightnessAtPixel(cX, cY);
          
          if (brightness < 0)
          {
            out.println("No brightness value found. Cancelling.");
            finished = true;
          }
          else
          {
            // Scale pixel amplitude to be in range 0 to <maxAmplitude>,
            // where brightest = 0 and darkest = <maxAmplitude>
            byte amplitude = brightness;
            amplitude = pixel_scaleDensity(amplitude, pbmDepth, maxAmplitude);
  
            // Draw the wave:
            float halfWavelength = float(wavelength) / 2.0;
            float halfAmplitude = float(amplitude) / 2.0;
            changeLength(tA+halfWavelength, tB-halfAmplitude);
            changeLength(tA+halfWavelength, tB+halfAmplitude);
            changeLength(tA+wavelength, tB);
            tA += wavelength;
//            changeLength(tA, tB);
          }
        }
      }
      else
      {
        out.println("Finished!!");
        // finished
      }
    }
    penlift_penUp();
  }
  else
  {
    out.println("Rove area must be chosen for this operation.");
  }
}

void rove_drawRoveAreaFittedToImage()
{
  if (useRoveArea)
  {
    // get parameters
    String filename = inParam1;

    // Look up file and open it
    if (!sd_openPbm(filename))
    {
      out.print("Couldn't open that file - ");
      out.println(filename);
      return;
    }
    else
    {
      out.print("image size "); 
      out.print(pbmWidth, DEC);
      out.print(", ");
      out.println(pbmHeight, DEC);
      out.print("(roveWidth:");
      out.print(roveWidth);
      out.println(")");
      pbmScaling = roveWidth / pbmWidth;
      out.print("Scaling factor:");
      out.println(pbmScaling);
      out.print("Rove width:");
      out.println(roveWidth);
      out.print("Image offset:");
      out.println(pbmImageoffset);
    }
    
    // set roveHeight so that it is the same shape as the image.  
    roveHeight = roveWidth * pbmAspectRatio;
    long rove2x = rove1x + roveWidth;
    long rove2y = rove1y + roveHeight;

    out.print("rove2x:");
    out.print(rove2x);
    out.print("rove2y:");
    out.println(rove2y);

    
    // go to first point, top-left
//    out.println("Point 1.");
    float mA = motorA.currentPosition();
    float mB = motorB.currentPosition();
    float tA = getMachineA(rove1x, rove1y);
    float tB = getMachineB(rove1x, rove1y);
//    out.print("now a:");
//    out.print(mA);
//    out.print(",b:");
//    out.print(mB);
//    out.print(", target a:");
//    out.print(tA);
//    out.print(", b:");
//    out.print(tB);
    exec_drawBetweenPoints(mA, mB, tA, tB, 20);
        
//    out.println("Point 2.");
    mA = motorA.currentPosition();
    mB = motorB.currentPosition();
    tA = getMachineA(rove2x, rove1y);
    tB = getMachineB(rove2x, rove1y);
//    out.print("now a:");
//    out.print(mA);
//    out.print(",b:");
//    out.print(mB);
//    out.print(", target a:");
//    out.print(tA);
//    out.print(", b:");
//    out.println(tB);
    exec_drawBetweenPoints(mA, mB, tA, tB, 20);

//    out.println("Point 3.");
    mA = motorA.currentPosition();
    mB = motorB.currentPosition();
    tA = getMachineA(rove2x, rove2y);
    tB = getMachineB(rove2x, rove2y);
//    out.print("now a:");
//    out.print(mA);
//    out.print(",b:");
//    out.print(mB);
//    out.print(", target a:");
//    out.print(tA);
//    out.print(", b:");
//    out.println(tB);
    exec_drawBetweenPoints(mA, mB, tA, tB, 20);

//    out.println("Point 4.");
    mA = motorA.currentPosition();
    mB = motorB.currentPosition();
    tA = getMachineA(rove1x, rove2y);
    tB = getMachineB(rove1x, rove2y);
//    out.print("now a:");
//    out.print(mA);
//    out.print(",b:");
//    out.print(mB);
//    out.print(", target a:");
//    out.print(tA);
//    out.print(", b:");
//    out.println(tB);
    exec_drawBetweenPoints(mA, mB, tA, tB, 20);

//    out.println("Point 5.");
    mA = motorA.currentPosition();
    mB = motorB.currentPosition();
    tA = getMachineA(rove1x, rove1y);
    tB = getMachineB(rove1x, rove1y);
//    out.print("now a:");
//    out.print(mA);
//    out.print(",b:");
//    out.print(mB);
//    out.print(", target a:");
//    out.print(tA);
//    out.print(", b:");
//    out.println(tB);
    exec_drawBetweenPoints(mA, mB, tA, tB, 20);
    exec_drawBetweenPoints(float(motorA.currentPosition()), float(motorB.currentPosition()), getMachineA(rove1x, rove1y), getMachineB(rove1x, rove1y), 20);
//    out.println("Done.");
    
  }
  else
  {
    out.println("Rove area must be chosen for this operation.");
  }
}

/**
*  This moves to a random positions inside the rove area.
*/
void  rove_moveToRandomPositionInRoveArea()
{
  long x = random(rove1x, rove1x+roveWidth);
  long y = random(rove1y, rove1y+roveHeight);
  float a = getMachineA(x,y);
  float b = getMachineB(x,y);
  
  penlift_penUp();
  changeLength(a,b);
}

void rove_swirl()
{
  motorA.run();
  motorB.run();
  
  if (motorA.distanceToGo() == 0)
  {
    long x = random(rove1x, rove1x+roveWidth);
    long y = random(rove1y, rove1y+roveHeight);
    float a = getMachineA(x,y);
    motorA.moveTo(a);
  }

  if (motorB.distanceToGo() == 0)
  {
    long x = random(rove1x, rove1x+roveWidth);
    long y = random(rove1y, rove1y+roveHeight);
    float b = getMachineB(x,y);
    motorB.moveTo(b);
  }
}

void rove_controlSwirling()
{
  if (atoi(inParam1) == 0)
  {
    swirling = false;
  }
  else 
  {
    if (useRoveArea)
    {
      swirling = true;
    }
    else
    {
      out.println("Rove area must be defined to swirl.");
    }
  }
}

#endif

