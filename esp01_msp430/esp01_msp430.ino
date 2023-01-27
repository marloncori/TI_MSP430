
/* Serial1 corresponds to Esp-01 */
const int BAUD_RATE = 9600;

void setup()
{
  Serial1.begin(BAUD_RATE);
  Serial.begin(BAUD_RATE);
}

void loop()
{
  while(Serial1.available() > 0) {
      Serial.write(Serial1.read());
  }

  while(Serial.available() > 0) {
      Serial1.write(Serial.read());
  }
}
