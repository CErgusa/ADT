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
