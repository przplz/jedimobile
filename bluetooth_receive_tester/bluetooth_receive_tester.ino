#include<SoftwareSerial.h>

const int SOFT_SERIAL_RX_PIN = 8;
const int SOFT_SERIAL_TX_PIN = 9; // not actually used

SoftwareSerial BTSerial(SOFT_SERIAL_RX_PIN, SOFT_SERIAL_TX_PIN);

int conc = 0;

void setup()
{
  Serial.begin(38400);
  BTSerial.begin(38400);
}

void loop()
{
  if (BTSerial.available())
  {
    int b = int(BTSerial.read());
    Serial.print(b);
    Serial.print(" - ");
    Serial.println((char) b);
  }
}
