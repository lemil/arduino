/**
*  Polargraph Server. - CORE
*  Written by Sandy Noble
*  Released under GNU License version 3.
*  http://www.polargraph.co.uk
*  https://github.com/euphy/polargraph_server_a1

Pixel.

This is one of the core files for the polargraph server program.  

This is a biggie, and has the routines necessary for generating and drawing
the squarewave and scribble pixel styles.

*/
#ifdef PIXEL_DRAWING
void pixel_changeDrawingDirection() 
{
  globalDrawDirectionMode = atoi(inParam1);
  globalDrawDirection = atoi(inParam2);
//  out.print(F("Changed draw direction mode to be "));
//  out.print(globalDrawDirectionMode);
//  out.print(F(" and direction is "));
//  out.println(globalDrawDirection);
}

void pixel_drawSquarePixel() 
{
    long originA = multiplier(atol(inParam1));
    long originB = multiplier(atol(inParam2));
    int size = multiplier(atoi(inParam3));
    int density = atoi(inParam4);
    
    /*  Here density is accepted as a recording of BRIGHTNESS, where 0 is black and 255 is white.
        Later on, density gets scaled to the range that is available for this particular 
        pixel+pentip combination, and also inverted so that it becomes a recording of DARKNESS,
        where 0 is white and the higher values are darker.
        
        (Using the same variable to save on space, really.)
        
        This is because paper is white, and ink is black, and this density value is used to 
        control how many waves are drawn. 
        
        O waves means no ink, so a very light pixel.
        50 waves means lots of ink, so a much darker pixel.
    */

    int halfSize = size / 2;
    
    long startPointA;
    long startPointB;
    long endPointA;
    long endPointB;

    int calcFullSize = halfSize * 2; // see if there's any rounding errors
    int offsetStart = size - calcFullSize;
    
    if (globalDrawDirectionMode == DIR_MODE_AUTO)
      globalDrawDirection = pixel_getAutoDrawDirection(originA, originB, motorA.currentPosition(), motorB.currentPosition());
      

    if (globalDrawDirection == DIR_SE) 
    {
//      out.println(F("d: SE"));
      startPointA = originA - halfSize;
      startPointA += offsetStart;
      startPointB = originB;
      endPointA = originA + halfSize;
      endPointB = originB;
    }
    else if (globalDrawDirection == DIR_SW)
    {
//      out.println(F("d: SW"));
      startPointA = originA;
      startPointB = originB - halfSize;
      startPointB += offsetStart;
      endPointA = originA;
      endPointB = originB + halfSize;
    }
    else if (globalDrawDirection == DIR_NW)
    {
//      out.println(F("d: NW"));
      startPointA = originA + halfSize;
      startPointA -= offsetStart;
      startPointB = originB;
      endPointA = originA - halfSize;
      endPointB = originB;
    }
    else //(drawDirection == DIR_NE)
    {
//      out.println(F("d: NE"));
      startPointA = originA;
      startPointB = originB + halfSize;
      startPointB -= offsetStart;
      endPointA = originA;
      endPointB = originB - halfSize;
    }

    /* pixel_scaleDensity takes it's input value as a BRIGHTNESS value (ie 255 = white),
       but returns a DARKNESS value (ie 0 = white). 
       Here I'm using the same variable to hold both, save space in memory. */
    density = pixel_scaleDensity(density, 255, pixel_maxDensity(penWidth, size));
//    out.print(F("Start point: "));
//    out.print(startPointA);
//    out.print(COMMA);
//    out.print(startPointB);
//    out.print(F(". end point: "));
//    out.print(endPointA);
//    out.print(COMMA);
//    out.print(endPointB);
//    out.println(F("."));
    
    changeLength(startPointA, startPointB);
    if (density > 1)
    {
      pixel_drawSquarePixel(size, size, density, globalDrawDirection);
    }
    changeLength(endPointA, endPointB);
    
    //outputAvailableMemory(); 
}

byte pixel_getRandomDrawDirection()
{
  return random(1, 5);
}

byte pixel_getAutoDrawDirection(long targetA, long targetB, long sourceA, long sourceB)
{
  byte dir = DIR_SE;
  
  if (targetA<sourceA && targetB<sourceA)
    dir = DIR_NW;
  else if (targetA>sourceA && targetB>sourceB)
    dir = DIR_SE;
  else if (targetA<sourceA && targetB>sourceB)
    dir = DIR_SW;
  else if (targetA>sourceA && targetB<sourceB)
    dir = DIR_NE;
  else if (targetA==sourceA && targetB<sourceB)
    dir = DIR_NE;
  else if (targetA==sourceA && targetB>sourceB)
    dir = DIR_SW;
  else if (targetA<sourceA && targetB==sourceB)
    dir = DIR_NW;
  else if (targetA>sourceA && targetB==sourceB)
    dir = DIR_SE;
  else
  {
//    out.println("Not calculated - default SE");
  }

  return dir;
}

void pixel_drawScribblePixel() 
{
    long originA = multiplier(atol(inParam1));
    long originB = multiplier(atol(inParam2));
    int size = multiplier(atoi(inParam3));
    int density = atoi(inParam4);
    
    int maxDens = pixel_maxDensity(penWidth, size);

    density = pixel_scaleDensity(density, 255, maxDens);
    pixel_drawScribblePixel(originA, originB, size*1.1, density);
    
//    outputAvailableMemory(); 
}

void pixel_drawScribblePixel(long originA, long originB, int size, int density) 
{

//  int originA = motorA.currentPosition();
//  int originB = motorB.currentPosition();
  
  long lowLimitA = originA-(size/2);
  long highLimitA = lowLimitA+size;
  long lowLimitB = originB-(size/2);
//  long highLimitB = lowLimitB+size;
  int randA;
  int randB;
  
  int inc = 0;
  int currSize = size;
  
  for (int i = 0; i <= density; i++)
  {
    randA = random(0, currSize);
    randB = random(0, currSize);
    changeLength(lowLimitA+randA, lowLimitB+randB);
    
    lowLimitA-=inc;
    highLimitA+=inc;
    currSize+=inc*2;
  }
}

int pixel_minSegmentSizeForPen(float penSize)
{
  float penSizeInSteps = penSize * stepsPerMM;

  int minSegSize = 1;
  if (penSizeInSteps >= 2.0)
    minSegSize = int(penSizeInSteps);
    
//  out.print(F("Min segment size for penSize "));
//  out.print(penSize);
//  out.print(F(": "));
//  out.print(minSegSize);
//  out.print(F(" steps."));
//  out.println();
  
  return minSegSize;
}

int pixel_maxDensity(float penSize, int rowSize)
{
  float rowSizeInMM = mmPerStep * rowSize;
#ifdef DEBUG_PIXEL
  out.print(F("MSG,D,rowsize in mm: "));
  out.print(rowSizeInMM);
  out.print(F(", mmPerStep: "));
  out.print(mmPerStep);
  out.print(F(", rowsize: "));
  out.println(rowSize);
#endif

  float numberOfSegments = rowSizeInMM / penSize;
  int maxDens = 1;
  if (numberOfSegments >= 2.0)
    maxDens = int(numberOfSegments);
    
  if (maxDens <= 2)
  {
    out.print("num of segments float:");
    out.println(numberOfSegments);
    out.print(F("MSG,I,Max density for penSize: "));
    out.print(penSize);
    out.print(F(", rowSize: "));
    out.print(rowSize);
    out.print(F(" is "));
    out.println(maxDens);
    out.println(F("MSG,I,You probably won't get any detail in this."));
  }
    

//
//    
  
  return maxDens;
}

int pixel_scaleDensity(int inDens, int inMax, int outMax)
{
  float reducedDens = (float(inDens) / float(inMax)) * float(outMax);
  reducedDens = outMax-reducedDens;
//  out.print(F("inDens:"));
//  out.print(inDens);
//  out.print(F(", inMax:"));
//  out.print(inMax);
//  out.print(F(", outMax:"));
//  out.print(outMax);
//  out.print(F(", reduced:"));
//  out.println(reducedDens);
  
  // round up if bigger than .5
  int result = int(reducedDens);
  if (reducedDens - (result) > 0.5)
    result ++;
  
  return result;
}

void pixel_drawSquarePixel(int length, int width, int density, byte drawDirection) 
{
  // work out how wide each segment should be
  int segmentLength = 0;

  if (density > 0)
  {
    // work out some segment widths
    int basicSegLength = length / density;
    int basicSegRemainder = length % density;
    float remainderPerSegment = float(basicSegRemainder) / float(density);
    float totalRemainder = 0.0;
    int lengthSoFar = 0;
    
//    out.print("Basic seg length:");
//    out.print(basicSegLength);
//    out.print(", basic seg remainder:");
//    out.print(basicSegRemainder);
//    out.print(", remainder per seg");
//    out.println(remainderPerSegment);
    
    for (int i = 0; i <= density; i++) 
    {
      totalRemainder += remainderPerSegment;

      if (totalRemainder >= 1.0)
      {
        totalRemainder -= 1.0;
        segmentLength = basicSegLength+1;
      }
      else
      {
        segmentLength = basicSegLength;
      }

      if (drawDirection == DIR_SE) {
        pixel_drawSquareWaveAlongAxis(motorA, motorB, width, segmentLength, density, i);
      }
      else if (drawDirection == DIR_SW) {
        pixel_drawSquareWaveAlongAxis(motorB, motorA, width, segmentLength, density, i);
      }
      else if (drawDirection == DIR_NW) {
        segmentLength = 0 - segmentLength; // reverse
        pixel_drawSquareWaveAlongAxis(motorA, motorB, width, segmentLength, density, i);
      }
      else if (drawDirection == DIR_NE) {
        segmentLength = 0 - segmentLength; // reverse
        pixel_drawSquareWaveAlongAxis(motorB, motorA, width, segmentLength, density, i);
      }
      lengthSoFar += segmentLength;
    //      out.print("distance so far:");
    //      out.print(distanceSoFar);
      
      
      reportPosition();
    } // end of loop
  }
}

void pixel_drawSquareWaveAlongAxis(AccelStepper &longAxis, AccelStepper &wideAxis, int waveAmplitude, int waveLength, int totalWaves, int waveNo)
{
  if (waveNo == 0) 
  { 
    // first one, half a line and an along
    out.println(F("First wave half"));
    if (lastWaveWasTop) {
      moveAxis(wideAxis, waveAmplitude/2);
      moveAxis(longAxis, waveLength);
    }
    else {
      moveAxis(wideAxis, 0-(waveAmplitude/2));
      moveAxis(longAxis, waveLength);
    }
    pixel_flipWaveDirection();
  }
  else if (waveNo == totalWaves) 
  { 
    // last one, half a line with no along
    if (lastWaveWasTop) {
      moveAxis(wideAxis, waveAmplitude/2);
    }
    else {
      moveAxis(wideAxis, 0-(waveAmplitude/2));
    }
  }
  else 
  { 
    // intervening lines - full lines, and an along
    if (lastWaveWasTop) {
      moveAxis(wideAxis, waveAmplitude);
      moveAxis(longAxis, waveLength);
    }
    else {
      moveAxis(wideAxis, 0-waveAmplitude);
      moveAxis(longAxis, waveLength);
    }
    pixel_flipWaveDirection();
  }
}

void pixel_flipWaveDirection()
{
  if (lastWaveWasTop)
    lastWaveWasTop = false;
  else
    lastWaveWasTop = true;
}

  void pixel_testPenWidth()
  {
    int rowWidth = multiplier(atoi(inParam1));
    float startWidth = atof(inParam2);
    float endWidth = atof(inParam3); 
    float incSize = atof(inParam4);

    int tempDirectionMode = globalDrawDirectionMode;
    globalDrawDirectionMode = DIR_MODE_PRESET;
    
    float oldPenWidth = penWidth;
    int iterations = 0;
    
    for (float pw = startWidth; pw <= endWidth; pw+=incSize)
    {
      iterations++;
      penWidth = pw;
      int maxDens = pixel_maxDensity(penWidth, rowWidth);
//      out.print(F("Penwidth test "));
//      out.print(iterations);
//      out.print(F(", pen width: "));
//      out.print(penWidth);
//      out.print(F(", max density: "));
//      out.println(maxDens);
      pixel_drawSquarePixel(rowWidth, rowWidth, maxDens, DIR_SE);
    }

    penWidth = oldPenWidth;
    
    moveAxis(motorB, 0-rowWidth);
    for (int i = 1; i <= iterations; i++)
    {
      moveAxis(motorB, 0-(rowWidth/2));
      moveAxis(motorA, 0-rowWidth);
      moveAxis(motorB, rowWidth/2);
    }
    
    penWidth = oldPenWidth;
    globalDrawDirectionMode = tempDirectionMode;
  }
#endif

