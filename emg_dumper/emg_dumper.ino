/**
 * EMG Dumper
 * Arduino simple program to send single channel EMG raw data over serial to be used with RealtimePlotter.
 */

// Parameters
const int EMG_INPUT_PIN = A1;

const int AVG_SAMPLES = 100; // Number of samples to average.
// Remember that significant EMG activity takes place between 5Hz and 450Hz, so a sampling at 1kHz or higher is appropriate.
// ref: https://www.researchgate.net/post/What_is_the_range_of_Human_EMG_signal_frequencies_Min_and_Max2
const int DELAY = 50; // us (micros) OR ms (millis), depending if delay() or delayMicroseconds() is used later.

// Variables
int counter = 1; // contatore (parte da valore 1)
float contraction = 0;
float voltsum = 0; // somma dei voltaggi per farci la media (parte da valore 0)
float avg = 0; // media dei valori di voltsum
float minC = 5;
float maxC = 0;

//
void setup()
{
  // Init input
  pinMode(EMG_INPUT_PIN, INPUT);
  Serial.begin(115200); // inizializzazione della seriale, manda 115200 byte al secondo
}

void loop()
{
  Serial.print(analogRead(EMG_INPUT_PIN));
  Serial.print('\r');
  // Wait before triggering another loop
  //delayMicroseconds(DELAY);
  delay(DELAY);
}
