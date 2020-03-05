from tkgui import *
from tkinter import *

from UART import *
import RPi.GPIO as GPIO
import time
import serial
import smbus
import atexit
import spidev


def exit_handler():
    global ser
    #global spi_spi

    #print("will cancel all SPI actions")
    #spi_spi.close()
    print("will cancel all UART actions")
    ser.close()
    print("will set the outputs to 0V")
    print("Goodbye!")
    
atexit.register(exit_handler)


root, gui = gui_init()
root.after(1000, gui.tkinter_init)  # refresh function for tkinter : after ( ms period for refresh, function to call )
root.mainloop()  # idk, some function needed for tkinter    

# DONE List:
# - 


# TODO List:
# - turn intersectionID into an array for fog of war
#   this way it isn't a shitty update to a variable that literally does nothing
#
# - collision detection
# 
# - 
