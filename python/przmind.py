import mindwave, time

_device = '/dev/tty.MindWaveMobile-DevA'
_headset = None #just declaring the var, unnecessary.

#def onBlink(headset, blinkStrength):
 #   print("Blink detected: strength = %d" %(blinkStrength))

def setup():
    _headset = mindwave.Headset(_device, '625f') # stiamo dicendo cos'è l'headset e da quale device prende i valori
    time.sleep(2) # in python sono secondi, serve per aspettare che il MindWave sia pronto
  #  _headset.blink_handlers.append(onBlink)
    return _headset
    
def loop():
    if (_headset):
        print("attention = %d, meditation = %d" %(_headset.attention, _headset.meditation))
    else:
        print("No headset..")

if __name__ == '__main__': # se sto eseguendo il programma allora fai la roba sotto, altrimenti no
    _headset = setup() # occhio al name space! 
    while True:
        loop()
        time.sleep(1)
    
#EOF
