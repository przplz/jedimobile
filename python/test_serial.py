#!/usr/bin/env python3

## prova programmino in python
## vogliamo mandare all'arduino dei valori a caso da 0 a 100 e stampare quello che riceviamo da arduino, sostituisce il mindwave per ora :)

from time import sleep # dal modulo time importo la funzione sleep
import serial # serial: un altro modulo
from random import randrange
from sys import argv, stdout

MAX_DISPLAY_WIDTH = 80

if (len(argv) > 1):
    arduinoSerialPort = argv[1]
else:
    arduinoSerialPort = '/dev/cu.usbmodem1411'

baudRate = 9600
ser = serial.Serial(arduinoSerialPort, baudRate) # Establish the connection on a specific port

def display(text):
    stdout.write('\r')
    for i in range(MAX_DISPLAY_WIDTH):
        stdout.write(' ')
    stdout.write('\r')
    stdout.write(str(text)[:MAX_DISPLAY_WIDTH])
    stdout.flush()

while True:
    rand = randrange(0,101,1)
    #print('writing random value: %d' %rand)
    # Converting to char since we want to send the number in a single byte,
    # otherwise we get to read 1,2 or 3 char digits on the other end of the
    # serial, depending on the number sent.
    sent = ser.write(chr(rand).encode(encoding='ascii'))
    #
    if (sent > 0):
        recvValue = ord(ser.read(sent))
        if (recvValue == rand):
            display("Ok! Sent and recv values match! :)\t(%3d)" %(recvValue))
        else:
            display("FAIL! Sent-recv value mismatch: %03d != %03d\n" %(rand, recvValue))
    else:
        display("Error, unable to send (sent=%d)" %sent)
    sleep(0.001)
