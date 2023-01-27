#include "Energia.h"

#line 1 "C:/Users/NUC/workspace_v7/leds_btn_counter_2452/leds_btn_counter_2452.ino"



void setup();
void loop();

#line 4
const int buttonPin = PUSH2;     
const int LEDS[2] =  {GREEN_LED, RED_LED};      
const bool STATES[2] =  {LOW, HIGH};      


int buttonState;             
int lastButtonState = LOW;   


static int counter = 0;



long lastDebounceTime = 0;  
long debounceDelay = 50;    

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  for(int i = 0; i < 2; i++){
    pinMode(LEDS[i], OUTPUT);
  }
}

void loop() {
  
  int reading = digitalRead(buttonPin);

  
  
  

  
  if (reading != lastButtonState) {
    
    lastDebounceTime = millis();
    counter++;
  }

  if(counter > 5){
     counter = 0;
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    
    
    buttonState = reading;
  }
  
  switch(counter){
     case 1:
        
       digitalWrite(LEDS[0], buttonState);
       break;

     case 2:
       digitalWrite(LEDS[1], buttonState);
       break;

     case 3:
       digitalWrite(LEDS[0], buttonState);
       digitalWrite(LEDS[1], !buttonState);
       delay(500);
       digitalWrite(LEDS[0], !buttonState);
       digitalWrite(LEDS[1], buttonState);
       delay(500);
       break;

     case 4:
       for(int i = 0; i < 2; i++){
         digitalWrite(LEDS[i], LOW);
       }
       break;

     case 5:
       for(int i = 0; i < 2; i++){
          digitalWrite(LEDS[i], HIGH);
       }
       break;

     default:
       digitalWrite(LEDS[0], STATES[1]);
       digitalWrite(LEDS[1], STATES[0]);
       delay(200);
       digitalWrite(LEDS[0], STATES[0]);
       digitalWrite(LEDS[1], STATES[1]);
       delay(200);
       break;
  }
  
  
  lastButtonState = reading;
}



