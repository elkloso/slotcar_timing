# carrera_timing
Arduino - Slotcar Timing Software &amp; Lap Counter


## Idea
The idea was to create a simple timing for slotcar tracks in the hobby area with an Arduino. 

## Parts
* Wemos D1
* 2 Reed Contacts
* 5 LEDs (1 Orange, 4 Red)
* 1 Poti
* 1 Button
* 1 Display I2C

## Function
First, you can chose the amount of rounds, you want to race (0-100) via Poti. 
In the next step, a starting sequence is run. The orange LED is on for 5 seconds, after that the display count backwards from 5 and the red LEDs are flashing. If the race is started all LEDs go off.
The race mode has a lap counter as well as a timing function. 
In the last round (or if the button is pushed) the orange LEDs lights up and after the last round all LEDs lights up and the script waits for the second car to finish its round. 
After that, the results will be shown (e.g. fastest lap, Winner, time,...)

## Known Issues
* Lap count goes crazy, probably because the reed contact is triggered by the electricity of the slotcar track
* Timing is wrong, because the D1 has no RTC
* Last round of second car doesn't end the race if there is still a lap difference between Car 1 and Car 2


## To be added
* Circuit Diagram
* Pictures of my setup
* Step to step tutorial
