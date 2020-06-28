#define _DEBUG_
#define _DISABLE_TLS_

//Include Libraries
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//Define Display
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//Define Input / Output
#define startbtn D7           
#define lappot A0  
#define ledOrange 15           
#define ledRed1 1
#define ledRed2 16
#define ledRed3 0
#define ledRed4 2  
#define reedC1 14
#define reedC2 12

//#########   Setup   #########//
boolean runXTimes = true;
boolean startseq = false;
boolean systemReadyMsg = true;
boolean lapSet = false;
boolean raceMode = false;
boolean resultMode = false;
boolean finishMode = false;
boolean resultsShown = false;
boolean timingCar1Allowed = true;
boolean timingCar2Allowed = true;
int car1Rounds = -1;
int car2Rounds = -1;
int lapsNumOld = 999;
int lapconfig = 0;
int buttonStateSubmit = 0;
int lapsNum = 0;
int remainingLapsOld = 999;
int remainingLaps = 999;
int remainingLapsC1 = 999;
int remainingLapsC2 = 999;
int prntTotalLaps = 0;
int car1RoundsBefore = 999;
int car2RoundsBefore = 999;
unsigned long totalTimeC1 = 0;
unsigned long totalTimeC2 = 0;
unsigned long totalTime = 0;
unsigned long startTime;
unsigned long nowTimeC1;
unsigned long lastTimeC1 = 0;
unsigned long lapTimeC1 = 0;
unsigned long nowTimeC2;
unsigned long lastTimeC2 = 0;
unsigned long lapTimeC2 = 0;
unsigned long lastTime = 0;
unsigned long C1;
unsigned long C2;
unsigned long lapTime;
unsigned long currentMillis;
long previousMillis = 0;
long previousMillis2 = 0;
long previousMillisC1 = 0;
long previousMillisC2 = 0;
const long trackDistance = 200;
const long interval = 1000; 
const long intervalTiming = 4000;
String displayMsg;
String leader;
String lastTrigger;
String winnerCar;
String timeDelta;

//Center Text on Display
void drawCentreString(const String &buf, int x, int y)
{
    //String Height Size 2 = 16px
    //Margin between Strings = 4px
    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds(buf, x, y, &x1, &y1, &w, &h); //calc width of new string
    display.setCursor((SCREEN_WIDTH - w) / 2, y);
    display.println(buf);
}

//Control LEDs for Start Sequence
int ledArray[] = {15,1,16,0,2}; 
const int ledArraySize = sizeof(ledArray)/sizeof(ledArray[0]);
String countdown[5] = { "5", "4", "3", "2", "1"};

//Convert Millis to Time
String printTime(unsigned long inputTime){
  unsigned long millisec  =  inputTime % 100;
  unsigned long tseconds = inputTime / 1000;
  unsigned long tminutes = tseconds / 60;
  unsigned long seconds = tseconds % 60;
  String outputString = String(tminutes) + ":" + String(seconds) + ":" + String(millisec);
  return outputString;
};

void setup() {
  Serial.begin(115200);
  Serial.println("Ready");
  pinMode(lappot, INPUT);  
  pinMode(startbtn, INPUT);  
  pinMode(reedC1, INPUT);  
  pinMode(reedC2, INPUT);  
  pinMode(ledOrange, OUTPUT);
  pinMode(ledRed1, OUTPUT);
  pinMode(ledRed2, OUTPUT);
  pinMode(ledRed3, OUTPUT);
  pinMode(ledRed4, OUTPUT);

  //Initialize Display
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }     

  //Bootscreen
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  drawCentreString("Slotcar", 0, 14);
  drawCentreString("Timing", 0, 34);
  display.display();
}

void loop() {
  while(lapSet == false){
    //Chose Distance via Poti (0-100 Rounds, 5 Steps)
    unsigned long currentMillis = millis();
    buttonStateSubmit = digitalRead(startbtn);
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      int lapPoti = analogRead(lappot);
      int m = map(lapPoti,0,1023,1,1000)/10;
      int lapsNum = ((m +4)/5)*5;
      if(lapsNumOld != lapsNum){
        display.clearDisplay();
        display.setTextSize(1);
        drawCentreString("Choose Laps and", 0, 9);
        drawCentreString("Press Button", 0, 19);
        display.setTextSize(2);
        drawCentreString(("Laps: " + String(lapsNum)), 0, 35);
        display.display();
        lapsNumOld = lapsNum;
      }; 
    };
    
    //Press Button to Start
    unsigned long currentMillis2 = millis();
    if (currentMillis2 - previousMillis2 >= interval) {
      if (buttonStateSubmit == HIGH) {
          previousMillis2 = currentMillis2;
          lapconfig = lapsNumOld;
          display.clearDisplay();
          display.setTextSize(2);
          drawCentreString(("Laps = " + String(lapconfig)), 0, 20);
          display.setTextSize(1);
          drawCentreString("Press Button to Start", 0, 44);
          display.display();
          lapSet = true;
      };
    }; 
    delay(0);
  };

  unsigned long lapTimesCar1[lapsNumOld];
  int lapTimeCar1Index = 0;
  unsigned long lapTimesCar2[lapsNumOld];
  int lapTimeCar2Index = 0;
  
  //Start Signal Sequence
  int buttonStateStart = digitalRead(startbtn);
  unsigned long currentMillis2 = millis();
  if (currentMillis2 - previousMillis2 >= interval) {
    previousMillis2 = currentMillis;
    if ((buttonStateStart == HIGH) && (startseq == false) && (lapSet == true)) {
      while(startseq == false){
        //Start Signal
        digitalWrite(15, HIGH);
        display.clearDisplay();
        display.setTextSize(2);
        drawCentreString("Get Ready!", 0, 24);
        display.display();
        delay(4000); // Wait for 4s
        for (int i=0; i<ledArraySize; i++) {
            digitalWrite(ledArray[i], HIGH);
            display.clearDisplay();
            display.setTextSize(3);
            drawCentreString(String(countdown[i]), 0, 20);
            display.display();
            delay(1000); // Wait for 1s
        };
        for (int i=0; i<ledArraySize; i++) {
            digitalWrite(ledArray[i], LOW);
        };
        display.clearDisplay();
        display.setTextSize(2);
        drawCentreString("START!", 0, 24);
        display.display();
        startTime = millis();
        lastTime = startTime;
        startseq = true;
        raceMode = true;
        buttonStateStart = 0;
      };
    };
  };

  //Start Timing & Lap Counter
  while(raceMode == true){  
    int buttonStateStop = digitalRead(startbtn);
    int car1Reed = digitalRead(reedC1);
    int car2Reed = digitalRead(reedC2);
    
    //Count Lap if Reed is triggered
    if((car1Reed == HIGH) && (timingCar1Allowed == true)){
      unsigned long currentMillisC1 = millis();
      if (currentMillisC1 - lastTimeC1 >= intervalTiming) {
        //Lap Count
        car1Rounds++;
        lastTrigger = "car1";
        if(remainingLaps == 1 && car1Rounds >= car2Rounds){
          remainingLaps = 0;
          car2RoundsBefore = car2Rounds;
        };
        //Lap Time
        lapTimeC1 = (currentMillisC1 - lastTimeC1); 
        lastTimeC1 = currentMillisC1;
        //Don't count start
        if(car1Rounds > 0){
          lapTimesCar1[lapTimeCar1Index++] = lapTimeC1;
        };
        timingCar1Allowed = false;
      };
    };
    if((car2Reed == HIGH) && (timingCar2Allowed == true)){
      unsigned long currentMillisC2 = millis();
      if (currentMillisC2 - lastTimeC2 >= intervalTiming) {
        //Lap Count
        car2Rounds++;
        lastTrigger = "car2";
        if(remainingLaps == 1 && car2Rounds >= car1Rounds){
          remainingLaps = 0;
          car1RoundsBefore = car1Rounds;
        };
        //Lap Time
        lapTimeC2 = (currentMillisC2 - lastTimeC2); 
        lastTimeC2 = currentMillisC2;
        //Don't count start
        if(car2Rounds > 0){
          lapTimesCar2[lapTimeCar2Index++] = lapTimeC2;
        }; 
        timingCar2Allowed = false;
      };
    };
    //Round doesn't count if a car stand still on Start
    if((car1Reed == LOW) && (timingCar1Allowed == false)){
      timingCar1Allowed = true;
    };
    if((car2Reed == LOW) && (timingCar2Allowed == false)){
      timingCar2Allowed = true;
    };
    //Remaining Laps Calculation   
    if(remainingLaps > 1){
      if(car1Rounds > car2Rounds){
        remainingLaps = lapconfig - car1Rounds;
      }else{
        remainingLaps = lapconfig - car2Rounds;
      };
    };
    //Calculate Leading Car 
    if(car1Rounds > car2Rounds){
      leader = "C1";
    }else if(car1Rounds < car2Rounds){
      leader = "C2";
    }else{
      if(lastTrigger == "car2"){
        leader = "C1";
      }else{
        leader = "C2";
      }
    };
    //Update Output
    if((remainingLaps < remainingLapsOld) && (remainingLaps > 1)){
        display.clearDisplay();
        display.setTextSize(2);
        drawCentreString(("Laps: +" + String(remainingLaps)) , 0, 11);
        drawCentreString(("Lead: " + String(leader)) , 0, 37);
        display.display();
        remainingLapsOld = remainingLaps;
    }; 
    //Last Lap or Start Button pushed again
    if((buttonStateStop == HIGH && finishMode == false) || (remainingLaps == 1 && finishMode == false)){
      finishMode = true; 
      //Set LEDs to Orange
      digitalWrite(ledOrange, HIGH);
      display.clearDisplay();
      display.setTextSize(2);
      drawCentreString("LAST LAP!" , 0, 11);
      drawCentreString(("Lead: " + String(leader)) , 0, 37);
      display.display();
      //Set Remaining Laps to 1
      remainingLaps = 1;
    };
    //Last Lap is Completed
    if(remainingLaps == 0){
      //Set LEDs to RED after last lap
      for (int i=0; i<ledArraySize; i++) {
          digitalWrite(ledArray[i], HIGH);
          display.clearDisplay();
          display.setTextSize(1);
          drawCentreString("Waiting For" , 0, 22);
          drawCentreString("Last Car" , 0, 34);
          display.display();
      };
      //Wait for Second Car to Complete Last Lap
      if(leader = "C1"){
        timingCar1Allowed == false;
        if(car2Rounds > car2RoundsBefore){
          raceMode = false;
          resultMode = true;
          buttonStateStop = 0;
        };
      };
      if(leader = "C2"){
        timingCar2Allowed == false;
        if(car1Rounds > car1RoundsBefore){
          raceMode = false;
          resultMode = true;
          buttonStateStop = 0;
        };
      };
      
    };
    delay(0);
  };

  //Show results
  unsigned long minimumLTC1 = lapTimesCar1[0];
  const int lapTimesCar1Size = sizeof(lapTimesCar1)/sizeof(lapTimesCar1[0]);
  unsigned long minimumLTC2 = lapTimesCar2[0];
  const int lapTimesCar2Size = sizeof(lapTimesCar2)/sizeof(lapTimesCar2[0]);
  boolean resultsCalc = false;
  boolean page1 =false;
  boolean page2 =false;
  boolean page3 =false;
  
  while(resultMode == true){
    int buttonStateReset = digitalRead(startbtn);

    //Calculate Results
    if(resultsCalc ==false){
      //Fastest Laps and Sum Laptime
      //Car 1
      for (int i=0; i<car1Rounds; i++) {
        if(lapTimesCar1[i] > 1){
          if (lapTimesCar1[i] < minimumLTC1){
            minimumLTC1 = lapTimesCar1[i];
          };
          totalTimeC1 += lapTimesCar1[i];
        };
      };
      //Car 2
      for (int i=0; i<car2Rounds; i++) {
        if(lapTimesCar2[i] > 1){
          if (lapTimesCar2[i] < minimumLTC2){
            minimumLTC2 = lapTimesCar2[i];
          };
          totalTimeC2 += lapTimesCar2[i];
        };          
      };
      
      //Who finished first with which timedelta and total Laps
      if(car1Rounds > car2Rounds){
        winnerCar = "Car 1";
        unsigned long roundDelta = car1Rounds - car2Rounds;
        timeDelta = "+ " + String(roundDelta) + " Laps";
        totalTime = totalTimeC1;
        prntTotalLaps = car1Rounds;
      }else if(car1Rounds < car2Rounds){
        winnerCar = "Car 2";
        unsigned long roundDelta = car2Rounds - car1Rounds;
        timeDelta = "+ " + String(roundDelta) + " Laps";
        totalTime = totalTimeC2;
        prntTotalLaps = car2Rounds;
      }else{
        prntTotalLaps = car2Rounds;
        if(totalTimeC1 > totalTimeC2){
          winnerCar = "Car 2";
          unsigned long totalTimeDelta = totalTimeC1 - totalTimeC2;
          timeDelta = "+ " + String(printTime(totalTimeDelta));
          totalTime = totalTimeC2;
        }else if(totalTimeC1 < totalTimeC2){
          winnerCar = "Car 1";
          unsigned long totalTimeDelta = totalTimeC2 - totalTimeC1;
          timeDelta = "+ " + String(printTime(totalTimeDelta));
          totalTime = totalTimeC1;
        }else{
          String winnerCar = "Equal";
          timeDelta = "+- 00:00:00";
          totalTime = totalTimeC1;
        } 
      };
      resultsCalc = true;
    };

    //Display Results
    if(page1 == false && page2 == false && page3 == false){
      //Output Page 1
      display.clearDisplay();
      display.setTextSize(1);
      drawCentreString("The Winner is" , 0, 0);
      display.setTextSize(2);
      drawCentreString(String(winnerCar) , 0, 12);
      display.setTextSize(1);
      drawCentreString("Time Delta:" , 0, 36);
      display.setTextSize(2);
      drawCentreString(String(timeDelta) , 0, 48);
      display.display();
      //Next Page
      currentMillis = millis();
      if(buttonStateReset == HIGH){
        if (currentMillis - previousMillis >= interval) {
          page1 = true;
          previousMillis = currentMillis;
       }; 
      };
    };

    if(page1 == true && page2 == false && page3 == false){
      //Output Page 2
      display.clearDisplay();
      display.setTextSize(1);
      drawCentreString("Total Laps:" , 0, 0);
      display.setTextSize(2);
      drawCentreString(String(prntTotalLaps) , 0, 12);
      display.setTextSize(1);
      drawCentreString("Total Time:" , 0, 36);
      display.setTextSize(2);
      drawCentreString(String(printTime(totalTime)) , 0, 48);
      display.display();
      //Next Page
      currentMillis = millis();
      if(buttonStateReset == HIGH){
        if (currentMillis - previousMillis >= interval) {
          page2 = true;
          previousMillis = currentMillis;
        }; 
      };
    };

    if(page1 == true && page2 == true && page3 == false){
      //Output Page 3
      display.clearDisplay();
      display.setTextSize(1);
      drawCentreString("Fastest Lap Car 1:" , 0, 0);
      display.setTextSize(2);
      drawCentreString(String(printTime(minimumLTC1)) , 0, 12);
      display.setTextSize(1);
      drawCentreString("Fastest Lap Car 2:" , 0, 36);
      display.setTextSize(2);
      drawCentreString(String(printTime(minimumLTC2)) , 0, 48);
      display.display();
       //Next Page
      currentMillis = millis();
      if(buttonStateReset == HIGH){
        if (currentMillis - previousMillis >= interval) {
          page3 = true;
          previousMillis = currentMillis;
        }; 
      };
    };
    
    if(page1 == true && page2 == true && page3 == true){
      //Reset Mode
      display.clearDisplay();
      display.setTextSize(1);
      drawCentreString("Press Button", 0, 23);
      drawCentreString("to Reset", 0, 33);
      display.display();
       //Next Page
      currentMillis = millis();
      if(buttonStateReset == HIGH){
        if (currentMillis - previousMillis >= interval) {
          resultsShown = true;
          runXTimes = true;
          startseq = false;
          systemReadyMsg = true;
          lapSet = false;
          raceMode = false;
          finishMode = false;
          resultsShown = false;
          car1Rounds = -1;
          car2Rounds = -1;
          lapsNumOld = 0;
          lapconfig = 0;
          buttonStateSubmit = 0;
          lapsNum = 0;
          remainingLapsOld = 999;
          remainingLaps = 999;
          remainingLapsC1 = 999;
          remainingLapsC2 = 999;
          car1RoundsBefore = 999;
          car2RoundsBefore = 999;
          prntTotalLaps = 0;
          totalTimeC1 = 0;
          totalTimeC2 = 0;
          totalTime = 0;
          startTime = 0;
          nowTimeC1 = 0;
          lastTimeC1 = 0;
          lapTimeC1 = 0;
          nowTimeC2 = 0;
          lastTimeC2 = 0;
          lapTimeC2 = 0;
          lastTime = 0;
          lapTime = 0;
          previousMillis = 0;
          previousMillisC1 = 0;
          previousMillisC2 = 0;
          previousMillis2 = 0;
          timingCar1Allowed = true;
          timingCar2Allowed = true;
          resultMode = false;
        };
      };
    };  
    //Connect to an Live App
    delay(0);
  };
};
