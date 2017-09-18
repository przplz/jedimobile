/**
 * EMG Dumper
 * Arduino simple program to send single channel EMG raw data over serial to be used with RealtimePlotter.
 */

// Parameters

const int RIGHT_INPUT_PIN = A0;
const int LEFT_INPUT_PIN = A1;

// Remember that significant EMG activity takes place between 5Hz and 450Hz, so a sampling at 1kHz or higher is appropriate.
// ref: https://www.researchgate.net/post/What_is_the_range_of_Human_EMG_signal_frequencies_Min_and_Max2
const int DELAY = 100; // us (micros) OR ms (millis), depending if delay() or delayMicroseconds() is used later.

const int SAMPLES = 500; // Number of samples to average.

//
void setup()
{
  // Init input
  pinMode(RIGHT_INPUT_PIN, INPUT);
  pinMode(LEFT_INPUT_PIN, INPUT);
  Serial.begin(115200); // Initialization of Serial communication port, at 115200 baud (symbols per second)
}

void highFreqLoop(int * rightResult, int * leftResult)
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
    delayMicroseconds(DELAY); // Wait DELAY microseconds
    //delay(DELAY); // Wait DELAY milliseconds
  }
  *(rightResult) = maxR - minR;
  *(leftResult) = maxL - minL;
}

void loop()
{
  int resR = 0;
  int resL = 0;
  highFreqLoop(&(resR), &(resL));
  Serial.print(resR);
  Serial.print(" ");
  Serial.print(resL);
  Serial.print('\r');  
}
