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
  pinMode(6, INPUT);	//End Button
  pinMode(5, INPUT);	//Reed Car 1
  pinMode(A0, INPUT);	//Round Poti
}

boolean runXTimes = true;
boolean startseq = false;
boolean timingRunning = false; 

int car1Rounds = -1;
int car2Rounds = -1;

unsigned long startTime;
unsigned long nowTime;
unsigned long lapTime;
unsigned long lastTime;

unsigned long previousMillis = 0;

const long interval = 1000; 


void loop()
{
  int lapPoti = analogRead(A0);
  int buttonStateStart = digitalRead(7);
  int buttonStateEnd = digitalRead(6);
  int car1Reed = digitalRead(5);
  
//Start Signal Sequence
  //Start Button Pushed and no Timing is running
  if ((buttonStateStart == HIGH) && (timingRunning == false)) {
    //Chose Distance via Poti (0-100 Rounds, 5 Steps)
    int m = map(lapPoti,0,1023,0,1000)/10;
  	int lapconfig = ((m +4)/5)*5;
    Serial.println("Laps set: ");
    Serial.println(lapconfig);
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
    startseq = true;
    startTime = millis();
    lastTime = startTime;
  };
  
//Start Timing & Lap Counter
  //If Startsequence is Completed
  if (startseq == true){  
    //Higher Completed Rounds if Reed is triggered
    if (car1Reed == HIGH){
      //Debounce Button
      unsigned long currentMillis = millis();
      if (currentMillis - previousMillis >= interval) {
        
        timingRunning = true; 
        
        //Lap Count
        car1Rounds++;
        Serial.print("Lap: ");
        Serial.println(car1Rounds);
        
        //Lap Time
        nowTime = millis();
        lapTime = (nowTime - lastTime); 
        lastTime = nowTime; 
        previousMillis = nowTime;
        unsigned long millisec  = lapTime % 100;
        unsigned long tseconds = lapTime / 1000;
        unsigned long tminutes = tseconds / 60;
        unsigned long seconds = tseconds % 60;
        Serial.print("Laptime: ");
        Serial.print(tminutes);
        Serial.print(":");
        Serial.print(seconds);
        Serial.print(":");
        Serial.println(millisec);
      };
    };
  };
  
  //Write Lap Time to Table for each Car
  //Write Lap Number to Table for each Car
  
  //Last Round or Stop Button pushed
  if ((buttonStateEnd == HIGH) || car1Rounds == (lapconfig-1)){
    //Set LEDs to Orange
    digitalWrite(8, HIGH);

    //Count Last Lap & Write Laptimes
    delay(3000); // Wait for 3s
    unsigned long nowTimeCar1 = millis();

    //Set LEDs to RED/STOP after last lap
    if (car1Rounds == (lapconfig+1)){
      for (int thisPin = 13; thisPin > 8; thisPin--) {
        digitalWrite(thisPin, HIGH);
      };

      //Total Time
      unsigned long totalTimeCar1 = (nowTimeCar1 - startTime);
      unsigned long millisec  = totalTimeCar1 % 100;
      unsigned long tseconds = totalTimeCar1 / 1000;
      unsigned long tminutes = tseconds / 60;
      unsigned long seconds = tseconds % 60;
      Serial.print("Total Time Car 1: ");
      Serial.print(tminutes);
      Serial.print(":");
      Serial.print(seconds);
      Serial.print(":");
      Serial.println(millisec);

      //Total Laps
      Serial.print("Total Laps: ");
      Serial.println(car1Rounds); 

      //Reset to Beginning after Button is pushed another time
      delay(30000); // Wait for 30s
      for (int thisPin = 13; thisPin > 7; thisPin--) {
        digitalWrite(thisPin, LOW);
      };
    };
  };
  
  
  
  //Reset to Beginning if Stop Button Pushed long
  
  
  //Who finished first
  //Fastest Lap
  //Which Time Delta
  //Calculate Mean Time and Mean Speed
  
  //Writing Functions
  //Display Output
  //Connect to an Live App
  
}
