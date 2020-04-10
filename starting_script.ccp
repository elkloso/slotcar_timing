void setup()
{
  Serial.begin(9600);
  pinMode(13, OUTPUT); 	//LED Red
  pinMode(12, OUTPUT); 	//LED Red
  pinMode(11, OUTPUT);	//LED Red
  pinMode(10, OUTPUT);	//LED Red
  pinMode(9, OUTPUT);	//LED Red
  pinMode(8, OUTPUT);	//LED Orange
  pinMode(7, INPUT);	//Start Button
  pinMode(6, INPUT);	//Reed Car 2
  pinMode(5, INPUT);	//Reed Car 1
  pinMode(A0, INPUT);	//Round Poti 
}

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
int lapsNumOld = 0;
int lapconfig = 0;
int buttonStateSubmit = 0;
int lapsNum = 0;
int remainingLapsOld = 999;
int remainingLaps = 999;
int remainingLapsC1 = 999;
int remainingLapsC2 = 999;
unsigned long startTime;
unsigned long inputTime;
unsigned long nowTimeC1;
unsigned long lapTimeC1;
unsigned long lastTimeC1;
unsigned long nowTimeC2;
unsigned long lapTimeC2;
unsigned long lastTimeC2;
unsigned long inputTimeC1;
unsigned long inputTimeC2;
long lapTime = 0;
long lastTime = 0;
long previousMillis = 0;
long previousMillisC1 = 0;
long previousMillisC2 = 0;
const long trackDistance = 200;
const long interval = 1000; 

void printTime()
{
  unsigned long millisec  = inputTime % 100;
  unsigned long tseconds = lapTime / 1000;
  unsigned long tminutes = tseconds / 60;
  unsigned long seconds = tseconds % 60;
  Serial.print(tminutes);
  Serial.print(":");
  Serial.print(seconds);
  Serial.print(":");
  Serial.println(millisec);
};


void loop()
{

//Setup Phase
  //System Ready
  while(systemReadyMsg == true){
    for (int thisPin = 13; thisPin > 7; thisPin--) {
      digitalWrite(thisPin, LOW);
    };
  	Serial.println("Ready");
    Serial.println("Please Chose Laps and Press Button");
    systemReadyMsg = false;
  };
  
  //Chose Distance via Poti (0-100 Rounds, 5 Steps)
  while(lapSet == false){
    //Debounce Button
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      buttonStateSubmit = digitalRead(7);
      int lapPoti = analogRead(A0);
      int m = map(lapPoti,0,1023,1,1000)/10;
      int lapsNum = ((m +4)/5)*5;
      if(lapsNumOld != lapsNum){
        Serial.print("Laps set: ");
        Serial.println(lapsNum);
        lapsNumOld = lapsNum;
      };
    };
    if ((buttonStateSubmit == HIGH) && (lapSet == false)) {
      lapconfig = lapsNumOld;
      Serial.print("Laps set to: ");
      Serial.println(lapconfig);
      Serial.println("Press Button to Start");
      lapSet = true;
      delay(1500);
    };
  };

  int buttonStateStart = digitalRead(7);
  int car1Reed = digitalRead(5);
  
//Start Signal Sequence
  //Start Button Pushed and Startsequence not completed
  if ((buttonStateStart == HIGH) && (startseq == false) && (lapSet == true)) {
    while(startseq == false){
      //Start Signal
      digitalWrite(8, HIGH);
      delay(3000); // Wait for 3s
      for (int thisPin = 13; thisPin > 8; thisPin--) {
          digitalWrite(thisPin, HIGH);
          delay(1000); // Wait for 1s
      };
      for (int thisPin = 13; thisPin > 7; thisPin--) {
          digitalWrite(thisPin, LOW);
      };

      Serial.println("Start!");
	  startTime = millis();
      lastTime = startTime;
      startseq = true;
      raceMode = true;
      buttonStateStart = 0;
    };
  }; 
  
//Start Timing & Lap Counter
  //If in Racemode
  int lapTimesCar1[lapsNumOld];
  int lapTimeCar1Index = 0;
  int lapTimesCar2[lapsNumOld];
  int lapTimeCar2Index = 0;
  
  while(raceMode == true){  
    int buttonStateStop = digitalRead(7);
    int car1Reed = digitalRead(5);
    int car2Reed = digitalRead(6);
    
    //Count Lap if Reed is triggered
    if((car1Reed == HIGH) && (timingCar1Allowed == true)){
      //Debounce Button
      unsigned long currentMillisC1 = millis();
      if (currentMillisC1 - previousMillisC1 >= interval) {
        //Lap Count
        car1Rounds++;
        Serial.print("Lap: ");
        Serial.println(car1Rounds);
        if(remainingLaps == 1 && car1Rounds > car2Rounds){
          remainingLaps = 0;
        };
        //Lap Time
        nowTimeC1 = millis();
        lapTimeC1 = (nowTimeC1 - lastTime); 
        lastTimeC1 = nowTimeC1; 
        previousMillisC1 = nowTimeC1;
        lapTimesCar1[lapTimeCar1Index++] = lapTimeC1;
        timingCar1Allowed = false;
        Serial.print("Laptime C1: ");
        inputTime = lapTimeC1;
        printTime();
      };
    };
    if((car2Reed == HIGH) && (timingCar2Allowed == true)){
      //Debounce Button
      unsigned long currentMillisC2 = millis();
      if (currentMillisC2 - previousMillisC2 >= interval) {
        //Lap Count
        car2Rounds++;
        Serial.print("Lap: ");
        Serial.println(car2Rounds);
        if(remainingLaps == 1 && car2Rounds > car1Rounds){
          remainingLaps = 0;
        };
        //Lap Time
        nowTimeC2 = millis();
        lapTimeC2 = (nowTimeC2 - lastTimeC2); 
        lastTimeC2 = nowTimeC2; 
        previousMillisC2 = nowTimeC2;
        lapTimesCar2[lapTimeCar2Index++] = lapTimeC2;
        timingCar1Allowed = false;
        Serial.print("Laptime C2: ");
        inputTime = lapTimeC2;
        printTime();
      };
    };
    
    //Round doesn't count if a car stand still on Start
    if((car1Reed == LOW) && (timingCar1Allowed == false)){
      timingCar1Allowed = true;
    };
    if((car2Reed == LOW) && (timingCar2Allowed == false)){
      timingCar2Allowed = true;
    };
 
    //Remaining Laps Calculation (Anpassen letzte Runde)   
    if(remainingLaps > 1){
      if(car1Rounds > car2Rounds){
        remainingLaps = lapconfig - car1Rounds;
      }else{
        remainingLaps = lapconfig - car2Rounds;
      };
     };
    
    if(remainingLaps < remainingLapsOld){
      	Serial.print("Remaining Laps: ");
    	Serial.println(remainingLaps);
      	remainingLapsOld = remainingLaps;
    };
      
    //Last Lap or Start Button pushed again
    if((buttonStateStop == HIGH && finishMode == false) || (remainingLaps == 1 && finishMode == false)){
      finishMode = true; 
      
      //Set LEDs to Orange
      digitalWrite(8, HIGH);
      
      //Set Remaining Laps to 1
      remainingLaps = 1;
    };
       
    //Last Lap is Completed
  	if(remainingLaps == 0){
      
      //Set LEDs to RED/STOP after last lap
      for (int thisPin = 13; thisPin > 8; thisPin--) {
        digitalWrite(thisPin, HIGH);
      };
      
      //Leave Racemode
      raceMode = false;
      resultMode = true;
      buttonStateStop = 0;
    };
  };
  
 
  //Show results
  while(resultMode == true){
    int buttonStateReset = digitalRead(7);
    
    if(resultsShown == false){
      //Total Time
      unsigned long endTime = millis() - startTime;
      inputTime = endTime;
      Serial.print("Total Time: ");
      printTime();

      //Total Laps
      Serial.print("Total Laps: ");
      Serial.print(car1Rounds); 
      Serial.print(" | ");
      Serial.println(car2Rounds);
      
      //Who finished first
      //Which Time Delta
      
      //Fastest Lap (ANPASSEN)
      Serial.println(lapTimesCar1[0]);
      
      //Calculate Mean Time and Mean Speed
      
      //Restart
      Serial.print("Press Button to Reset: ");
      resultsShown = true;
    };
    
    //Reset to Beginning after Button is pushed another time
    if(buttonStateReset == HIGH){
      Serial.println("Reset"); 
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
      previousMillis = 0;
      resultMode = false;
      delay(1000);
    };    
  };
  //Display Output
  //Connect to an Live App
};
