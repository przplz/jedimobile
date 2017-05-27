# Porting of mindwave.py to Python3

import select, serial, threading, binascii

# Byte codes
CONNECT              = b'\xc0'
DISCONNECT           = b'\xc1'
AUTOCONNECT          = b'\xc2'
SYNC                 = b'\xaa'
EXCODE               = b'\x55'
POOR_SIGNAL          = b'\x02'
ATTENTION            = b'\x04'
MEDITATION           = b'\x05'
BLINK                = b'\x16'
HEADSET_CONNECTED    = b'\xd0'
HEADSET_NOT_FOUND    = b'\xd1'
HEADSET_DISCONNECTED = b'\xd2'
REQUEST_DENIED       = b'\xd3'
STANDBY_SCAN         = b'\xd4'
RAW_VALUE            = b'\x80'

# Status codes
STATUS_CONNECTED     = 'connected'
STATUS_SCANNING      = 'scanning'
STATUS_STANDBY       = 'standby'

def readInt(s):
    return int.from_bytes(s.read(), byteorder='big')

def readIntArray(s, n):
    return [readInt(s) for x in range(n)]

def asInt(b):
    return int.from_bytes(b, byteorder='big')

class Headset(object):
    """
    A MindWave Headset
    """

    class DongleListener(threading.Thread):
        """
        Serial listener for dongle device.
        """
        def __init__(self, headset, debug=False, *args, **kwargs):
            """Set up the listener device."""
            self.headset = headset
            self.debug = debug
            super(Headset.DongleListener, self).__init__(*args, **kwargs)
        
        def run(self):
            """Run the listener thread."""
            s = self.headset.dongle

            # Re-apply settings to ensure packet stream
            s.write(DISCONNECT)
            d = s.getSettingsDict()
            for i in range(2):
                d['rtscts'] = not d['rtscts']
                s.applySettingsDict(d)

            while True:
                # Begin listening for packets
                try:
                    #Debug
                    r1 = s.read()
                    if r1 == SYNC:
                        r2 = s.read()
                    else:
                        r2 = None
                    #print(r1)
                    #print(r2)
                    #
                    if r1 == SYNC and r2 == SYNC:
                        # Packet found, determine plength
                        while True:
                            plength = readInt(s)
                            if plength != 170:
                                break
                        if plength > 170:
                            continue

                        # Read in the payload
                        payload = s.read(plength)
                        #print(payload) #debug

                        # Verify its checksum
                        #print([type(b) for b in payload[:-1]]) #debug
                        val = sum([b for b in payload[:-1]])
                        val &= 0xff
                        val = ~val & 0xff
                        chksum = readInt(s)

                        #if val == chksum:
                        if True: # ignore bad checksums
                            self.parse_payload(payload)
                except (select.error, OSError):
                    #print("Select error or OSError") #debug
                    break
                except serial.SerialException:
                    print("SerialException") #debug
                    s.close()
                    break

        def parse_payload(self, payload):
            """Parse the payload to determine an action."""
            while payload:
                # Parse data row
                excode = 0
                try:
                    code, payload = payload[0], payload[1:]
                    #print(code) #debug
                    #print(type(code)) #debug
                except IndexError:
                    pass
                while code == asInt(EXCODE):
                    # Count excode bytes
                    excode += 1
                    try:
                        code, payload = payload[0], payload[1:]
                    except IndexError:
                        pass
                if code < 0x80:
                    # This is a single-byte code
                    try:
                        value, payload = payload[0], payload[1:]
                    except IndexError:
                        pass
                    if code == asInt(POOR_SIGNAL):
                        # Poor signal
                        self.debug and print("> Poor signal") #debug
                        old_poor_signal = self.headset.poor_signal
                        self.headset.poor_signal = value
                        if self.headset.poor_signal > 0:
                            if old_poor_signal == 0:
                                for handler in \
                                    self.headset.poor_signal_handlers:
                                    handler(self.headset,
                                            self.headset.poor_signal)
                        else:
                            if old_poor_signal > 0:
                                for handler in \
                                    self.headset.good_signal_handlers:
                                    handler(self.headset,
                                            self.headset.poor_signal)
                    elif code == asInt(ATTENTION):
                        # Attention level
                        self.debug and print("> Attention received") #debug
                        self.headset.status = STATUS_CONNECTED
                        self.headset.attention = value
                        for handler in self.headset.attention_handlers:
                            handler(self.headset, self.headset.attention)
                    elif code == asInt(MEDITATION):
                        # Meditation level
                        self.debug and print("> Meditation received") #debug
                        self.headset.meditation = value
                        for handler in self.headset.meditation_handlers:
                            handler(self.headset, self.headset.meditation)
                    elif code == asInt(BLINK):
                        # Blink strength
                        self.debug and print("> Blink received") #debug
                        self.headset.blink = value
                        for handler in self.headset.blink_handlers:
                            handler(self.headset, self.headset.blink)
                else:
                    # This is a multi-byte code
                    try:
                        vlength, payload = payload[0], payload[1:]
                    except IndexError:
                        continue
                    value, payload = payload[:vlength], payload[vlength:]
                    # Multi-byte EEG and Raw Wave codes not included
                    # Raw Value added due to Mindset Communications Protocol
                    if code == asInt(RAW_VALUE):
                        raw=value[0]*256+value[1]
                        if (raw>=32768):
                            raw=raw-65536
                        self.headset.raw_value = raw
                        for handler in self.headset.raw_value_handlers:
                            handler(self.headset, self.headset.raw_value)
                    if code == asInt(HEADSET_CONNECTED):
                        # Headset connect success
                        self.debug and print("> Headset connected") #debug
                        run_handlers = self.headset.status != STATUS_CONNECTED
                        self.headset.status = STATUS_CONNECTED
                        self.headset.headset_id = value.encode('hex')
                        if run_handlers:
                            for handler in \
                                self.headset.headset_connected_handlers:
                                handler(self.headset)
                    elif code == asInt(HEADSET_NOT_FOUND):
                        # Headset not found
                        self.debug and print("> Headset not found") #debug
                        if vlength > 0:
                            not_found_id = value.encode('hex')
                            for handler in \
                                self.headset.headset_notfound_handlers:
                                handler(self.headset, not_found_id)
                        else:
                            for handler in \
                                self.headset.headset_notfound_handlers:
                                handler(self.headset, None)
                    elif code == asInt(HEADSET_DISCONNECTED):
                        # Headset disconnected
                        self.debug and print("> Headset disconnected") #debug
                        headset_id = value.encode('hex')
                        for handler in \
                            self.headset.headset_disconnected_handlers:
                            handler(self.headset, headset_id)
                    elif code == asInt(REQUEST_DENIED):
                        # Request denied
                        self.debug and print("> Request denied") #debug
                        for handler in self.headset.request_denied_handlers:
                            handler(self.headset)
                    elif code == asInt(STANDBY_SCAN):
                        # Standby/Scan mode
                        self.debug and print("> Standby/Scan") #debug
                        try:
                            byte = value[0]
                        except IndexError:
                            byte = None
                        if byte:
                            run_handlers = (self.headset.status !=
                                            STATUS_SCANNING)
                            self.headset.status = STATUS_SCANNING
                            if run_handlers:
                                for handler in self.headset.scanning_handlers:
                                    handler(self.headset)
                        else:
                            run_handlers = (self.headset.status !=
                                            STATUS_STANDBY)
                            self.headset.status = STATUS_STANDBY
                            if run_handlers:
                                for handler in self.headset.standby_handlers:
                                    handler(self.headset)


    def __init__(self, device, headset_id=None, open_serial=True, debug=False):
        """Initialize the  headset."""
        # Initialize headset values
        self.dongle = None
        self.listener = None
        self.device = device
        self.headset_id = headset_id
        self.poor_signal = 255
        self.attention = 0
        self.meditation = 0
        self.blink = 0
        self.raw_value = 0
        self.status = None

        # Debug switch
        self.debug = debug

        # Create event handler lists
        self.poor_signal_handlers = []
        self.good_signal_handlers = []
        self.attention_handlers = []
        self.meditation_handlers = []
        self.blink_handlers = []
        self.raw_value_handlers = []
        self.headset_connected_handlers = []
        self.headset_notfound_handlers = []
        self.headset_disconnected_handlers = []
        self.request_denied_handlers = []
        self.scanning_handlers = []
        self.standby_handlers = []

        # Open the socket
        if open_serial:
            self.serial_open()

    def connect(self, headset_id=None):
        """Connect to the specified headset id."""
        if headset_id:
            self.headset_id = headset_id
        else:
            headset_id = self.headset_id
            if not headset_id:
                self.autoconnect()
                return
        # self.dongle.write(''.join([CONNECT, headset_id.decode('hex')]))
        self.dongle.write(CONNECT + binascii.a2b_hex(headset_id.encode('ascii'))) # To Python3

    def autoconnect(self):
        """Automatically connect device to headset."""
        self.dongle.write(AUTOCONNECT)

    def disconnect(self):
        """Disconnect the device from the headset."""
        self.dongle.write(DISCONNECT)

    def serial_open(self):
        """Open the serial connection and begin listening for data."""
        # Establish serial connection to the dongle
        if not self.dongle or not self.dongle.isOpen():
            self.dongle = serial.Serial(self.device, 115200)

        # Begin listening to the serial device
        if not self.listener or not self.listener.isAlive():
            self.listener = self.DongleListener(self, debug=self.debug)
            self.listener.daemon = True
            self.listener.start()

    def serial_close(self):
        """Close the serial connection."""
        self.dongle.close()
