/*
Adafruit Arduino - Lesson 13. DC Motor
*/


int motorPin = 3;
int inputPin = 13;
const int BINARY_PWM_LEVEL = 100;
 
void setup() 
{ 
  pinMode(motorPin, OUTPUT);
  pinMode(inputPin, INPUT);
  Serial.begin(115200);
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

void controlFromPin()
{ 
  // If serial is not available, try to read a digital input on pin "inputPin"
  int hasInput = digitalRead(inputPin);
  if (hasInput == HIGH)
  {
    analogWrite(motorPin, BINARY_PWM_LEVEL);
  }
  else
  {
    analogWrite(motorPin, 0);
  }
}

void loop()
{
  if (Serial)
  {
    controlFromSerial();
  }
  else
  {
    controlFromPin();
  }
}

