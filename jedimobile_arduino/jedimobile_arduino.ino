/**
 * JediMobile
 * May the force drive your car.
 * Arduino simple program to drive an RC car using EEG+EMG.
 */

// Parameters
const int RIGHT_HAND_INPUT_PIN = A0;
const int LEFT_HAND_INPUT_PIN = A1;
const int ENGINE_POWER_SWITCH_PIN = 4;
const int RIGHT_STEER_SWITCH_PIN = 7;
const int LEFT_STEER_SWITCH_PIN = 8;

const int AVG_SAMPLES = 200; // Number of samples to average.
const int DELAY = 500; // us (micros)
const int CONCENTRATION_THRESHOLD = 60; // Concentration must be > this to power the engine
const float MIN_STEER_THRESHOLD = 0.8; // threshold minimo
const float MAX_STEER_THRESHOLD = 2; // threshold massimo

// Variables
int rawContraction;  // valore della contrazione
int counter = 1; // contatore (parte da valore 1)
float rContraction = 0;
float lContraction = 0;
float rVoltsum = 0; // somma dei voltaggi per farci la media (parte da valore 0)
float lVoltsum = 0;
float rAvg = 0; // media dei valori di voltsum
float rMin = 5;
float rMax = 0;
float lAvg = 0;
float lMin = 5;
float lMax = 0;
int concentration = 0; // input da mindwave
byte telemetry = 0; // cosa sta facendo
char outPacket[2]; // l'array dei 2 byte che rimanda: 1 byte è il valore di mindwave, l'altro è il valore di telemetry

//
void setup()
{
  // Init input
  pinMode(RIGHT_HAND_INPUT_PIN, INPUT); // dove mettiamo l'input e da dove lo leggiamo
  pinMode(LEFT_HAND_INPUT_PIN, INPUT);
  Serial.begin(9600); // inizializzazione della seriale, manda 9600 byte al secondo
  // Init output
  pinMode(ENGINE_POWER_SWITCH_PIN, OUTPUT);
  pinMode(RIGHT_STEER_SWITCH_PIN, OUTPUT);
  pinMode(LEFT_STEER_SWITCH_PIN, OUTPUT);
  digitalWrite(ENGINE_POWER_SWITCH_PIN, LOW);
  digitalWrite(RIGHT_STEER_SWITCH_PIN, LOW);
  digitalWrite(LEFT_STEER_SWITCH_PIN, LOW);
}

void loop()
{
  // Keeping count of how many times we have performed the loop
  ++counter;         //sommo 1 al counter ad ogni giro
  
  // Read contraction from both arms
  rawContraction = analogRead(RIGHT_HAND_INPUT_PIN);
  rContraction = 5.0*rawContraction/1024;
  rawContraction = analogRead(LEFT_HAND_INPUT_PIN);
  lContraction = 5.0*rawContraction/1024;
  
  // Summing voltages into their accumulators for averages
  rVoltsum += rContraction; //prendi la variabile e somma al valore che già c'è questa operazione
  lVoltsum += lContraction;
  
  // Updating min/max for right arm
  if (rContraction < rMin)
  {
    rMin = rContraction;
  }
  if (rContraction > rMax)
  {
    rMax = rContraction;
  }
  // Updating min/max for left arm
  if (lContraction < lMin)
  {
    lMin = lContraction;
  }
  if (lContraction > lMax)
  {
    lMax = lContraction;
  }
  
  // Read concentration value from serial
  if (Serial.available() > 0) // vede se c'è qualcosa in arrivo da python
  {
    concentration = Serial.read(); // parentesi vuote si usano quando la funzione non ha bisogno di parametri, tipo in questo caso legge il valore di seriale
  }
  
  // Averages/decision computation
  if (counter == AVG_SAMPLES) // comparison: is it time to calculate the averages?
  { 
    // Decide wether last read concentration was enough to power the engine ON
    if (concentration > CONCENTRATION_THRESHOLD)
    {
      digitalWrite(ENGINE_POWER_SWITCH_PIN, HIGH);
      bitWrite(telemetry, 0, 1); // +3 valori: byte su cui scrivo i bit, il bit che voglio scrivere e il valore che deve avere
    }
    else
    {
      digitalWrite(ENGINE_POWER_SWITCH_PIN, LOW);
      bitWrite(telemetry, 0, 0);
    }
    
    // Decide about steering right
    rAvg = rVoltsum/AVG_SAMPLES;
    if (rMin < MIN_STEER_THRESHOLD && rMax > MAX_STEER_THRESHOLD)
    {
      digitalWrite(RIGHT_STEER_SWITCH_PIN, HIGH);
      bitWrite(telemetry, 2, 1);
    }
    else 
    {
      digitalWrite(RIGHT_STEER_SWITCH_PIN, LOW);
      bitWrite(telemetry, 2, 0);
    }

    // Decide about steering left
    lAvg = lVoltsum/AVG_SAMPLES;
    if (lMin < MIN_STEER_THRESHOLD && lMax > MAX_STEER_THRESHOLD
	&& !bitRead(PORTD,RIGHT_STEER_SWITCH_PIN)) // Additional condition, if right steer pin is NOT HIGH (since you cannot steer both right and left)
    {
      digitalWrite(LEFT_STEER_SWITCH_PIN, HIGH);
      bitWrite(telemetry, 1, 1);
    }
    else 
    {
      digitalWrite(LEFT_STEER_SWITCH_PIN, LOW);
      bitWrite(telemetry, 1, 0);
    }
    
    outPacket[0] = (char) telemetry;
    outPacket[1] = (char) concentration;
    Serial.write(outPacket, 2);

    // Resetting counter/avgs/mins/maxs
    counter = 0;
    rVoltsum = 0;
    rMin = 5;
    rMax = 0;
    lVoltsum = 0;
    lMin = 5;
    lMax = 0;
  }
  
  // Wait before triggering another loop
  delayMicroseconds(DELAY);
}
