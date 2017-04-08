#!/usr/bin/env python3

## prova programmino in python
## vogliamo mandare all'arduino dei valori a caso da 0 a 100 e stampare quello che riceviamo da arduino, sostituisce il mindwave per ora :)

from time import sleep, time # dal modulo time importo la funzione sleep
import serial # serial: un altro modulo
from random import randrange
from sys import argv, stdout

MAX_DISPLAY_WIDTH = 80
#
INPUT_PACKET_SIZE = 2 # Bytes
#
# Protocol specification:
# - byte 0:
#     *0 = engineOnOffFlag
#     *1 = steeringLeftFlag
#     *2 = steeringRightFlag
# - byte 1:
#     Echoed concentration value short integer
#

class Display():
    buf = [] # char list buffer
    cur = 0 # cursor
    toCleanLength = 0

    def append(self, text):
        for c in text:
            if (self.cur >= len(self.buf)):
                self.buf.append(c)
            else:
                self.buf[self.cur] = c
            self.cur += 1

    def carriageReturn(self):
        self.cur = 0

    def clear(self):
        self.toCleanLength = max(self.toCleanLength, len(self.buf))
        self.buf = []
        self.cur = 0

    def display(self):
        stdout.write('\r')
        stdout.write(''.join(self.buf))
        for i in range(self.toCleanLength - len(self.buf)):
            stdout.write(' ')
        self.toCleanLength = 0
        stdout.flush()

def getBit(byte, bitNumber):
    """
    Returns the value of the bitNumber-th bit of the given byte.
    """
    return (byte >> bitNumber) & 0b1;

def sendOverSerial(text):
    """
    Sends the given text over the serial connection, converting it to ascii encoding.
    The number of bytes sent is returned.
    """
    return ser.write(text.encode(encoding='ascii'))

def loop(ser, display, timeIndicator):
    curTimeIndicator = int(time()) % 10 # Changes when the clock second changes
    # Echo test
    #
    rand = randrange(0,101,1)
    # Converting to char since we want to send the number in a single byte,
    # otherwise we get to read 1,2 or 3 char digits on the other end of the
    # serial, depending on the number sent.
    if (curTimeIndicator != timeIndicator):
        sent = sendOverSerial(chr(rand))
    else:
        sent = 0
    #
    
    # Getting telemetry data
    if (ser.in_waiting >= INPUT_PACKET_SIZE):
        inTelemetryPacket = ser.read(INPUT_PACKET_SIZE)
        checkBit = getBit(inTelemetryPacket[0], 7)
        if (checkBit != 0 and ser.in_waiting > 0):
            ser.read(1) # If out of sync, skip 1 byte and go to next loop
            return curTimeIndicator
        isEngineOn = getBit(inTelemetryPacket[0], 0)
        isSteeringLeft = getBit(inTelemetryPacket[0], 1)
        isSteeringRight = getBit(inTelemetryPacket[0], 2)

        display.clear()

        wheel = "|"
        
        if (isSteeringLeft):
            leftArrow = "<--"
            wheel = "\\"
        else:
            leftArrow = "   "

        if (isEngineOn):
            engine = "-(ON)-"
        else:
            engine = "-(  )-"

        if (isSteeringRight):
            rightArrow = "-->"
            wheel = "/"
        else:
            rightArrow = "   "

        display.append(leftArrow)
        display.append(' ')
        display.append(wheel)
        display.append(engine)
        display.append(wheel)
        display.append(' ')
        display.append(rightArrow)
        #
        #print([int(x) for x in inTelemetryPacket]) #debug
        concentration = int(inTelemetryPacket[1])
        display.append("\t(conc: %3d)" %(concentration))
        display.display()
        ser.reset_input_buffer() # Deliberately dropping data, we want to process at our own pace...

    sleep(0.01)
    return curTimeIndicator
    

if __name__ == "__main__":
    if (len(argv) > 1):
        arduinoSerialPort = argv[1]
    else:
        arduinoSerialPort = '/dev/cu.usbmodem1411'

    baudRate = 9600
    ser = serial.Serial(arduinoSerialPort, baudRate) # Establish the connection on a specific port
    display = Display()
    timeIndicator = 0
    while True:
        timeIndicator = loop(ser, display, timeIndicator)
