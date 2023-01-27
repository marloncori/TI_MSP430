
// constants won't change. They're used here to
// set pin numbers:
const int buttonPin = PUSH2;     // the number of the pushbutton pin
const int LEDS[2] =  {GREEN_LED, RED_LED};      // the number of the LED pin
const bool STATES[2] =  {LOW, HIGH};      // the number of the LED pin

// Variables will change:
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin

//count button presses
static int counter = 0;

// the following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 50;    // the debounce time; increase if the output flickers

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  for(int i = 0; i < 2; i++){
    pinMode(LEDS[i], OUTPUT);
  }
}

void loop() {
  // read the state of the switch into a local variable:
  int reading = digitalRead(buttonPin);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH),  and you've waited
  // long enough since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
    counter++;
  }

  if(counter > 5){
     counter = 0;
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:
    buttonState = reading;
  }
  
  switch(counter){
     case 1:
        // set the LED using the state of the button:
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
  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState:
  lastButtonState = reading;
}
