// increments timer by 1 every second
void incrTimer(){
   if(!action) action = true;
    timer ++;
    bigButton(100,110, 220, 0xFFFFFF, 0x00, 0, ""); // clear the text in the body of the page
    tft.setTextSize(3); //set font size
    tft.setCursor(130,110); // st position of cursor
    tft.print(timer); 
    sensorReadings[3] = getSensorReadings(A2, 200); // get temp sply
    if (sensorReadings[3] >= trig_pr_hi){
      timer = timer/float(10);
      calculateBTU = true;
      Timer1.stop(); //stop interrupt
    }
}

/*
 * read sensor readings
 */
void readSensors(){
  sensorReadings[1] = getSensorReadings(A0, 100); // get temp sply
  sensorReadings[2] = getSensorReadings(A1, 100); // get temp rtn
  sensorReadings[3] = getSensorReadings(A2, 200); // get temp sply
  if(pageNumber == 0){
    BOXSIZE = 20;
    tft.setTextSize(2); //set font size
    if(abs(sensorReadings[1] - sensorReadingsPrev[1]) > 2){
      bigButton(120,70,200, 0x00,0x00, 0, "");
      tft.setTextColor(0xFFFFFF); // set font to brown
      tft.setCursor(120,70); // st position of cursor
      tft.print(sensorReadings[1]);
      tft.print(" deg");
      sensorReadingsPrev[1] = sensorReadings[1];
    }
    if(abs(sensorReadings[2] - sensorReadingsPrev[2]) > 2){
      bigButton(120,100,200, 0x00,0x00, 0, "");
      tft.setTextColor(0xFFFFFF); // set font to brown
      tft.setCursor(120,100); // st position of cursor
      tft.print(sensorReadings[2]);
      tft.print(" deg");
      sensorReadingsPrev[2] = sensorReadings[2];
    }
  }
  if(pageNumber == 0 || pageNumber == 3 || pageNumber == 4){
    if(abs(sensorReadings[3] - sensorReadingsPrev[3]) > 2){
      if (pageNumber == 3 || pageNumber == 4){
        bigButton(80,70,120, 0x00,0x00, 0, "");
        tft.setCursor(80,70); // st position of cursor
      }
      if (pageNumber == 0){
        bigButton(120,130,200, 0x00,0x00, 0, "");
        tft.setCursor(120,130); // st position of cursor
      }
      tft.setTextColor(0xFFFFFF); // set font to white
      tft.print(sensorReadings[3]);
      tft.print(" Pa");
      sensorReadingsPrev[3] = sensorReadings[3];
    }
    BOXSIZE = 40;
  }
}

/*
 * save the records in EEPROM
 */
template <class T> void EEPROM_writeAnything(int ee, const T& value)
{
   const byte* p = (const byte*)(const void*)&value;
   int i;
   for (i = 0; i < sizeof(value); i++)
       EEPROM.write(ee++, *p++);
}

/*
 * read from EEPROM
 */
template <class T> void EEPROM_readAnything(int ee, T& value)
{
   byte* p = (byte*)(void*)&value;
   int i;
   for (i = 0; i < sizeof(value); i++)
       *p++ = EEPROM.read(ee++);
}

/*
 * save to EEPROM
 */
void saveEeprom(){
  int addr = EEPROM.read(0)*16+1; // set begining address
  EEPROM_writeAnything(addr, BTUH); //  save BTUH
  EEPROM_writeAnything(addr+4, timer); // save time
  EEPROM_writeAnything(addr+8, sensorReadings[1]); // save temp sply
  EEPROM_writeAnything(addr+12, sensorReadings[2]); // save temp rtn
  EEPROM.write(0,EEPROM.read(0)+1); // save measurement count
}

/*
 * get sensor readings
 */
float getSensorReadings(uint8_t num, int scale){
  volatile float reading = analogRead(num);
  reading = float(reading)/float(1023) * float(scale);
  return reading;
}

/*
 * clear previous values
 */
void clearPrev(){
  sensorReadingsPrev[0] = 0; // set previus battery reading to 0
  sensorReadingsPrev[1] = 0; // set previus battery reading to 0
  sensorReadingsPrev[2] = 0; // set previus battery reading to 0
  sensorReadingsPrev[3] = 0; // set previus battery reading to 0
}

