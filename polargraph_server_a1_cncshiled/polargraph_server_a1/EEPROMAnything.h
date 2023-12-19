template <class T> int EEPROM_writeAnything(int ee, const T& value)
{
#if MICROCONTROLLER == MC_ESP8266
    EEPROM.begin(EEPROM_BEGIN);  //Initialize EEPROM
#endif    
    const byte* p = (const byte*)(const void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
    {
      EEPROM.write(ee++, *p++);
      //EEPROM.write(0x0F+i, sample[i]); //Write one by one with starting address of 0x0F
    }
    
#if MICROCONTROLLER == MC_ESP8266
    EEPROM.commit();
//    Serial.println("EEPROM successfully committed");
#endif
   
    return i;
}


template <class T> int EEPROM_readAnything(int ee, T& value)
{
#if MICROCONTROLLER == MC_ESP8266
    EEPROM.begin(EEPROM_BEGIN);  //Initialize EEPROM
#endif   
    byte* p = (byte*)(void*)&value;
    int i;
    for (i = 0; i < sizeof(value); i++)
        *p++ = EEPROM.read(ee++);
    return i;
    
}
