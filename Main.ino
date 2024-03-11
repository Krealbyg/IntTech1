/*
  LiquidCrystal Library 
  The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * Variable resistor
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)
*/
/*
TUTORIALS USED IN THE MAKING OF THIS:
https://docs.arduino.cc/built-in-examples/basics/Blink/
https://docs.arduino.cc/built-in-examples/digital/Button/
https://blog.udemy.com/arduino-ldr/
https://docs.arduino.cc/built-in-examples/analog/AnalogInput/
https://github.com/milesburton/Arduino-Temperature-Control-Library/blob/master/examples/Single/Single.ino
https://randomnerdtutorials.com/complete-guide-for-ultrasonic-sensor-hc-sr04/
https://docs.arduino.cc/learn/electronics/lcd-displays/
https://web.archive.org/web/20210511143644/https://tronixstuff.com/2011/01/11/tutorial-using-analog-input-for-multiple-buttons/
*/







// include the library code:
#include <LiquidCrystal.h>
#include "TimerSystemClass.h" // we ended up not using this 
#include <OneWire.h>
#include <DallasTemperature.h>
#include <EEPROM.h>


// Data wire is plugged into port 5 on the Arduino
#define ONE_WIRE_BUS 6
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);



// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


//initialize the buttons and their press states
int buttonArray = 0;

//initialize the menus
int menuStart = 0; // Program states
int menu = 0; // what menu option you want to select
int delayMenu = 0; //What menu option you want to select within the delay menu
int timer = 0; // a timer for the buttons so they dont infinitly press when pressed but have a slight cooldown

//initalize spray stuff
int manSpray = 1; //Delay for the manual spray
int pissSpray =1; // Delay for the number 1 spray
int shitSpray =1; // Delay for the number 2 spray
int standardDelay = 18000; // the delay of the spray itself we cannot change
int toiletTime = 10000; //10 seconds for testing purposes
int toiletMode = 2;  // what the toilet is currently doing, 0 is number 1, 1 is number 2
//temperature stuff
int testTimer = 0; // this has to be a normal timer, for now it is to update the temperature on the lcd screen every 100 ticks, which is arounf 5 ish  Seconds
float tempPrinter =0; // this is what the lcd screen prints as what the temperature is.



//initialize the arrays for the menus, all going to 16 characters to fill up the screen since lcd.Clear() seems to be broken

String menuItems [3] = {"Return          ", "Configure Delays", "Reset Sprays    "};
String sprayDelayOptions [4] = {"Return          " ,"Number 1 config ", "Number 2 config ", "Manual config   " };
String modeTexts [] = {"In progress     ","Cleaning mode   ", "Number 1        ", "Number 2        ", "Unknown usage   "       };
int delayOptions [4] = {15, 20, 25, 30};
int delayCycle = 0;

//initialize the distance sensor
int echoPin = A2;
int triggerPin = 8;
int distance;
long duration;

//Initialize mosfet
int mosfet = 7;


// Initmotionsensor
int motionPut = 0;
int motionPin = A1;

//magnet sensor
int magPin = A3;
int magPut = 0;
int magOpen = 25;

//led
int redLed = 13;
int greenLed = 9;
int blueLed = 10;


//light sensor
int lightPin = A4;
int lightValue = 0;
int lightCurrent = 1000;
int lightThres = 200;
bool lightOn = false;


int amountOfSprays   ;





void setup() {

  //Pinmodes for the rgb LED
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(blueLed, OUTPUT);

  //pinMode button array
  pinMode(A5, INPUT_PULLUP);

  //pinmode for the button
  pinMode (mosfet, OUTPUT);

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  //pinMode motion sensor
  pinMode(motionPin, INPUT);

  //pinMode magnet sensor
  pinMode(magPin, INPUT_PULLUP);

  //pinModes for the distance sensor
  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  //Get the value of the eeprom for the amount of sprays left
  amountOfSprays = EEPROM.get(1, amountOfSprays);

  // Start up the onewire library
  sensors.begin();
}



int readButtons(int pin)
// returns the button number pressed, or zero for none pressed 
// int pin is the analog pin number to read 
{
  int b,c = 0;
  c=analogRead(pin); // get the analog value  
  if (c>990)
  {
    b=0; // buttons have not been pressed
  }   
else
  if (c>350 && c<370)
  {
    b=1; // button 1 pressed
  }     
  else
    if (c>210 && c<230)
    {
      b=2; // button 2 pressed
    }       
    else
      if (c < 25)
      {
        b=3; // button 3 pressed
      }         

return b;
}

void loop() {


  buttonArray = readButtons(A5);
  magPut = analogRead(magPin);
  motionPut = analogRead(motionPut);
  lightValue = analogRead (lightPin); 



  // messure Distance in cm
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(5);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = (duration / 2) / 29.1;




  EEPROM.update(0, amountOfSprays);




  

  if (menuStart == 0) {
    // base case / case 0 is currently just waiting for the select button to be pressed, this has to be changed to showing the correct info
    digitalWrite(blueLed, LOW);
    digitalWrite(redLed, LOW);
    digitalWrite(greenLed, LOW);
    if (timer > 0) {
      timer--;
    } else {
      if (buttonArray == 1) {
        menuStart = 1;
        timer = 100;
      }
    }
      
    if( buttonArray ==3)
    {
      sprayActivate(manSpray);
    }

    

    light(lightValue);
    if (lightOn)
    {
      menuStart = 4;
    }
    //testTimer is to update the temperature sensor.
    testTimer ++;
    tempChecker();

    //Display the current temperature on the idle screen.
    printingTwoLines("Local temp:" + String(tempPrinter)+ " ", "Sprays Left:" + String(amountOfSprays));
    
  } 
  else if (menuStart == 1) { //Main menu state
    //Led states of this program state
    digitalWrite(blueLed, LOW);
    digitalWrite(redLed, HIGH);   
    digitalWrite(greenLed, LOW);
  
    // timer is to make sure that whenever the button is pressed it does not instantly press the next option as well so you can actually see the menu.
    if (timer > 0) {
      timer--;
    } else {
      if (buttonArray == 1) {
        executeAction();
        timer = 100;
      }
    }

    // if the down button is pressed you go down in the list so the number goes up, cannot go above a hardcoded limit, this can be changed when needed.
    if (buttonArray == 2) {
      if (menu == 2) {
         menu = -1;
      } else {
        menu++;
        delay(300);
        
      }
    }

    

    // menu cycles through so you only need 1 button for the scrolling of the menu, if there are more options the limits need to be changed
    if (menu == 2) { 
      
      printingTwoLines(menuItems[menu],menuItems[0] );    
    } else {
      
      printingTwoLines(menuItems[menu],menuItems[menu + 1] );
    }
  } 
  else if (menuStart == 2) { //The delay selection menu
    digitalWrite(blueLed, LOW);
    digitalWrite(redLed, HIGH);
    
    digitalWrite(greenLed, LOW);
  
    if (timer > 0) {
      timer--;
    } else {
      if (buttonArray == 1) {
        executeDelayAction();
        delay(300);
        timer = 200;
      }
    }

    if (buttonArray == 2) {
      if (delayMenu == 3) {
         delayMenu = -1;
      } else {
        delayMenu++;
        delay(300);
      }
    }

    
    if (delayMenu == 3) { 
      
      printingTwoLines(sprayDelayOptions[delayMenu],sprayDelayOptions[0] );   
    } else {
      
      printingTwoLines(sprayDelayOptions[delayMenu],sprayDelayOptions[delayMenu + 1] );
    }
  } 

  
  else if (menuStart == 3) { // the cleaning state
    printingTwoLines(modeTexts[1], modeTexts[0]);
    digitalWrite(blueLed, LOW);
    digitalWrite(redLed, LOW);
    
    digitalWrite(greenLed, HIGH);
    
    if (magPut < magOpen)
    {
      menuStart = 0;
      

    }

    timer = 200; //can be replaced if something else needs to happen during a selection of an option, currently it finishes an action before coming back here
  } 

  else if (menuStart == 4) {// The unknown state state waiting for user input
    digitalWrite(blueLed, HIGH);
    digitalWrite(redLed, LOW);
    
    digitalWrite(greenLed, HIGH);
        
          if (buttonArray == 1) {
            menuStart = 1;
            timer = 100;
          }
        
          
    light(lightValue);
    if(lightOn == false)
    {
      menuStart = 0;
    }
    printingTwoLines(modeTexts[4], modeTexts[0]);
    magnetCheck();
    distanceCheck();
    
    
    
    
    





    timer = 200; //can be replaced if something else needs to happen during a selection of an option, currently it finishes an action before coming back here
  }


  else if (menuStart == 5) { // The user is on the toilet state


    
     //timer needs to countdown from 2 minutes, if those are over and the distance is still less than 100 cm it should go into nr 2 mode
    unsigned long start = millis();


    if (toiletMode == 2)
    {

      digitalWrite(blueLed, HIGH);
      digitalWrite(redLed, LOW);
    
      digitalWrite(greenLed, LOW);

          printingTwoLines(modeTexts[2], modeTexts[0]);
          while (millis() - start < toiletTime)
          {
              if (distance > 30)
              {
                  toiletMode = 1;
                  
                 
              } else if ( distance < 30)
              {
                toiletMode = 0;
              }
          }
    }

    if(toiletMode ==1)
    {

    sprayActivate(pissSpray);
    toiletMode = 2;
    menuStart = 0;
    lightOn = false;
    
    }
   if (toiletMode == 0)
    {

    printingTwoLines(modeTexts[3], modeTexts[0]);
    digitalWrite(blueLed, HIGH);
    digitalWrite(redLed, HIGH);
    
    digitalWrite(greenLed, LOW);
    if (distance > 30)
    {


    sprayActivate(shitSpray);
    sprayActivate(3);
    toiletMode = 2;
    menuStart = 0;
    lightOn = false;
    }
    }
    

    





    timer = 200;
  }

  else if (menuStart == 6) { //choosing delay options within the delay menu state for number 1

    if (buttonArray == 2) {
      if (delayCycle == 3) {
         delayCycle = -1;
      } else {
        delayCycle++;
        delay(300);
      }
    }
    if (delayCycle == 3) { 
      
      printingTwoLines(String(delayOptions[delayCycle]) + " Seconds       " + String (buttonArray),String(delayOptions[0])+ " Seconds       " );   
    } else {
      
      printingTwoLines(String(delayOptions[delayCycle])+ " Seconds       ",String(delayOptions[delayCycle + 1]) + " Seconds       " );   
    }
    
    if (buttonArray == 1 )
    {
      
      pissSpray = delayCycle * 5;
      menuStart = 0;
      

    }
   


    timer = 200; 
  }

  else if (menuStart == 7) {//choosing delay options within the delay menu state for number 2


    if (buttonArray == 2) {
      if (delayCycle == 3) {
         delayCycle = -1;
      } else {
        delayCycle++;
        delay(300);
      }
    }
    if (delayCycle == 3) { 
      
      printingTwoLines(String(delayOptions[delayCycle]) + " Seconds       " + String (buttonArray),String(delayOptions[0])+ " Seconds       " );   
    } else {
      
      printingTwoLines(String(delayOptions[delayCycle])+ " Seconds       ",String(delayOptions[delayCycle + 1]) + " Seconds       " );   
    }
    
    if (buttonArray == 1 )
    {
      
      shitSpray = delayCycle * 5;
      menuStart = 0;
      

    }
  

   
    timer = 200; 
  }

  else if (menuStart == 8) { //choosing delay options within the delay menu state for the manual spray


    if (buttonArray == 2) {
      if (delayCycle == 3) {
         delayCycle = -1;
      } else {
        delayCycle++;
        delay(300);
      }
    }
    if (delayCycle == 3) { 
      
      printingTwoLines(String(delayOptions[delayCycle]) + " Seconds       " + String (buttonArray),String(delayOptions[0])+ " Seconds       " );   
    } else {
      
      printingTwoLines(String(delayOptions[delayCycle])+ " Seconds       ",String(delayOptions[delayCycle + 1]) + " Seconds       " );   
    }
    
    if (buttonArray == 1 )
    {
      
      manSpray = delayCycle * 5;
      menuStart = 0;
      

    }   
    timer = 200; 
  }

  else if (menuStart == 9) {
    timer = 200; 
  }

  else if (menuStart == 10) {
    timer = 200; 
  }






}

//Executes the item in the configure delays menu
void executeDelayAction() {
  switch (delayMenu) {
    case 0:
      menuStart = 1;


      
      break;
    case 1:
      menuStart = 6;
      delayCycle =0;
      timer = 20;
     


      break;
    case 2:
    menuStart = 7;
    delayCycle =0;
    timer = 20;
     
      break;

    case 3:
    menuStart = 8;
    delayCycle =0;
    timer = 20;
     
      break;
    
  }
}


//checks if someone is on the toilet by seeing if the distance is under a set value

void distanceCheck()
{
  if (distance < 30 && distance > 0)
  {
    menuStart = 5;
  }


}
//checks if the magnet sensor is open or closed.
void magnetCheck()
{
   if (magPut > magOpen)
    {
      menuStart = 3;
     
    } 

}

// Each case in the switch decides on a thing that can happen whenever an option is selected in the menu
void executeAction() {
  switch (menu) {
    case 0:
      menuStart = 0;
      break;
    case 1:
     menuStart =2;
     timer = 100;
      break;
    case 2:
      resetSprays();
      menuStart = 0;
      timer = 100;

      break;
    
  }
}
//prints to both lines of the lcd screen
void printingTwoLines(String line1, String line2)
{
      lcd.setCursor(0, 0);
      lcd.print(line1);
      lcd.setCursor(0, 1);
      lcd.print(line2);



}
//checks if a light turns off or on, or if the motion sensor goes off in a highly lit area
void light(int value)
{
    if(lightValue > (lightCurrent + 200) )
    {
        lightOn = true;
    }
    else if (lightValue < 400|| lightValue < (lightCurrent -100))
    {
        lightOn = false;
    }
    else if (lightValue >= 600 && motionPut > 1100)
    {
      lightOn = true;

    }
    lightCurrent = lightValue;

}

//checks the temperature to be displayed on the idle ascreen
void tempChecker()
{
   sensors.requestTemperatures();
    float temperature = sensors.getTempCByIndex(0);
    if (testTimer > 100) {
      tempPrinter = temperature;
      testTimer = 0;
    }
}
//reset the amount of sprays left in a can
void resetSprays()
{
  amountOfSprays = 2400;
  EEPROM.put(1, 2400);


}


//activates the mosfet to spray the air freshener.
void sprayActivate(int delay)
{
  printingTwoLines("Spray incoming!!", "                ");
  
  DelayTimer((delay * 1000));
  digitalWrite(mosfet, HIGH);
  DelayTimer(standardDelay);
  digitalWrite(mosfet, LOW);
  toiletMode = 2;
  amountOfSprays = amountOfSprays -1;
  
  EEPROM.put(1, amountOfSprays);
  


}
  
// a timer
void DelayTimer(unsigned long ms)
{
    unsigned long start = millis();
    while (millis() - start < ms)
    {
        timer = 10;
    }
}
   
