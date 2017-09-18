/*
Control a DC motor
 */
#include<SoftwareSerial.h>

const int motorPin = 3;
const int inputPin = 13;
const int leftTurnPin = 11;
const int rightTurnPin = 12;
const int softSerialRxPin = 8; // 16 = MOSI
const int softSerialTxPin = 9; // 15 = SCK, but this is never used
SoftwareSerial BTSerial(softSerialRxPin, softSerialTxPin);

const byte ZERO_BYTE = B0;

boolean isEngineEnabled = true;

void setup() 
{ 
    pinMode(motorPin, OUTPUT);
    pinMode(inputPin, INPUT);
    pinMode(leftTurnPin, OUTPUT);
    digitalWrite(leftTurnPin, LOW);
    pinMode(rightTurnPin, OUTPUT);
    digitalWrite(rightTurnPin, LOW);
    Serial.begin(57600);
    BTSerial.begin(38400);
    BTSerial.listen();
}

int getBit(byte b, int bitNumber)
{
    //Returns the value of the bitNumber-th bit of the given byte.
    return (b >> bitNumber) & 1;
}

boolean isTelemetryByte(byte b)
{
    // Checks if the given byte is a telemetry one (returns True) or something else (returns False).
    return (getBit(b, 6) and getBit(b, 7));
}

void handleTelemetryByte(byte telemetry)
{
    // Parsing telemetry
    int isEngineOn = getBit(telemetry, 0);
    int isSteeringLeft = getBit(telemetry, 1);
    int isSteeringRight = getBit(telemetry, 2);
    if (isEngineOn) 
    {
      Serial.print("E,");
      isEngineEnabled = true;
    }
    else
    {
      Serial.print("-,");
      isEngineEnabled = false;
    }
    if (isSteeringLeft)
    {
      Serial.print("L,");
      digitalWrite(leftTurnPin, HIGH);
    }
    else
    {
      Serial.print("-,");
      digitalWrite(leftTurnPin, LOW);
    }
    if (isSteeringRight)
    {
      Serial.print("R");
      digitalWrite(rightTurnPin, HIGH);
    }
    else
    {
      Serial.print("-");
      digitalWrite(rightTurnPin, LOW);
    }
}

void handleConcentrationByte(byte incomingByte)
{
    int conc = incomingByte & 0x7f;
    int speed = map(conc, 0, 100, 0, 255);
    Serial.print(conc);
    Serial.print(" (");
    Serial.print(conc);
    Serial.print(")");
    Serial.print(" - ");
    Serial.println(speed);
    if (!isEngineEnabled)
    {
      analogWrite(motorPin, 0);
    }
    else if (speed >= 0 
      && speed <= 255)
    {
        analogWrite(motorPin, speed);
    }
}

void controlFromSerial() 
{ 
    if (Serial.available())
    {
        int speed = Serial.parseInt();
        Serial.println(speed);
        if (speed >= 0 && speed <= 255)
        {
            analogWrite(motorPin, speed);
        }
    }
}

void controlFromBTSerial() 
{ 
    if (BTSerial.available() >= 2) // We are waiting for 2 bytes to arrive: type + payload
    {
        int type = BTSerial.read();
        Serial.print((char) type); //debug
        Serial.print(' '); //debug
        Serial.print((int) type); //debug
        byte payload;
        if (type == 'T')
        {
          payload = BTSerial.read();
          Serial.print(payload); //debug
          if (isTelemetryByte(payload))
          {
            Serial.println("\tisTelemetry"); //debug
            handleTelemetryByte(payload);
          }
        }
        else if (type == 'C')
        {
          payload = BTSerial.read();
          Serial.print(payload); //debug
          Serial.println("\tisConcentration"); //debug
          handleConcentrationByte(payload);
        }
    }
}

void loop()
{
    controlFromSerial();
    controlFromBTSerial();
}
