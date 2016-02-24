#include <EEPROM.h>
#include <TimerOne.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <SPI.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_STMPE610.h>

// This is calibration data for the raw touch data to the screen coordinates
#define TS_MINX 250
#define TS_MINY 150
#define TS_MAXX 3850
#define TS_MAXY 3750

// The STMPE610 uses hardware SPI on the shield, and #8
#define STMPE_CS 8
Adafruit_STMPE610 ts = Adafruit_STMPE610(STMPE_CS);

// The display also uses hardware SPI, plus #9 & #10
#define TFT_CS 10
#define TFT_DC 9
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

// adc chip select pin #7
#define ADC_CS 7

const byte adc_single_ch0   = (0x08);     // ADC Single Channel 0
const byte adc_single_ch1   = (0x09);     // ADC Single Channel 1
const byte adc_single_ch2   = (0x0A);     // ADC Single Channel 2

SPISettings MCP3008(2000000, MSBFIRST, SPI_MODE0); // set up SPI terminal

// Size of the color selection boxes and the paintbrush size
volatile uint8_t BOXSIZE = 40;

// sensorReadings array holds data from the ADC 
// 0 - battery level
// 1 - temp0
// 2 - temp1
// 3 - pressure
volatile float sensorReadings[4] = {0,5,3,0};
volatile float sensorReadingsPrev[4] = {0,0,0,0};// sensorReadingsPrev holds previous sensor value readings in the defined order

volatile uint8_t pageNumber = 0; // number that marks the page that is currently displayed on the screen
int vol = 0; // holds the volume
int trig_pr_lo = 0; // holds the lower limit pressure
int trig_pr_hi = 0; // holds the upper limit pressure
float BTUH = 0; // holds BTUH
boolean action = false; // ensures there are no multiple clicks received
volatile float timer = 0; // times the sampling period
volatile boolean startTimer = false; // determines when to start the timer
volatile unsigned calculateBTU = false; // 
uint8_t count = 0; // holds current position of data during scrolling
unsigned long timeTrack = 0; // monitor battery sampling intervals
unsigned long prev_timeTrack = 0; // holds previous time battery level was checked

void setup() {
  SPI.begin();
  Serial.begin(9600);
  
  
  //initialize the display
  tft.begin();
  if (!ts.begin()) {
    Serial.println("Couldn't start touchscreen controller");
    while (1);
  }
  Serial.println("Touchscreen started");
  tft.setRotation(1); // set in landscape mode
  tft.fillScreen(ILI9341_BLACK); // clear tft screen*/
  
  sensorReadingPage(); // display first page on the tft screen

  // set adc cs pin to high 
  pinMode (ADC_CS, OUTPUT);
  digitalWrite (ADC_CS, LOW);
  digitalWrite (ADC_CS, HIGH);
  
 
  delay(3000);
  
  //set timer interrupt for timing measurements
  Timer1.initialize(100000); // initialize timer one
  Timer1.attachInterrupt(incrTimer); // increase timer by 1 every 0.1 seconds
  Timer1.stop(); // stop the timer*/
}



void loop() {
  timeTrack = millis(); // track time
  sensorReadings[0] = analogRead(A0); // get battery voltage
  //check battery voltage evey minute or when the voltage changes
  if (timeTrack - prev_timeTrack >= 60000 || abs(sensorReadings[0] - sensorReadingsPrev[0]) > 15){
     batteryMeter(); // display battery voltage on screen
     sensorReadingsPrev[0] = sensorReadings[0]; //set previous voltage value to current voltage value
     prev_timeTrack = timeTrack; //clear time
  }
  
  readSensors();// read sensor values
  delay(50);
  
  // display BTU calculation results
  if (calculateBTU){
    tft.fillScreen(ILI9341_BLACK);
    page9();
    calculateBTU = false;
  }
  // See if there's any  touch data for us
  if (ts.bufferEmpty()) {
    action = false;
    return;
  }
  // Retrieve a point  
  TS_Point p = ts.getPoint();
  p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
  p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());
  //Serial.print("X = "); Serial.print(p.x);
  //Serial.print("\tY = "); Serial.println(p.y);

  // start the timer one
  if (startTimer){
    EEPROM_readAnything(2008, trig_pr_hi); // get trigger pressure hi value
    Timer1.setPeriod(100000);
    Timer1.start();// start timer
    startTimer = false;
  }
  
  // execute touch command
  if (!action){
    switch (pageNumber){
      case 0: // page 1 touch response
        if (p.y > 66 && p.y < 175 && p.x > 16 && p.x < 66) {
          pageNumber = 1;
          tft.drawRect(93, 190, 145, BOXSIZE,ILI9341_WHITE); // highlight button
          tft.fillScreen(ILI9341_BLACK); // clear screen
          page2(); // open page 2
        }
        break;
      case 1: // page 2 touch response
        if (p.y > 45 && p.y < 190 && p.x > 200 && p.x < 255) {
          tft.drawRect(65, 50, 187, BOXSIZE,ILI9341_WHITE); // highlight button
          tft.fillScreen(ILI9341_BLACK); // clear screen
          page3(); // open page 3
        }
        else if (p.y > 30 && p.y < 200 && p.x > 115 && p.x < 172) {
          tft.drawRect(45, 110, 225, BOXSIZE,ILI9341_WHITE); // highlight button
          tft.fillScreen(ILI9341_BLACK); // clear screen
          page6(); // open page 6
        }
        else if (p.y > 70 && p.y < 157 && p.x > 38 && p.x < 90) {
          tft.drawRect(95, 170, 117, BOXSIZE,ILI9341_WHITE); // highlight button
          tft.fillScreen(ILI9341_BLACK); // clear screen
          page11(); // open page 11
        }
        break;
      case 2: // page 3 touch response
        if (p.y > 190 && p.y < 220 && p.x > 172 && p.x < 225) {
          tft.drawRect(100, 190, 110, BOXSIZE,ILI9341_BLACK);
          tft.drawRect(255, 140, 40, BOXSIZE,ILI9341_BLACK);
          tft.drawRect(255, 70, 40, BOXSIZE,ILI9341_WHITE); // highlight button
          bigButton(80, 115, 175, 0x00,0x00, 0, "");
          increment(&vol, " m3", 80);
        }
        else if (p.y > 190 && p.y < 220 && p.x > 74 && p.x < 134) {
          tft.drawRect(100, 190, 110, BOXSIZE,ILI9341_BLACK);
          tft.drawRect(255, 70, 40, BOXSIZE,ILI9341_BLACK);
          tft.drawRect(255, 140, 40, BOXSIZE,ILI9341_WHITE); // highlight button
          bigButton(80, 115, 175, 0x00,0x00, 0, "");
          decrement(&vol, " m3", 80);
        }
        else if (p.y > 75 && p.y < 160 && p.x > 9 && p.x < 60) {
          tft.drawRect(255, 140, 40, BOXSIZE,ILI9341_BLACK);
          tft.drawRect(255, 70, 40, BOXSIZE,ILI9341_BLACK);
          tft.drawRect(100, 190, 110, BOXSIZE,ILI9341_WHITE); // highlight button
          int n = 0;
          EEPROM_readAnything( 2000, n);
          if(vol != n)EEPROM_writeAnything(2000, vol); // save value in eeprom
          tft.fillScreen(ILI9341_BLACK); // clear screen
          page4(); // open page 4
        }
        break;
      case 3: // page 4 touch response
        if (p.y > 190 && p.y < 220 && p.x > 172 && p.x < 225) {
          tft.drawRect(100, 190, 110, BOXSIZE,ILI9341_BLACK);
          tft.drawRect(255, 140, 40, BOXSIZE,ILI9341_BLACK);
          tft.drawRect(255, 70, 40, BOXSIZE,ILI9341_WHITE); // highlight button
          bigButton(165, 115, 85, 0x00,0x00, 0, "");
          increment(&trig_pr_lo, " Pa", 170);
        }
        else if (p.y > 190 && p.y < 220 && p.x > 74 && p.x < 134) {
          tft.drawRect(100, 190, 110, BOXSIZE,ILI9341_BLACK);
          tft.drawRect(255, 70, 40, BOXSIZE,ILI9341_BLACK);
          tft.drawRect(255, 140, 40, BOXSIZE,ILI9341_WHITE); // highlight button
          bigButton(165, 115, 85, 0x00,0x00, 0, "");
          decrement(&trig_pr_lo, " Pa", 170);
        }
        else if (p.y > 75 && p.y < 160 && p.x > 9 && p.x < 60) {
          tft.drawRect(255, 140, 40, BOXSIZE,ILI9341_BLACK);
          tft.drawRect(255, 70, 40, BOXSIZE,ILI9341_BLACK);
          tft.drawRect(100, 190, 110, BOXSIZE,ILI9341_WHITE); // highlight button
          int n = 0;
          EEPROM_readAnything( 2004, n);
          if(trig_pr_lo != n)EEPROM_writeAnything(2004, trig_pr_lo); // save value in eeprom
          tft.fillScreen(ILI9341_BLACK); // clear screen
          page5(); // open page 5
        }
        break;
      case 4: // page 5 touch response
        if (p.y > 190 && p.y < 220 && p.x > 172 && p.x < 225) {
          tft.drawRect(100, 190, 110, BOXSIZE,ILI9341_BLACK);
          tft.drawRect(255, 140, 40, BOXSIZE,ILI9341_BLACK);
          tft.drawRect(255, 70, 40, BOXSIZE,ILI9341_WHITE);
          bigButton(165, 115, 85, 0x00,0x00, 0, ""); // highlight button
          increment(&trig_pr_hi, " Pa", 170);
        }
        else if (p.y > 190 && p.y < 220 && p.x > 74 && p.x < 134) {
          tft.drawRect(100, 190, 110, BOXSIZE,ILI9341_BLACK);
          tft.drawRect(255, 70, 40, BOXSIZE,ILI9341_BLACK);
          tft.drawRect(255, 140, 40, BOXSIZE,ILI9341_WHITE); // highlight button
          bigButton(165, 115, 85, 0x00,0x00, 0, "");
          decrement(&trig_pr_hi, " Pa", 170);
        }
        else if (p.y > 75 && p.y < 160 && p.x > 9 && p.x < 60) {
          tft.drawRect(255, 140, 40, BOXSIZE,ILI9341_BLACK);
          tft.drawRect(255, 70, 40, BOXSIZE,ILI9341_BLACK);
          tft.drawRect(100, 190, 110, BOXSIZE,ILI9341_WHITE); // highlight button
          int n = 0;
          EEPROM_readAnything( 2008, n);
          if(trig_pr_hi != n)EEPROM_writeAnything(2008, trig_pr_hi); // save value in eeprom
          tft.fillScreen(ILI9341_BLACK); // clear screen
          page6(); // open page 6
        }
        break;
      case 5: // page 6 touch response
        if (p.y > 5 && p.y < 83 && p.x > 15 && p.x < 65) {
          tft.drawRect(10, 190, 100, BOXSIZE,ILI9341_WHITE); // highlight button
          bigButton(225, 100, 95, 0x00,0x00, 0, "");
          EEPROM.write(0, 0); // clear count
          tft.setTextColor(0xFCBA47); // set font to brown
          tft.setCursor(229, 100); // st position of cursor
          tft.print(EEPROM.read(0));
          delay(1000);
          tft.fillScreen(ILI9341_BLACK); // clear screen
          page7(); // open page 7
        }
        else if (p.y > 109 && p.y < 220 && p.x > 15 && p.x < 65) {
          tft.drawRect(150, 190, 150, BOXSIZE,ILI9341_WHITE); // highlight button
          tft.fillScreen(ILI9341_BLACK); // clear screen
          page7(); // open page 7
        }
        break;
      case 6: // page 7 touch response
        if (p.y > 66 && p.y < 175 && p.x > 16 && p.x < 66) {
          tft.drawRect(10, 190, 145, BOXSIZE,ILI9341_WHITE); // highlight button
          
          //indicate that the setup has been armed for measurement
          tft.setTextSize(2); //set font size
          tft.setTextColor(0xFFFFFF); // set font to white
          tft.setCursor(60,80); // st position of cursor
          tft.print("Armed - Start Flow");
          delay(2000);
          EEPROM_readAnything(2004, trig_pr_lo); 
          if(sensorReadings[3] >= trig_pr_lo){
            timer = 0; // initialize timer
            startTimer = true; // get ready to start measurement if presssure is greater than lower limit pressure
            // go to next page
            page8(); // open page 8
          }
          else{ 
            tft.setTextColor(ILI9341_RED); // set font to brown
            tft.setCursor(10,110); // st position of cursor
            tft.print("Pressure below required");
            tft.setCursor(10,130); // st position of cursor
            tft.print("level!!");
          }
        }
        else if (p.y > 172 && p.y < 232 && p.x > 15 && p.x < 65) {
          tft.drawRect(230, 190, 76, BOXSIZE,ILI9341_WHITE); // highlight button
          tft.fillScreen(ILI9341_BLACK); // clear screen
          sensorReadingPage(); // open page 1
        }
        break;
      case 7: // page 8 touch response
        if (p.y > 66 && p.y < 175 && p.x > 16 && p.x < 66) {
          tft.drawRect(93, 190, 145, BOXSIZE,ILI9341_WHITE); // highlight button
          noInterrupts();
          timer = 0; // initialize timer
          Timer1.stop();// stop timer
          startTimer = false;
          interrupts();
          tft.fillScreen(ILI9341_BLACK); // clear screen
          page7(); // load measurement page
        }
        break;
      case 8: // page 9 touch response
        if (p.y > 5 && p.y < 80 && p.x > 15 && p.x < 65) {
          tft.drawRect(10, 190, 100, BOXSIZE,ILI9341_WHITE); // highlight button     
          saveEeprom(); // save parameters in eeprom
          tft.fillScreen(ILI9341_BLACK); // clear screen
          page10(); // open page 10
        }
        else if (p.y > 89 && p.y < 163 && p.x > 15 && p.x < 65) {
          tft.drawRect(120, 190, 100, BOXSIZE,ILI9341_WHITE); // highlight button
          tft.fillScreen(ILI9341_BLACK); // clear screen
          page7(); // open page 7
        }
        else if (p.y > 172 && p.y < 232 && p.x > 15 && p.x < 65) {
          tft.drawRect(230, 190, 76, BOXSIZE,ILI9341_WHITE); // highlight button
          tft.fillScreen(ILI9341_BLACK); // clear screen
          sensorReadingPage(); // open page 1
        }
        break;
      case 9: // page 10 touch response
        if (p.y > 5 && p.y < 75 && p.x > 15 && p.x < 65) {
          tft.drawRect(10, 190, 92, BOXSIZE,ILI9341_WHITE); // highlight button         
          tft.fillScreen(ILI9341_BLACK); // clear screen
          page7(); // open page 7
        }
        else if (p.y > 89 && p.y < 170 && p.x > 15 && p.x < 65) {
          tft.drawRect(122, 190, 105, BOXSIZE,ILI9341_WHITE); // highlight button
          tft.fillScreen(ILI9341_BLACK); // clear screen
          page3(); // open page 3
        }
        else if (p.y > 186 && p.y < 232 && p.x > 15 && p.x < 65) {
          tft.drawRect(250, 190, 58, BOXSIZE,ILI9341_WHITE); // highlight button
          tft.fillScreen(ILI9341_BLACK); // clear screen
          page11(); // open page 4
        }
        break;
      case 10: // page 11 touch response
        if (p.y > 5 && p.y < 75 && p.x > 15 && p.x < 65) {
          tft.drawRect(230, 190, 76, BOXSIZE,ILI9341_BLACK);
          tft.drawRect(122, 190, 92, BOXSIZE,ILI9341_BLACK);
          tft.drawRect(10, 190, 92, BOXSIZE,ILI9341_WHITE); // highlight button         
          previous();
        }
        else if (p.y > 89 && p.y < 160 && p.x > 15 && p.x < 65) {
          tft.drawRect(10, 190, 92, BOXSIZE,ILI9341_BLACK);
          tft.drawRect(230, 190, 76, BOXSIZE,ILI9341_BLACK);
          tft.drawRect(122, 190, 92, BOXSIZE,ILI9341_WHITE); // highlight button
          next();
        }
        else if (p.y > 170 && p.y < 232 && p.x > 15 && p.x < 65) {
          tft.drawRect(10, 190, 92, BOXSIZE,ILI9341_BLACK);
          tft.drawRect(122, 190, 92, BOXSIZE,ILI9341_BLACK);
          tft.drawRect(230, 190, 76, BOXSIZE,ILI9341_WHITE); // highlight button
          tft.fillScreen(ILI9341_BLACK); // clear screen
          sensorReadingPage(); // open page 1
        }
        break;
    }
    action = true;
  }
  interrupts();
}
