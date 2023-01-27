#include "Energia.h"

#line 1 "C:/Users/NUC/workspace_v7/test/test.ino"













#define LED RED_LED


#define LED2 GREEN_LED


void setup();
void blink();
void loop();

#line 20
void setup() {
  
  pinMode(LED, OUTPUT);
  pinMode(LED2, OUTPUT);
  digitalWrite(LED2, HIGH);
  digitalWrite(LED, LOW);
}

void blink(){
    digitalWrite(LED, !digitalRead(LED));   
    digitalWrite(LED2, !digitalRead(LED2));
    delay(1000);               

    digitalWrite(LED, !digitalRead(LED));    
    digitalWrite(LED2, !digitalRead(LED2));    
    delay(1000);               

}

void loop() {
   blink();
}




