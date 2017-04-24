#!/usr/bin/env python3

from time import sleep, time # dal modulo time importo la funzione sleep
import mindwave
import serial # serial: un altro modulo
from random import randrange
from sys import argv, stdout

#
LOOP_DELAY_SEC = 0.01 # Seconds
INPUT_PACKET_SIZE = 2 # Bytes
#

ARDUINO_DEFAULT_DEVICE = '/dev/cu.usbmodem1411'
MINDWAVE_DEFAULT_DEVICE = '/dev/tty.MindWaveMobile-DevA'

# Protocol specification:
# - byte 0:
#     *0 = engineOnOffFlag
#     *1 = steeringLeftFlag
#     *2 = steeringRightFlag
#     *3 = <reserved-for-future-use>
#     *4 = <reserved-for-future-use>
#     *5 = <reserved-for-future-use>
#     *6 = 1 (constant)
#     *7 = 1 (constant)
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
    return (byte >> bitNumber) & 0b1; # shifto del numero di bit che mi servono per avere il bit di interesse nella posizione piu a destra e poi lo prendo come valore (scarto gli altri bit sia a dx che a sx di quello che voglio)

def sendOverSerial(text):
    """
    Sends the given text over the serial connection, converting it to ascii encoding.
    The number of bytes sent is returned. Because python represents the char in UTF8
    """
    return ser.write(text.encode(encoding='ascii')) #cambia l'encoding e da il valore di write all'arduino gia nell'encoding giusto

def isTelemetryByte(byte):
    """
    Checks if the given byte is a telemetry one (returns True) or something else (returns False).
    """
    return (getBit(byte, 6) and getBit(byte, 7))

def loop(ser, headset, display, timeIndicator):
    curTimeIndicator = int(time()) % 10 # Changes when the clock second changes. Time da il tempo in secondi
    #
    # If in simulator mode, generate a random number, else get attention.
    if (headset):
        concentration = headset.attention
    else:
        concentration = randrange(0,101,1)
    # Converting to char since we want to send the number in a single byte,
    # otherwise we get to read 1,2 or 3 char digits on the other end of the
    # serial, depending on the number sent.
    if (curTimeIndicator != timeIndicator):
        sent = sendOverSerial(chr(concentration)) # chr di un numero da il carattere ascii corrispondente a quel numero
    else:
        sent = 0
    #
    
    # Getting telemetry data
    if (ser.in_waiting >= INPUT_PACKET_SIZE):
        incomingPacket = ser.read(INPUT_PACKET_SIZE) # ser.read preleva i byte dal buffer (in questo caso ne prende 2 come definito sopra in input packet size)
        telemetry = incomingPacket[0]
        # Check if we are in sync
        if not isTelemetryByte(telemetry):
            if (ser.in_waiting > 0):
                ser.read(1) # If out of sync, skip 1 byte and go to next loop
            return curTimeIndicator
        #
        # Parsing and displaying telemetry
        isEngineOn = getBit(telemetry, 0)
        isSteeringLeft = getBit(telemetry, 1)
        isSteeringRight = getBit(telemetry, 2)

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
        # Parsing and display the concentration level echoed back
        concentration = int(incomingPacket[1])
        display.append("\t(conc: %3d)" %(concentration))
        display.display()
        #
        ser.reset_input_buffer() # Deliberately dropping data, we want to process at our own pace...

    return curTimeIndicator
    

if __name__ == "__main__":
    if (len(argv) > 1):
        arduinoSerialPort = argv[1]
        if (len(argv) > 2):
            mindwaveSerialPort = argv[2] # "simulator" is an accepted value, which triggers sendin random concentration values
        else:
            mindwaveSerialPort = MINDWAVE_DEFAULT_DEVICE
    else:
        arduinoSerialPort = ARDUINO_DEFAULT_DEVICE

    baudRate = 9600
    ser = serial.Serial(arduinoSerialPort, baudRate) # Establish the connection on a specific port
    if (mindwaveSerialPort == "simulator"):
        headset = None
    else:
        headset = mindwave.Headset(mindwaveSerialPort, '625f')
    display = Display()
    timeIndicator = 0
    while True:
        timeIndicator = loop(ser, headset, display, timeIndicator)
        sleep(LOOP_DELAY_SEC)
