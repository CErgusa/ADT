import serial

serialport = serial.Serial("/dev/ttyS0", baudrate=9600, timeout=3.0)

while True:
    serialport.write("\r\nSay something funny")
    rcv = serialport.read(4)
    serialport.write("\r\nYou sent:" + repr(rcv))
