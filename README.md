# jedimobile
May the force drive your car.

Project for a RC car which:
- Has steering controlled by 2-channel EMG (e.g. left/right arms) using Olimex EMG shields
- Has engine forward triggered by concentration levels as measured by a MindWave device

###### Quick How-To
To run a simple test where concentration levels are simulated:
1. Make sure you have Python 3.* installed (tip: `python -V`)
2. Load the jedimobile_arduino.ino sketch onto your Arduino
3. Run the test_serial.py script, passing the serial device as argument (e.g. `python3 test_serial.py /dev/ttyACM0`)