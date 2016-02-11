void sensorReadingChanges(){
  tft.setTextSize(2); //set font size
  tft.setTextColor(ILI9341_WHITE); // set font to black
  String text;
  if (sensorReadings[1] - sensorReadingsPrev[1] != 0){
    tft.fillRect(120, 70, 200, 17, ILI9341_BLACK); // clear value on screen
    tft.setCursor(120,70); // st position of cursor
    text = String(sensorReadings[1])+" deg";
    tft.print(text);
    sensorReadingsPrev[1] = sensorReadings[1]; 
  }
  if (sensorReadings[2] - sensorReadingsPrev[2] != 0){
    tft.fillRect(120, 100, 200, 17, ILI9341_BLACK); // clear value on screen
    tft.setCursor(120,100); // st position of cursor
    text = String(sensorReadings[2])+" deg";
    tft.print(text);
    sensorReadingsPrev[2] = sensorReadings[2]; 
  }
  if (sensorReadings[3] - sensorReadingsPrev[3] != 0){
    tft.fillRect(120, 130, 200, 17, ILI9341_BLACK); // clear value on screen
    tft.setCursor(120,130); // st position of cursor
    text = String(sensorReadings[3])+" Pa";
    tft.print(text);
    sensorReadingsPrev[3] = sensorReadings[3]; 
  }     
}
void batteryStructure(){
  tft.drawLine(275, 5, 280, 5, ILI9341_WHITE);
  tft.drawLine(275, 5, 275, 18, ILI9341_WHITE);
  tft.drawLine(275, 18, 280, 18, ILI9341_WHITE);
  tft.drawLine(280, 3, 318, 3, ILI9341_WHITE);
  tft.drawLine(318, 3, 318, 21, ILI9341_WHITE);
  tft.drawLine(280, 21, 318, 21, ILI9341_WHITE);
  tft.drawLine(280, 21, 280, 18, ILI9341_WHITE);
  tft.drawLine(280, 3, 280, 5, ILI9341_WHITE);
}
void batteryMeter(){
  volatile float level = float(sensorReadings[0])/float(1023) * float(6);
  level = level / float(6);
  Serial.println(level);
  tft.fillRect(281, 5, 35, 15, 0x00); // clear battery
  tft.fillRect(277, 7, 5, 10, 0x00);
  tft.fillRect(317-(level*35), 5, level*35, 15, 0xffffff); // fill battery body
  if(level == 1)tft.fillRect(277, 7, 5, 10, 0xffffff); // fill tip of battery
  tft.setCursor(287,9); // st position of cursor
  tft.setTextSize(1); //set font size
  tft.setTextColor(ILI9341_RED); // set font to black
  String text = String(int(level* 100))+ "%";
  tft.print(text);
}

/*
 * command button designer
 */
void bigButton(uint8_t x, uint8_t y, uint8_t width, uint32_t foreColor,uint32_t solidColor, uint8_t offset, String text ){
  tft.fillRect(x, y, width, BOXSIZE, solidColor); //create  command button
  tft.setCursor(x+offset,y+15); // st position of cursor
  tft.setTextSize(2); //set font size
  tft.setTextColor(foreColor); // set font to black
  tft.print(text);
}

/*
 * increase the value in question on pressing the increment button
 */
void increment(int *val, String units, int x){
  *val += 1;
  String text = String(*val) + units;
  tft.setTextSize(2); //set font size
  tft.setCursor(x,115); // st position of cursor
  tft.setTextColor(0xFFFFFF); // set font to white
  tft.print(text);
}

/*
 * decrease the value in question on pressing the decrement button
 */
void decrement(int *val, String units, int x){
  *val -= 1;
  String text = String(*val) + units;
  tft.setTextSize(2); //set font size
  tft.setCursor(x,115); // st position of cursor
  tft.setTextColor(0xFFFFFF); // set font to white
  tft.print(text);
}

/*
 * sets up interface for adjustment of values i.e. +, - and lock in buttons
 */
void adjustmentInterface(){
  bigButton(255, 70, 40, 0x00, 0x959191, 15, "+" );
  bigButton(255, 140, 40, 0x00, 0x959191, 15, "-" );
  bigButton(100, 190, 110, 0x00, 0x959191, 15, "Lock In" );
}

/*
 * pressure trigger interface
 */
 void triggerInterface(String label, uint8_t num, int trigger){
  tft.setTextSize(2); //set font size
  tft.setTextColor(0xFCBA47); // set font to brown
  tft.setCursor(110,30); // st position of cursor
  tft.print("DEFAULTS");  
  tft.setTextSize(2); //set font size
  tft.setCursor(10,70); // st position of cursor
  tft.print("Pr = "); 
  tft.setCursor(10,115); // st position of cursor
  tft.print(label);  
  adjustmentInterface();
  batteryStructure();
  clearPrev();

  tft.setTextColor(0xFFFFFF); // set font to white
  tft.setCursor(165,115); // st position of cursor
  String text = String(trigger) + " Pa";
  tft.print(text);  
  pageNumber = num;
}

/*
 * data display interface
 */
void displayData(){
  EEPROM_readAnything(2000, vol); // get volume from EEPROM
  float CFM = float(vol) * 60 / (timer/float(10)); // calculate CFM
  float CT = sensorReadings[1] - sensorReadings[2]; // get temperature difference
  BTUH = CFM * CT * 1.08;
  tft.fillScreen(ILI9341_BLACK);
  batteryStructure();
  clearPrev();
  tft.setTextColor(0xFFFFFF); // set font to white
  tft.setCursor(118,60); // st position of cursor
  tft.print("BTUH = ");  
  tft.print(BTUH);
  tft.setCursor(118,80); // st position of cursor
  tft.print("Time = ");  
  tft.print(timer);
  tft.setCursor(58,100); // st position of cursor
  tft.print("Temp Sply = ");  
  tft.print(sensorReadings[1]);
  tft.setCursor(70,120); // st position of cursor
  tft.print("Temp Rtn = ");  
  tft.print(sensorReadings[2]);
  tft.setCursor(10,140); // st position of cursor
  tft.print("Measure Count = ");  
  tft.print(EEPROM.read(0));
 }

 /*
  * Scroll saved values
  */
void next(){
  int num = EEPROM.read(0); // get total saved preset
  if (num == 255) num = 0; // set num to 0 if its 255
  BOXSIZE = 100;
  bigButton(200, 60, 120, 0x00, 0x00, 0, ""); // clear the text in the body of the page
  BOXSIZE = 40;
  tft.setTextSize(2); //set font size
  tft.setTextColor(0xFFFFFF); // set font to white
  if (num != 0){
    count++;
    if(count > num) count = 1;
    int addr = (count - 1) * 16 + 1; // set begining address
    tft.setCursor(200,60); // st position of cursor
    tft.print(count);
    tft.setCursor(200,80); // st position of cursor
    EEPROM_readAnything( addr, BTUH);
    tft.print(BTUH);
    tft.setCursor(200,100); // st position of cursor
    EEPROM_readAnything( addr + 4, timer);
    tft.print(timer); tft.print(" sec");
    tft.setCursor(200,120); // st position of cursor
    EEPROM_readAnything( addr + 8, sensorReadings[1]);
    tft.print(sensorReadings[1]); tft.print(" deg");
    tft.setCursor(200,140); // st position of cursor
    EEPROM_readAnything( addr + 12, sensorReadings[2]);
    tft.print(sensorReadings[2]); tft.print(" deg");
  }
}

/*
 * scroll to previous saved settings
 */
void previous(){
  int num = EEPROM.read(0); // get total saved preset
  if (num == 255) num = 0; // set num to 0 if its 255
  BOXSIZE = 100;
  bigButton(200, 60, 120, 0x00, 0x00, 0, ""); // clear the text in the body of the page
  BOXSIZE = 40;
  tft.setTextSize(2); //set font size
  tft.setTextColor(0xFFFFFF); // set font to white
  if (num != 0){
    count--;
    if(count < 1) count = num;
    int addr = (count - 1) * 16 + 1; // set begining address
    tft.setCursor(200,60); // st position of cursor
    tft.print(count);
    tft.setCursor(200,80); // st position of cursor
    EEPROM_readAnything( addr, BTUH);
    tft.print(BTUH);
    tft.setCursor(200,100); // st position of cursor
    EEPROM_readAnything( addr + 4, timer);
    tft.print(timer); tft.print(" sec");
    tft.setCursor(200,120); // st position of cursor
    EEPROM_readAnything( addr + 8, sensorReadings[1]);
    tft.print(sensorReadings[1]); tft.print(" deg");
    tft.setCursor(200,140); // st position of cursor
    EEPROM_readAnything( addr + 12, sensorReadings[2]);
    tft.print(sensorReadings[2]); tft.print(" deg");
  }
}

