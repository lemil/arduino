#include <EEPROM.h>
 
int addr = 0;




void setup_write()
{
  
  EEPROM.begin(512);  //Initialize EEPROM
 
  // write to EEPROM.
  EEPROM.write(addr, 'a');    
  addr++;                      //Increment address
  EEPROM.write(addr, 'b');   
  addr++;                      //Increment address
  EEPROM.write(addr, 'C');    

 
  //Write string to eeprom
  String sample = "testing eeprom";
  for(int i=0;i<sample.length();i++)
  {
    EEPROM.write(0x0F+i, sample[i]); //Write one by one with starting address of 0x0F
  }
  EEPROM.commit();    //Store data to EEPROM
  
}

void setup_read()
{
  int addr = 0;
    
  EEPROM.begin(512);  //Initialize EEPROM
 
  Serial.println("");
  byte g   = EEPROM.read(addr);   
  int i = atoi( g );
  Serial.print(i);
  addr++;                      //Increment address
  Serial.print(char(EEPROM.read(addr)));
  addr++;                      //Increment address
  Serial.println(char(EEPROM.read(addr)));
 
  //Read string from eeprom (testing eeprom)
  String strText;   
  for(int i=0;i<14;i++) 
  {
    strText = strText + char(EEPROM.read(0x0F+i)); //Read one by one with starting address of 0x0F    
  }  
 
  Serial.print(strText);  //Print the text
}

void setup()
{
  Serial.begin( 115200 );
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  //setup_write();
}

void loop()
{   
  Serial.println("Ready");
  //setup_read();
  delay(1000);
}
