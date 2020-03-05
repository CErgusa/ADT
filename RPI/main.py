import GPIO as gpio
import GDL as gdl

from SPI import Spi
from UART import Uart

import RPi.GPIO as GPIO
import time
import serial
import smbus
import atexit
import spidev

IR1 = 0
IR2 = 0
IR3 = 0

Cell1 = 255
Cell2 = 255
Cell3 = 255

spi = Spi()
spi_spi = spi.SPI_getSpi()

uart = Uart()
ser = uart.UART_getSer()

def exit_handler():
    global ser
    global spi_spi

    print("will cancel all SPI actions")
    spi_spi.close()
    print("will cancel all UART actions")
    ser.close()
    print("will set the outputs to 0V")
    print("Goodbye!")
    
atexit.register(exit_handler)

def main():
    global uart
    global spi

    gpio.GPIO_init()

    try:
        while True:
            #SPI_mainloop()
            #GDL_mainloop()
            uart.xbee_mainloop()
        sys.stdout.flush()
    except KeyboardInterrupt():
        exit_handler()
        

main()
