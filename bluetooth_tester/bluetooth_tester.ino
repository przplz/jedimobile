#include<SoftwareSerial.h>

const int SOFT_SERIAL_RX_PIN = 12; // not actually used
const int SOFT_SERIAL_TX_PIN = 13;

SoftwareSerial BTSerial(SOFT_SERIAL_RX_PIN, SOFT_SERIAL_TX_PIN);

int conc = 0;

void setup()
{
  Serial.begin(9600); // inizializzazione della seriale, manda 9600 byte al secondo
  BTSerial.begin(38400);
}

void loop()
{
  if (Serial.available() > 0)
  {
    int incoming = Serial.parseInt();
    BTSerial.write(incoming - 128);
    Serial.println(incoming);
    conc = incoming;
  }
  else
  {
    //conc = (conc + 1) % 101;
    //BTSerial.write(conc);
    //Serial.println(conc);
    //delay(1000);
  }
}
