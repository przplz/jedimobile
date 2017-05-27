import mindwave
import time

_device = '/dev/tty.MindWaveMobile-DevA'
_headset = None #just declaring the var, unnecessary.

#def onBlink(headset, blinkStrength):
 #   print("Blink detected: strength = %d" %(blinkStrength))

def setup():
    _headset = mindwave.Headset(_device, '625f', debug=False) # stiamo dicendo cos'e' l'headset e da quale device prende i valori
    time.sleep(5) # in python sono secondi, serve per aspettare che il MindWave sia pronto
    #  _headset.blink_handlers.append(onBlink)
    _headset.connect()
    return _headset

def reconnect():
    print("Reconnecting...")
    _headset.disconnect()
    #_headset.serial_close()
    #time.sleep(2)
    #_headset.serial_open()
    time.sleep(2)
    _headset.connect()
    time.sleep(1)
    
def loop():
    if (_headset):
        status = _headset.status
        if (status != mindwave.STATUS_CONNECTED):
            print("Not connected... [%s]" %(status))
            reconnect()
        else:
            print("attention = %d, meditation = %d" %(_headset.attention, _headset.meditation))
    else:
        print("No headset..")

if __name__ == '__main__': # se sto eseguendo il programma allora fai la roba sotto, altrimenti no
    try:
        _headset = setup() # occhio al name space! 
        while True:
            loop()
            time.sleep(1)
    except KeyboardInterrupt:
        print("Closing serial and shutting down...")
        _headset.serial_close()

    
#EOF
