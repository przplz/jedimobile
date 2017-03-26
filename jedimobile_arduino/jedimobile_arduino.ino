
int contraction;  // valore della contrazione
int counter = 1; // contatore (parte da valore 1)
float vcontraction = 0;
float voltsum = 0; // somma dei voltaggi per farci la media (parte da valore 0)
float media = 0; // media dei valori di voltsum
float minimum = 5;
float maximum = 0;
float variance = 0;

const int SAMPLES = 200;
const int DELAY = 500; //us
const float MINTHR = 0.8; // threshold minimo
const float MAXTHR = 2; // threshold massimo

void setup() {
  pinMode (A0, INPUT); // dove mettiamo l'input e da dove lo leggiamo
  pinMode (4, OUTPUT);
  pinMode (7, OUTPUT);
  Serial.begin(9600);
  digitalWrite(7, LOW);   
  
}

void loop() {
  //digitalWrite(4, HIGH);
  //digitalWrite(7, LOW);
  //delay(1000);

  //digitalWrite(4, LOW);
  //digitalWrite(7, HIGH);
  //delay(1000);
  
  contraction = analogRead(A0);
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
  
  if (counter == SAMPLES) // comparison
  {
   media = voltsum/SAMPLES;
   if (minimum < MINTHR && maximum > MAXTHR)
   {
    digitalWrite(7, HIGH);    
   }
   else 
   {
    digitalWrite(7, LOW);
   }

   Serial.print("media = "); Serial.print(media);
   Serial.print(", min = "); Serial.print(minimum);
   Serial.print(", max = "); Serial.println(maximum); // 1024 perchè legge a 10 bit
   counter = 0;
   voltsum = 0;
   minimum = 5;
   maximum = 0;
   variance = 0;
  }

  delayMicroseconds(DELAY);
}
