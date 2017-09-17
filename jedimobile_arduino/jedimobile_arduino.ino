/**
 * JediMobile
 * May the force drive your car.
 * Arduino simple program to drive an RC car using EEG+EMG.
 */

#include<SoftwareSerial.h>

// Parameters
const int RIGHT_INPUT_PIN = A0;
const int LEFT_INPUT_PIN = A1;
const int ENGINE_POWER_SWITCH_PIN = 4;
const int RIGHT_STEER_SWITCH_PIN = 7;
const int LEFT_STEER_SWITCH_PIN = 8;
const int SOFT_SERIAL_RX_PIN = 12; // not actually used
const int SOFT_SERIAL_TX_PIN = 13;

const int SAMPLES = 1000; // Number of samples to average. 
const int DELAY = 50; // us (micros)
const int CONCENTRATION_THRESHOLD = 50; // Concentration must be > this to power the engine
const int STEER_THRESHOLD = 30; // threshold to steer

// Variables
int concentration = 0; // input da mindwave
byte telemetry = 0; // cosa sta facendo
char outPacket[2]; // l'array dei 2 byte che rimanda: 1 byte è il valore di mindwave, l'altro è il valore di telemetry

SoftwareSerial BTSerial(SOFT_SERIAL_RX_PIN, SOFT_SERIAL_TX_PIN);

//
void setup()
{
  // Init input
  pinMode(RIGHT_INPUT_PIN, INPUT); // dove mettiamo l'input e da dove lo leggiamo
  pinMode(LEFT_INPUT_PIN, INPUT);
  Serial.begin(9600); // inizializzazione della seriale, manda 9600 byte al secondo
  BTSerial.begin(38400);
  // Init output
  pinMode(ENGINE_POWER_SWITCH_PIN, INPUT);
  pinMode(RIGHT_STEER_SWITCH_PIN, INPUT);
  pinMode(LEFT_STEER_SWITCH_PIN, INPUT);
  // Initialize telemetry 6-th and 7-th bits to be 1 (necessary for checks on the receiver side)
  bitWrite(telemetry, 6, 1);
  bitWrite(telemetry, 7, 1);
}

void highFreqSamplingLoop(int * rightResult, int * leftResult)
{
  int rawR = 0;
  int rawL = 0;
  int minR = 1024;
  int maxR = 0;
  int minL = 1024;
  int maxL = 0;
  for (int i = 0; i < SAMPLES; ++i)
  {
    rawR = analogRead(RIGHT_INPUT_PIN);
    rawL = analogRead(LEFT_INPUT_PIN);
    if (rawR < minR)
    {
      minR = rawR;
    }
    if (rawR > maxR)
    {
      maxR = rawR;
    }
    if (rawL < minL)
    {
      minL = rawL;
    }
    if (rawL > maxL)
    {
      maxL = rawL;
    }
    // Wait before triggering another loop
    delayMicroseconds(DELAY); 
    //delay(DELAY);
  }
  *(rightResult) = maxR - minR;
  *(leftResult) = maxL - minL;
}

void loop()
{
  int rightActivation = 0;
  int leftActivation = 0;
  bool isSteeringRight = false;

  highFreqSamplingLoop(& rightActivation, & leftActivation);
  
  // Read concentration value from serial
  if (Serial.available() > 0) // vede se c'è qualcosa in arrivo da python
  {
    concentration = Serial.read(); // parentesi vuote si usano quando la funzione non ha bisogno di parametri, tipo in questo caso legge il valore di seriale
    BTSerial.write(concentration - 128); // try sending it over Bluetooth
  }
  
  // Decide to brake: if both arms are contracted, the machine will shut engine off (panic button)
  if (rightActivation > STEER_THRESHOLD && leftActivation > STEER_THRESHOLD)
  {
    pinMode(ENGINE_POWER_SWITCH_PIN, INPUT);
    bitWrite(telemetry, 0, 0);
    pinMode(RIGHT_STEER_SWITCH_PIN, INPUT);
    bitWrite(telemetry, 2, 0);
    isSteeringRight = false;
    pinMode(LEFT_STEER_SWITCH_PIN, INPUT);
    bitWrite(telemetry, 1, 0);
  }
  else 
  {
    // Decide wether last read concentration was enough to power the engine ON
    if (concentration > CONCENTRATION_THRESHOLD)
    {
      pinMode(ENGINE_POWER_SWITCH_PIN, OUTPUT);
      digitalWrite(ENGINE_POWER_SWITCH_PIN, HIGH);
      bitWrite(telemetry, 0, 1); // +3 valori: byte su cui scrivo i bit, il bit che voglio scrivere e il valore che deve avere
    }
    else
    {
      pinMode(ENGINE_POWER_SWITCH_PIN, INPUT);
      bitWrite(telemetry, 0, 0);
    }
    
    // Decide about steering right
    if (rightActivation > STEER_THRESHOLD)
    {
      pinMode(RIGHT_STEER_SWITCH_PIN, OUTPUT);
      digitalWrite(RIGHT_STEER_SWITCH_PIN, HIGH);
      bitWrite(telemetry, 2, 1);
      isSteeringRight = true;
    }
    else 
    {
      pinMode(RIGHT_STEER_SWITCH_PIN, INPUT);
      bitWrite(telemetry, 2, 0);
      isSteeringRight = false;
    }

    // Decide about steering left
    if (leftActivation > STEER_THRESHOLD && !isSteeringRight) // Additional condition, if not right steering (since you cannot steer both right and left)
    {
      pinMode(LEFT_STEER_SWITCH_PIN, OUTPUT);
      digitalWrite(LEFT_STEER_SWITCH_PIN, HIGH);
      bitWrite(telemetry, 1, 1);
    }
    else 
    {
      pinMode(LEFT_STEER_SWITCH_PIN, INPUT);
      bitWrite(telemetry, 1, 0);
    }
  }
  outPacket[0] = (char) telemetry;
  outPacket[1] = (char) concentration;
  Serial.write((unsigned char *) outPacket, 2);
}
