
unsigned long previousMillis = 0;        // will store last time LED was updated
// the follow variables is a long because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
unsigned long interval = 700;           // interval at which to blink (milliseconds)

void setup()
{
// pinMode(GREEN_LED, OUTPUT);
// pinMode(RED_LED, OUTPUT);
 P1DIR = 0b01000001; // 0x41 - - BIT6, BIT0
// digitalWrite(GREEN_LED, HIGH);
// digitalWrite(RED_LED, HIGH);
 P1OUT = 0b01000001; // 0x41 - BIT6, BIT0
}

void toggleLeds(){
   P1OUT ^= 0x41;
}

void loop()
{
    unsigned long currentMillis = millis();

     if(currentMillis - previousMillis > interval) {
       // save the last time you blinked the LED
       previousMillis = currentMillis;
       toggleLeds();
     }
}
