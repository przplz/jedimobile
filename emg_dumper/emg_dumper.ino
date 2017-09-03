/**
 * EMG Dumper
 * Arduino simple program to send single channel EMG raw data over serial to be used with RealtimePlotter.
 */

// Parameters

const int RIGHT_INPUT_PIN = A0;
const int LEFT_INPUT_PIN = A1;

// Remember that significant EMG activity takes place between 5Hz and 450Hz, so a sampling at 1kHz or higher is appropriate.
// ref: https://www.researchgate.net/post/What_is_the_range_of_Human_EMG_signal_frequencies_Min_and_Max2
const int DELAY = 50; // us (micros) OR ms (millis), depending if delay() or delayMicroseconds() is used later.

const int AVG_SAMPLES = 100; // Number of samples to average.

const int SMOOTHING_SAMPLES = 10;

//
void setup()
{
  // Init input
  pinMode(RIGHT_INPUT_PIN, INPUT);
  pinMode(LEFT_INPUT_PIN, INPUT);
  Serial.begin(115200); // inizializzazione della seriale, manda 115200 byte al secondo
}

void highFreqLoop(int * rightResult, int * leftResult)
{
  int rawR = 0;
  int rawL = 0;
  int minR = 1024;
  int maxR = 0;
  int minL = 1024;
  int maxL = 0;
  for (int i = 0; i < AVG_SAMPLES; ++i)
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
//  Serial.print(rawR);
//  Serial.print(" ");
//  Serial.print(*rightResult);
//  Serial.print(" ");
//  Serial.print(rawL);
//  Serial.print(" ");
//  Serial.print(*leftResult);
//  Serial.print('\r');
}

void loop()
{
  int curR = 0;
  int minR = 1024;
  int maxR = 0;
  int curL = 0;
  int minL = 1024;
  int maxL = 0;
  
  for (int i = 0; i < SMOOTHING_SAMPLES; ++i)
  {
    highFreqLoop(&(curR), &(curL));
    if (curR < minR)
    {
      minR = curR;
    }
    if (curR > maxR)
    {
      maxR = curR;
    }
    if (curL < minL)
    {
      minL = curL;
    }
    if (curL > maxL)
    {
      maxL = curL;
    }
  }
  Serial.print(maxR - minR);
  Serial.print(" ");
  Serial.print(maxL - minL);
  Serial.print('\r');
}
