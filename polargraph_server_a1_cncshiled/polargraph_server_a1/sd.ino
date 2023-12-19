/**
*  Polargraph Server for ATMEGA1280+ 
*  Written by Sandy Noble
*  Released under GNU License version 3.
*  http://www.polargraph.co.uk
*  https://github.com/euphy/polargraph_server_polarshield

Specific features for Polarshield / arduino mega.
SD.

For initialising, reading and writing the SD card data store.

*/

/*  ==============================================================
    Processing the SD card if there is one attached. 
=================================================================*/

#if MICROCONTROLLER == MC_MEGA
void sd_initSD()
{
  currentlyDrawingFromFile = false;
  currentlyDrawingFilename = "";
  cardPresent = false;
  cardInit = false;
  commandFilename = "";
  
//  sd_alternativeInit();
  sd_simpleInit();
}

void sd_simpleInit() {
  pinMode(chipSelect, OUTPUT); // necessary for SD card reading to work

  // see if the card is present and can be initialized:
  int initValue = 0;
  initValue = SD.begin(chipSelect);
  if (initValue == 0) {
    out.println("Card failed, or not present");
  }
  else {
    out.println("Successfully beginned.");
    cardPresent = true;
  }
  
  if (cardPresent) {
    out.println("card initialized.");
    root = SD.open("/", FILE_READ);
//    File entry =  root.openNextFile();    
//    entry.close();
    cardInit = true;
    sd_printDirectory(root, 0);
    out.println("done!");
  }
}

void sd_alternativeInit() {
  Sd2Card card;
  SdVolume volume;
  SdFile sdFile;
  if (!card.init(SPI_HALF_SPEED, chipSelect)) 
  {
    out.println("initialization failed. Things to check:");
    out.println("* is a card is inserted?");
    out.println("* Is your wiring correct?");
    out.println("* did you change the chipSelect pin to match your shield or module?");
    return;
  } 
  else 
  {
   out.println("Wiring is correct and a card is present."); 
  }

  // print the type of card
  out.print("\nCard type: ");
  switch(card.type()) {
    case SD_CARD_TYPE_SD1:
      out.println("SD1");
      break;
    case SD_CARD_TYPE_SD2:
      out.println("SD2");
      break;
    case SD_CARD_TYPE_SDHC:
      out.println("SDHC");
      break;
    default:
      out.println("Unknown");
  }

  // Now we will try to open the 'volume'/'partition' - it should be FAT16 or FAT32
  if (!volume.init(card)) {
    out.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
    return;
  }


  // print the type and size of the first FAT-type volume
  uint32_t volumesize;
  out.print("\nVolume type is FAT");
  out.println(volume.fatType(), DEC);
  out.println();
  
  volumesize = volume.blocksPerCluster();    // clusters are collections of blocks
  volumesize *= volume.clusterCount();       // we'll have a lot of clusters
  volumesize *= 512;                            // SD card blocks are always 512 bytes
  out.print("Volume size (bytes): ");
  out.println(volumesize);
  out.print("Volume size (Kbytes): ");
  volumesize /= 1024;
  out.println(volumesize);
  out.print("Volume size (Mbytes): ");
  volumesize /= 1024;
  out.println(volumesize);

  
//  out.println("\nFiles found on the card (name, date and size in bytes): ");
  sdFile.openRoot(volume);
  
// list all files in the card with date and size
  sdFile.ls(LS_R | LS_DATE | LS_SIZE);
  
  cardPresent = true;
  cardInit = true;  
}

/*
http://stackoverflow.com/questions/18158136/why-cant-i-pass-typedef-or-enum-in-arduino
Using struct File here so that the SD library does NOT need to be included.
*/
void sd_printDirectory(struct File dir, int numTabs) {
   while(true) {
     File entry =  dir.openNextFile();
     if (! entry) {
       // no more files
       // out.println("...");
       break;
     }
     for (uint8_t i=0; i<numTabs; i++) {
       out.print('\t');
     }
     out.print(entry.name());
     if (entry.isDirectory()) {
       out.println("/");
       sd_printDirectory(entry, numTabs+1);
     } else {
       // files have sizes, directories do not
       out.print("\t\t");
       out.println(entry.size(), DEC);
     }
     entry.close();
   }
}

void sd_storeCommand(String command)
{
  // delete file if it exists
  char filename[commandFilename.length()+1];
  commandFilename.toCharArray(filename, commandFilename.length()+1);

  File storeFile = SD.open(filename, FILE_WRITE);
  
  // if the file opened okay, write to it:
  if (storeFile) 
  {
    out.print("Writing to file ");
    out.println(commandFilename);
    storeFile.println(command);

    // close the file:
    storeFile.close();
    out.println("done.");
  } 
  else 
  {
    // if the file didn't open, print an error:
    out.print("error opening ");
    out.println(commandFilename);
  }  
}

/**
*  Most of this bmp image opening / handling stuff only slightly adapted from
*  Adafruit's marvellous stuff.
https://github.com/adafruit/TFTLCD-Library/blob/master/examples/tftbmp/tftbmp.pde
*/

boolean sd_openPbm(String pbmFilename)
{
  char filename[pbmFilename.length()+1];
  pbmFilename.toCharArray(filename, pbmFilename.length()+1);  
  pbmFile = SD.open(filename, FILE_READ);

  if (! pbmFile) 
  {
    out.println("didnt find image");
    return false;
  }
  
  if (! sd_pbmReadHeader()) 
  { 
     out.println("bad pbm");
     return false;
  }
  
  pbmFileLength = pbmFile.size();
  
  return true;
}

byte sd_getBrightnessAtPixel(int x, int y) 
{
  out.print("Pixel x:");
  out.print(x);
  out.print(", y:");
  out.println(y);
  
  out.print("PbmImageOffset:");
  out.println(pbmImageoffset);
  out.print("pbmWidth:");
  out.println(pbmWidth);
  
  long addressToSeek = (pbmImageoffset + (y * pbmWidth) + x);
  out.print("Address:");
  out.print(addressToSeek);
  if (addressToSeek > pbmFileLength)
  {
    return -1;
  }
  else
  {
    pbmFile.seek(addressToSeek);
    byte pixelValue = pbmFile.read();
    out.print(", Pixel value:");
    out.println(pixelValue);
    return pixelValue;
  }
}

boolean sd_pbmReadHeader() 
{
  pbmFile.seek(0);
  // read header
  char buf;
  String magicNumber = "  ";
  buf = pbmFile.read();
  magicNumber[0] = buf;
  
  buf = pbmFile.read();
  magicNumber[1] = buf;
  
  if (magicNumber != "P5")
  {
    out.print("This isn't a P5 file. It's a ");
    out.print(magicNumber);
    out.println(" file, and that's no good.");
    return false;
  }
  else
    out.println("This is a very good file Herr Doktor!");
  
  buf = pbmFile.read(); // this is a blank

  // get image width
  String numberString = "";
  buf = pbmFile.read();
  
  // photoshop puts a linebreak (0A) inbetween the width & height,
  // GIMP puts a space (20).
  while (buf != 0x0A && buf != 0x20) 
  {
    // check for comments, these start with a # - hex 23
    if (buf == 0x23)
    {
      while (buf != 0x0A)
        buf = pbmFile.read(); // just loop through until we get to the end of the comment
    }

    numberString = numberString + buf;
    buf = pbmFile.read();
  }
  
  out.print("PBM width:");
  out.println(numberString);
  
  char paramChar[numberString.length() + 1];
  numberString.toCharArray(paramChar, numberString.length() + 1);
  pbmWidth = atoi(paramChar);
  
  if (pbmWidth < 10)
  {
    out.println(F("PBM image must be at least 10 pixels wide."));
    return false;
  }

  // get image height
  numberString = "";
  buf = pbmFile.read();
  while (buf != 0x0A)
  {
    // check for comments, these start with a # - hex 23
    if (buf == 0x23)
    {
      while (buf != 0x0A)
        buf = pbmFile.read(); // just loop through until we get to the end of the comment
    }
    numberString = numberString + buf;
    buf = pbmFile.read();
  }
  
  out.print("PBM height:");
  out.println(numberString);
  
  paramChar[numberString.length() + 1];
  numberString.toCharArray(paramChar, numberString.length() + 1);
  pbmHeight = atoi(paramChar);
  
  // work out aspect ratio
  pbmAspectRatio = float(pbmHeight) / float(pbmWidth);
  out.print("PBM aspect ratio:");
  out.println(pbmAspectRatio);

  // get image depth
  numberString = "";
  buf = pbmFile.read();
  while (buf != 0x0A)
  {
    numberString = numberString + buf;
    buf = pbmFile.read();
  }
  
  out.print("Numberstring depth:");
  out.println(numberString);
  
  paramChar[numberString.length() + 1];
  numberString.toCharArray(paramChar, numberString.length() + 1);
  pbmDepth = atoi(paramChar);

  pbmImageoffset = pbmFile.position();
  out.print("Image offset:");
  out.println(pbmImageoffset);

  return true;
}
#endif
