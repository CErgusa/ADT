import serial

class Uart():
    def __init__(self):
        self.ser = serial.Serial (
            port = "/dev/ttyS0",
            baudrate = 9600,
            parity = serial.PARITY_NONE,
            stopbits = serial.STOPBITS_ONE,
            bytesize = serial.EIGHTBITS,
            timeout = 0) # TODO: should change timeout
        
        print("Serial information: ")
        print(self.ser)
    
    def UART_getSer(self):
        return self.ser
    
    def UART_close(self):
        print("closing serial port")
        self.ser.close()

    def UART_tx(self):
        #print("Got here!")
        self.ser.write("*Packet*")

    def UART_rx(self, num_bytes):
        #print("Got here?")
        farts = self.ser.readline()
        if farts != '':
            print(farts)
        
    def xbee_mainloop(self):
        self.UART_tx()
        self.UART_rx(8)

    def UART_testloop(self):
        self.ser.write("\r\nSay something funny:")
        rcv = self.ser.readline()
        self.ser.write("\r\nou sent: " + repr(rcv))
