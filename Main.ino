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
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)

 Library originally added 18 Apr 2008
 by David A. Mellis
 library modified 5 Jul 2009
 by Limor Fried (http://www.ladyada.net)
 example added 9 Jul 2009
 by Tom Igoe
 modified 22 Nov 2010
 by Tom Igoe
 modified 7 Nov 2016
 by Arturo Guadalupi

 This example code is in the public domain.

 https://docs.arduino.cc/learn/electronics/lcd-displays

*/

// include the library code:
#include <LiquidCrystal.h>

#include "TimerSystemClass.h"

#include <OneWire.h>
#include <DallasTemperature.h>


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


// Old button states for 3 buttons, up, down and select
int downstate = 0;
int upstate = 0;
int selectstate = 0;

//initialize the menus
int menuStart = 0; // Program states
int menu = 0; // what menu option you want to select
int delayMenu = 0; //What menu option you want to select within the delay menu
int timer = 0; // a timer for the buttons so they dont infinitly press when pressed but have a slight cooldown

//initalize spray stuff
int manSpray = 1;
int pissSpray =1;
int shitSpray =1;
int standardDelay = 18000;
bool sprayActive = false;
int toiletTime = 120000; //120 seconds
int toiletMode = 0;
//temperature stuff
int testTimer = 0; // this has to be a normal timer, for now it is to update the temperature on the lcd screen every 100 ticks, which is arounf 5 ish  Seconds
float tempPrinter =0; // this is what the lcd screen prints as what the temperature is.



//initialize the arrays for the menus, all going to 16 characters to fill up the screen since lcd.Clear() seems to be broken

String menuItems [3] = {"Return          ", "Configure Delays", "Reset Sprays    "};
String sprayDelayOptions [4] = {"Return          " ,"Number 1 config ", "Number 2 config ", "Manual config   " };
String modeTexts [] = {"In progress     ","Cleaning mode   ", "Number 1        ", "Number 2        ", "Unknown usage   "       };
int delayOptions [4] = {15, 20, 25, 30};
int delayCycle = 0;

//initialize the other components
//int ledPin =13;
int echoPin = 9;
int triggerPin = 8;
int distance;
long duration;
int mosfet = 7;


// Initmotionsensor
int motionPut = 0;
int motionPin = 13;

//magnet sensor
int magPin = A3;
int magPut = 0;
int magOpen = 25;

//light sensor
int lightPin = A4;
int lightValue = 0;
int lightCurrent = 1000;
int lightThres = 200;
bool lightOn = false;
//this needs to be on the non volataile memory:
int amountOfSprays = 4200;




void setup() {

  
  Serial.begin(9600);
  pinMode(A5, INPUT_PULLUP);
  pinMode (mosfet, OUTPUT);
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  //pinMode(ledPin, OUTPUT);
  pinMode(motionPin, INPUT);
  pinMode(magPin, INPUT_PULLUP);

  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
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
  motionPut = digitalRead(motionPut);
  lightValue = analogRead (lightPin); 

  // messure Distance in cm
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(5);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = (duration / 2) / 29.1;


  //------------------------------------------------------------------------------------------------------------------------------------------
  // Some things might wanna be put in different functions and or classes
  // a switch for program states. 
  // idle state - waiting for anything to happen
  // in use state - should display nr 1 and nr 2 with leds -> manual spray button -> all sprays should be configurable delay
  // cleaning state - the system should not spray -> wait for idle to return
  // menu state - all other functionality should halt -> reconfigure spray delay for each time a spray can occur -> reset number of shots in the spray can -> menu closes after a certain idle time
  //------------------------------------------------------------------------------------------------------------------------------------------
  
  //spray delays is delay + 15 at least



  if (menuStart == 0) {
    // base case / case 0 is currently just waiting for the select button to be pressed, this has to be changed to showing the correct info
      
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
    

    //------------------------------------------------------------------------------------------------------------------------------------------
    // temp display constantly updating at least every 2  Seconds
    // check for activation of system by light or motion sensor
    //------------------------------------------------------------------------------------------------------------------------------------------

    testTimer ++;
    tempChecker();
 
    printingTwoLines("Local temp:" + String(tempPrinter)+ " ", "Sprays Left:" + String(amountOfSprays));
    
  } 
  else if (menuStart == 1) {
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
  else if (menuStart == 2) {
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
  else if (menuStart == 3) {
    printingTwoLines(modeTexts[1], modeTexts[0]);
    if (magPut < magOpen)
    {
      menuStart = 0;
      

    }

    timer = 200; //can be replaced if something else needs to happen during a selection of an option, currently it finishes an action before coming back here
  } 
  else if (menuStart == 4) {
    if(!lightOn)
    {
      menuStart = 0;
    }
    printingTwoLines(modeTexts[4], modeTexts[0]);
    magnetCheck();
    
    
    
    
    





    timer = 200; //can be replaced if something else needs to happen during a selection of an option, currently it finishes an action before coming back here
  }
  else if (menuStart == 5) {

    //timer needs to countdown from 2 minutes, if those are over and the distance is still less than 100 cm it should go into nr 2 mode
    unsigned long start = millis();
    while (millis() - start < toiletTime)
    {
        if (distance > 100)
        {
            toiletMode == 1;
            return;
        }
    }

    if(toiletMode ==1)
    {
    printingTwoLines(modeTexts[2], modeTexts[0]);
    DelayTimer(toiletTime);
    }
    else if (toiletMode ==0)
    {
    printingTwoLines(modeTexts[3], modeTexts[0]);
    }

    timer = 200; //can be replaced if something else needs to happen during a selection of an option, currently it finishes an action before coming back here
  }
  else if (menuStart == 6) {

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
   


    timer = 200; //can be replaced if something else needs to happen during a selection of an option, currently it finishes an action before coming back here
  }
  else if (menuStart == 7) {


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
  

   
    timer = 200; //can be replaced if something else needs to happen during a selection of an option, currently it finishes an action before coming back here
  }
  else if (menuStart == 8) {


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
    timer = 200; //can be replaced if something else needs to happen during a selection of an option, currently it finishes an action before coming back here
  }
  else if (menuStart == 9) {
    timer = 200; //can be replaced if something else needs to happen during a selection of an option, currently it finishes an action before coming back here
  }
  else if (menuStart == 10) {
    timer = 200; //can be replaced if something else needs to happen during a selection of an option, currently it finishes an action before coming back here
  }






}


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




void distanceCheck()
{
  if (distance < 100 && distance > 20)
  {
    menuStart = 5;
  }
}
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
      //menuStart = 3;
      lcd.setCursor(0, 0);
      lcd.print("Blinking three ");
      lcd.setCursor(0, 1);
      lcd.print("times green");


      for (int i = 0; i < 3; i++) {
        // digitalWrite(ledPin, HIGH);
        // delay(300);
        // digitalWrite(ledPin, LOW);
        // delay(300);
      }
      menuStart = 0;
      timer = 100;

      break;
    
  }
}
void printingTwoLines(String line1, String line2)
{
      lcd.setCursor(0, 0);
      lcd.print(line1);
      lcd.setCursor(0, 1);
      lcd.print(line2);



}

void light(int value)
{
    if(lightValue > (lightCurrent + 200) )
    {
        lightOn = true;
    }
    else if (lightValue < (lightCurrent - 200))
    {
        lightOn = false;
    }
    lightCurrent = lightValue;

}


void tempChecker()
{
   sensors.requestTemperatures();
    float temperature = sensors.getTempCByIndex(0);
    if (testTimer > 100) {
      tempPrinter = temperature;
      testTimer = 0;
    }
}

void sprayActivate(int delay)
{
  printingTwoLines("Spray incoming!!", "                ");
  
  DelayTimer((delay * 1000));
  digitalWrite(mosfet, HIGH);
  DelayTimer(standardDelay);
  digitalWrite(mosfet, LOW);
  


}
  

void DelayTimer(unsigned long ms)
{
    unsigned long start = millis();
    while (millis() - start < ms)
    {
        timer = 10;
    }
}
   

    

//////----------------------------------
/* doos maken

amount of sprays in right memory

*/ 