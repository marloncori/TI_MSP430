#include "Energia.h"

#line 1 "C:/Users/NUC/workspace_v7/register_control/register_control.ino"

void setup();
void toggleLeds();
void loop();

#line 2
unsigned long previousMillis = 0;        


unsigned long interval = 700;           

void setup()
{


 P1DIR = 0b01000001; 


 P1OUT = 0b01000001; 
}

void toggleLeds(){
   P1OUT ^= 0x41;
}

void loop()
{
    unsigned long currentMillis = millis();

     if(currentMillis - previousMillis > interval) {
       
       previousMillis = currentMillis;
       toggleLeds();
     }
}



