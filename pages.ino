/*
 * page 1. Displays all the sensor data real time
 */
void sensorReadingPage(){
  bigButton(93, 190, 145, 0x00, 0x959191, 2, "Go to BTU/hr");
  tft.setCursor(33,70); // st position of cursor
  tft.setTextSize(2); //set font size
  tft.setTextColor(0xFCBA47); // set font to brown
  tft.print("Temp 0: ");
  tft.setCursor(33,100); // st position of cursor
  tft.print("Temp 1: ");
  tft.setCursor(10,130); // st position of cursor
  tft.print("Pressure: ");
  tft.setCursor(70,30); // st position of cursor
  tft.print("SENSOR READINGS");  
  batteryStructure();
  clearPrev();
  pageNumber = 0;
}
/*
 * Page 2. Contains the main menu
 */
void page2(){
  bigButton(65, 50, 187, 0x00, 0x959191, 5, "Adjust Defaults");
  bigButton(45, 110, 225, 0x00, 0x959191, 5, "Start Measurements");
  bigButton(95, 170, 117, 0x00, 0x959191, 5, "View Data");
  batteryStructure();
  clearPrev();
  pageNumber = 1;
}

/*
 * page that allows us to change the default vol
 */
void page3(){
  tft.setTextSize(2); //set font size
  tft.setTextColor(0xFCBA47); // set font to brown
  tft.setCursor(110,30); // st position of cursor
  tft.print("DEFAULTS");  
  tft.setTextSize(2); //set font size
  tft.setCursor(10,115); // st position of cursor
  tft.print("Vol = ");
  adjustmentInterface();
  batteryStructure();
  clearPrev();

  tft.setTextColor(0xFFFFFF); // set font to white
  tft.setCursor(80,115); // st position of cursor
  EEPROM_readAnything( 2000, vol); // get volume from EEPROM
  String text = String(vol) + " m3";
  tft.print(text);
  pageNumber = 2;
}
 /*
  * page that allows us to set the low pressure trigger
  */
void page4(){
  EEPROM_readAnything(2004, trig_pr_lo);
  triggerInterface("Trig_Pr_Lo = ", 3, trig_pr_lo);
}

/*
 * page that allows us to set the high pressure trigger
 */
void page5(){
  EEPROM_readAnything(2008, trig_pr_hi);
  triggerInterface("Trig_Pr_Hi = ", 4, trig_pr_hi);
}

/*
 * measurement count page. you can reset the count or leave it as is
 */
void page6(){
  tft.setTextSize(2); //set font size
  tft.setTextColor(0xFCBA47); // set font to brown
  tft.setCursor(10,30); // st position of cursor
  tft.print("Reset the Measurement");
  tft.setCursor(10,60); // st position of cursor
  tft.print("Count?");
  tft.setCursor(10,100); // st position of cursor
  String text = "Measurement Cnt = " + String(EEPROM.read(0));
  tft.print(text);
  bigButton(10, 190, 100, 0x00, 0x959191, 20, "Reset");
  bigButton(150, 190, 150, 0x00, 0x959191, 10, "Don't Reset");
  batteryStructure();
  clearPrev();
  pageNumber = 5;
}

/*
 * measurement page. We get here after we have changed the defaults or directly from the main menu on the second page
 */
void page7(){
  tft.setTextSize(2); //set font size
  tft.setTextColor(0xFCBA47); // set font to brown
  tft.setCursor(120,30); // st position of cursor
  tft.print("MEASURE");  
  bigButton(10, 190, 145, 0x00, 0x959191, 55, "Arm");
  bigButton(230, 190, 76, 0x00, 0x959191, 15, "Exit");
  batteryStructure();
  clearPrev();
  pageNumber = 6;
}

/*
 * measurement page. Shows the timer as measurements are taken
 */
void page8(){
  bigButton(60, 80, 240, 0x00, 0x00, 0, ""); // clear the text in the body of the page
  bigButton(10, 190, 145, 0x00, 0x00, 0, "");
  bigButton(230, 190, 76, 0x00, 0x00, 0, "");
  bigButton(93, 190, 145, 0x00, 0x959191, 50, "Stop"); // change command button to say stop
  pageNumber = 7;
}

/*
 * measurement page. We get here after we have changed the defaults or directly from the main menu on the second page
 */
void page9(){
  batteryStructure();
  clearPrev();
  tft.setTextSize(2); //set font size
  tft.setTextColor(0xFCBA47); // set font to brown
  tft.setCursor(90,60); // st position of cursor
  tft.print("Time = ");  
  tft.print(timer);
  tft.print(" sec");
  tft.setCursor(20,100); // st position of cursor
  tft.setTextColor(0xFFFFFF); // set font to white
  tft.print("Calculating heat flow");  
  delay(500);
  tft.print(".");
  delay(500);
  tft.print(".");
  delay(500);
  tft.print(".");
  displayData(); // display all data on screen
  bigButton(10, 190, 100, 0x00, 0x959191, 27, "Save");
  bigButton(120, 190, 100, 0x00, 0x959191, 10, "Measure");
  bigButton(230, 190, 76, 0x00, 0x959191, 15, "Exit");
  pageNumber = 8;
}

/*
 * page that is displayed after data is saved
 */
void page10(){
  tft.setTextSize(2); //set font size
  displayData(); // display all data on screen
  bigButton(10, 190, 92, 0x00, 0x959191, 5, "Measure");
  bigButton(122, 190, 105, 0x00, 0x959191, 5, "Defaults");
  bigButton(250, 190, 58, 0x00, 0x959191, 5, "View");
  pageNumber = 9;
}

/*
 * display all parameters. View Page.
 */
void page11(){  
  tft.setTextSize(2); //set font size
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(0xFCBA47); // set font to brown
  tft.setCursor(140,30); // st position of cursor
  tft.print("VIEW"); 
  batteryStructure();
  clearPrev();
  tft.setTextColor(0xFFFFFF); // set font to white
  tft.setCursor(10,60); // st position of cursor
  tft.print("Measure Count = ");
  tft.setCursor(118,80); // st position of cursor
  tft.print("BTUH = ");
  tft.setCursor(118,100); // st position of cursor
  tft.print("Time = "); 
  tft.setCursor(58,120); // st position of cursor
  tft.print("Temp Sply = ");  
  tft.setCursor(70,140); // st position of cursor
  tft.print("Temp Rtn = "); 
  bigButton(10, 190, 92, 0x00, 0x959191, 15, "<Prev");
  bigButton(122, 190, 92, 0x00, 0x959191, 16, "Next>");
  bigButton(230, 190, 76, 0x00, 0x959191, 15, "Exit");
  pageNumber = 10;
 }


