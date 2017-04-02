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

const int SAMPLES = 200; // Number of samples to average.
const int DELAY = 500; // us (micros)
const float MINTHR = 0.8; // threshold minimo
const float MAXTHR = 2; // threshold massimo

// Variables
int contraction;  // valore della contrazione
int counter = 1; // contatore (parte da valore 1)
float vcontraction = 0;
float voltsum = 0; // somma dei voltaggi per farci la media (parte da valore 0)
float media = 0; // media dei valori di voltsum
float minimum = 5;
float maximum = 0;
float variance = 0;
int concentration = 0; // input da mindwave

//
void setup()
{
  // Init input
  pinMode(RIGHT_HAND_INPUT_PIN, INPUT); // dove mettiamo l'input e da dove lo leggiamo
  pinMode(LEFT_HAND_INPUT_PIN, INPUT);
  Serial.begin(9600);
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
  contraction = analogRead(RIGHT_HAND_INPUT_PIN);
  vcontraction = 5.0*contraction/1024;
  //Serial.println(contraction);
  voltsum += vcontraction; //prendi la variabile e somma al valore che già c'è questa operazione
  
  if (vcontraction < minimum)
  {
    minimum = vcontraction;
  }
  if (vcontraction > maximum)
  {
    maximum = vcontraction;
  }

  ++counter; //sommo 1 al counter ad ogni giro

  // leggiamo da seriale
  if (Serial.available() > 0) 
  {
    concentration = Serial.read(); 
    Serial.print((char) concentration); // echoing the value back for debug purpose
  }
  
  if (counter == SAMPLES) // comparison
  { 
   media = voltsum/SAMPLES;
   if (concentration > 60)
   {
    digitalWrite(ENGINE_POWER_SWITCH_PIN, HIGH);
   }
   else
   {
    digitalWrite(ENGINE_POWER_SWITCH_PIN, LOW);
   }
   
   if (minimum < MINTHR && maximum > MAXTHR)
   {
    digitalWrite(RIGHT_STEER_SWITCH_PIN, HIGH);
   }
   else 
   {
    digitalWrite(RIGHT_STEER_SWITCH_PIN, LOW);
   }

   //Serial.print("media = "); Serial.print(media);
   //Serial.print(", min = "); Serial.print(minimum);
   //Serial.print(", max = "); Serial.println(maximum); // 1024 perchè legge a 10 bit
   counter = 0;
   voltsum = 0;
   minimum = 5;
   maximum = 0;
   variance = 0;
  }

  delayMicroseconds(DELAY);
}
