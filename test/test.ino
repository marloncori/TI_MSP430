/*
  Blink
  The basic Energia example.
  Turns on an LED on for one second, then off for one second, repeatedly.
  Change the LED define to blink other LEDs.

  Hardware Required:
  * LaunchPad with an LED

  This example code is in the public domain.
*/

// most launchpads have a red LED
#define LED RED_LED

//see pins_energia.h for more LED definitions
#define LED2 GREEN_LED

// the setup routine runs once when you press reset:
void setup() {
  // initialize the digital pin as an output.
  pinMode(LED, OUTPUT);
  pinMode(LED2, OUTPUT);
  digitalWrite(LED2, HIGH);
  digitalWrite(LED, LOW);
}

void blink(){
    digitalWrite(LED, !digitalRead(LED));   // turn the LED on/off (HIGH is the voltage level)
    digitalWrite(LED2, !digitalRead(LED2));
    delay(1000);               // wait for a second

    digitalWrite(LED, !digitalRead(LED));    // turn the LED off by making the voltage LOW
    digitalWrite(LED2, !digitalRead(LED2));    // turn the LED off by making the voltage LOW
    delay(1000);               // wait for a second

}
// the loop routine runs over and over again forever:
void loop() {
   blink();
}

