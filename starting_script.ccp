void setup()
{
  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(7, INPUT);
  pinMode(6, INPUT);
}

boolean runXTimes = true;
int buttonStateStart = 0;
int buttonStateEnd = 0;

void loop()
{
  buttonStateStart = digitalRead(7);
  buttonStateEnd = digitalRead(6);
  
  //Start Button Pushed
  if (buttonStateStart == HIGH) {
    //Start Signal Phase
      
    //After Reset if Cars are set 
    digitalWrite(8, HIGH);

    //Start Signal
    delay(3000); // Wait for 1s
    digitalWrite(13, HIGH);
    delay(1000); // Wait for 1s
    digitalWrite(12, HIGH);
    delay(1000); // Wait for 1s
    digitalWrite(11, HIGH);
    delay(1000); // Wait for 1s
    digitalWrite(10, HIGH);
    delay(1000); // Wait for 1s
    digitalWrite(9, HIGH);
    delay(1000); // Wait for 1s
    digitalWrite(13, LOW);
    digitalWrite(12, LOW);
    digitalWrite(11, LOW);
    digitalWrite(10, LOW);
    digitalWrite(9, LOW);
    digitalWrite(8, LOW);
  }
  
  
  //Chose Distance via Poti
  
  //Start Timing & Lap Counter
  	//Write Lap Time to Table for each Car
  	//Write Lap Number to Table for each Car
  	//Calculate Mean Time and Mean Speed
  	//Export Data
  
  //Last Round or Stop Button pushed
  if (buttonStateEnd == HIGH) {
    //Set LEDs to Orange
    digitalWrite(8, HIGH);

    //Count Last Lap
    delay(3000); // Wait for 3s

    //Set LEDs to RED/STOP
    digitalWrite(13, HIGH);
    digitalWrite(12, HIGH);
    digitalWrite(11, HIGH);
    digitalWrite(10, HIGH);
    digitalWrite(9, HIGH);

    //Reset to Beginning after x Seconds
    delay(30000); // Wait for 30s
    digitalWrite(13, LOW);
    digitalWrite(12, LOW);
    digitalWrite(11, LOW);
    digitalWrite(10, LOW);
    digitalWrite(9, LOW);
    digitalWrite(8, LOW);
  };
  
  //Reset to Beginning if Stop Button Pushed long
  
  //Pause if Start Button is Pushed
  
  
}
