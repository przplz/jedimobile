/*
Control a DC motor
 */
#include<SoftwareSerial.h>

const int motorPin = 3;
const int inputPin = 13;
const int softSerialRxPin = 8; // 16 = MOSI
const int softSerialTxPin = 9; // 15 = SCK, but this is never used
const int BINARY_PWM_LEVEL = 100;
SoftwareSerial BTSerial(softSerialRxPin, softSerialTxPin);

// BT-relatev vars
byte attention = 0;
byte meditation = 0;
boolean bigPacket = false;

void setup() 
{ 
  pinMode(motorPin, OUTPUT);
  pinMode(inputPin, INPUT);
  Serial.begin(9600);
  BTSerial.begin(38400);
  BTSerial.listen();
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

void controlFromBTSerial() 
{ 
  if (BTSerial.available() > 0)
  {
    //processBTData();
    int conc = BTSerial.read() - 128;
    //conc = conc - 96;
    int speed = map(conc, 0, 100, 0, 255);
    Serial.print(conc);
    Serial.print(" (");
    Serial.print(conc);
    Serial.print(")");
    Serial.print(" - ");
    Serial.println(speed);
    if (speed >= 0 && speed <= 255)
    {
      analogWrite(motorPin, speed);
    }
  }
}

void loop()
{
  controlFromSerial();
  controlFromBTSerial();
}

byte ReadOneByte()
{
  int ByteRead;
  while(!BTSerial.available());
  ByteRead = BTSerial.read();
  return ByteRead;
}

//void processBTData() {
//  //Serial.print((char)ReadOneByte());   // echo the same byte out the USB serial (for debug purposes)
//  // checksum variables
//  byte generatedChecksum = 0;
//  byte checksum = 0; 
//  int payloadLength = 0;
//  byte payloadData[64] = {
//    0  };
//  byte poorQuality = 0;
//
//  if(ReadOneByte() == 170 && ReadOneByte() == 170)
//  {
//    payloadLength = ReadOneByte();
//    if(payloadLength > 169)                      //Payload length can not be greater than 169
//    {
//      return;
//    }
//    generatedChecksum = 0;  
//    for(int i = 0; i < payloadLength; i++) 
//    {
//      payloadData[i] = ReadOneByte();            //Read payload into memory
//      generatedChecksum += payloadData[i];
//    }
//    checksum = ReadOneByte();                      //Read checksum byte from stream      
//    generatedChecksum = 255 - generatedChecksum;   //Take one's compliment of generated checksum
//    if(checksum == generatedChecksum)
//    {
//      poorQuality = 200;
//      attention = 0;
//      meditation = 0;
//      for(int i = 0; i < payloadLength; i++) 
//      {                                          // Parse the payload
//        switch (payloadData[i])
//        {
//        case 0x1:
//          i++;
//          // battery = payloadData[i];
//          //bigPacket = true;   
//          break;
//        case 0x2:
//          i++;
//          poorQuality = payloadData[i];
//          bigPacket = true;   
//          break;
//        case 0x4:
//          attention = payloadData[++i];
//          break;
//        case 0x5:
//          meditation = payloadData[++i];
//          break;
//        case 0x83:
//          // ASIC_EEG_POWER: eight big-endian 3-uint8_t unsigned integer values representing delta, theta, low-alpha high-alpha, low-beta, high-beta, low-gamma, and mid-gamma EEG band power values
//          // The next uint8_t sets the length, usually 24 (Eight 24-bit numbers... big endian?)
//          // We dont' use this value so let's skip it and just increment i
//          i++;
//          // Extract the values
//          for (int j = 0; j < 8; j++)
//          {
//            //Serial.print("-->");
//            //Serial.println( ((uint32_t)payloadData[++i] << 16) | ((uint32_t)payloadData[++i] << 8) | (uint32_t)payloadData[++i]);
//            ((uint32_t)payloadData[++i] << 16) | ((uint32_t)payloadData[++i] << 8) | (uint32_t)payloadData[++i];
//          }
//          //hasPower = true;
//          // This seems to happen once during start-up on the force trainer. Strange. Wise to wait a couple of packets before
//          // you start reading.
//          break;
//        case 0x80:
//          // We dont' use this value so let's skip it and just increment i
//          // uint8_t packetLength = payloadData[++i];
//          i++;
//          // Serial.print( ((int)payloadData[++i] << 8) | payloadData[++i]);
//          ((int)payloadData[++i] << 8) | payloadData[++i];
//          break;
//        default:
//          // Broken packet ?
//          break;
//        }
//      } // for loop
//      if(bigPacket) 
//      {
//        Serial.print("PoorQuality: ");
//        Serial.print(poorQuality, DEC);
//        Serial.print(" Attention: ");
//        Serial.print(attention, DEC);
//        Serial.print("\n");
//      }
//      bigPacket = false;
//    } // add else for eerors
//  }
//}

