## prova programmino in python
## vogliamo mandare all'arduino dei valori a caso da 0 a 100 e stampare quello che riceviamo da arduino, sostituisce il mindwave per ora :)

from time import sleep # dal modulo time importo la funzione sleep
import serial # serial: un altro modulo
from random import randrange

arduinoSerialPort = '/dev/cu.usbmodem1411'
baudRate = 9600
ser = serial.Serial(arduinoSerialPort, baudRate) # Establish the connection on a specific port

while True:
    rand = randrange(0,101,1)
    print('writing random value: %d' %rand)
    # Converting to char since we want to send the number in a single byte,
    # otherwise we get to read 1,2 or 3 char digits on the other end of the
    # serial, depending on the number sent.
    sent = ser.write(chr(rand))
    #
    if (sent > 0):
        print (ser.read(sent))
    sleep(1)
