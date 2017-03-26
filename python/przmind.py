import mindwave, time

_device = '/dev/tty.MindWaveMobile-DevA'
_headset = None #just declaring the var, unnecessary.

def onBlink(headset, blinkStrength):
    print("Blink detected: strength = %d" %(blinkStrength))

def setup():
    _headset = mindwave.Headset(_device, '625f')
    time.sleep(2)
    _headset.blink_handlers.append(onBlink)
    return _headset
    
def loop():
    if (_headset):
        print("attention = %d, meditation = %d" %(_headset.attention, _headset.meditation))
    else:
        print("No headset..")

if __name__ == '__main__':
    _headset = setup()
    while True:
        loop()
        time.sleep(1)
    
#EOF
