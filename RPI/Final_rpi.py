import RPi.GPIO as GPIO
import time
import atexit
import serial
import smbus
import spidev

GPIO.setwarnings(False)

GPIO.setmode(GPIO.BCM)

senA1 = 0
senB1 = 0
senA2 = 0
senB2 = 0
motorLduty_straight = 70
motorRduty_straight = 70
current_motor_dir = "w"
motor_dir = "w"
motorLR = 0 # 0 is L, 1 is R
IR1 = 0
IR2 = 0
IR3 = 0
Cell1 = 255
Cell2 = 255
Cell3 = 255


def exit_handler():
    print("will cancel all SPI actions")
    spi.close();
    print("will cancel all UART actions")
    ser.close();
    print("will set the outputs to 0V")
    GPIO.output(26, GPIO.LOW)
    GPIO.output(23, GPIO.LOW)
    pwmleft.ChangeDutyCycle(0)
    GPIO.output(18, GPIO.LOW)
    GPIO.output(16, GPIO.LOW)
    pwmright.ChangeDutyCycle(0)
    GPIO.output(12, GPIO.LOW)
    print("Goodbye!")
    
atexit.register(exit_handler)

#####################################
# Motor Control Initialization begin
def hall_sensor_A1(channel):
    global senA1
    #print("Hall sensor A1", senA1)
    senA1 += 1

def hall_sensor_B1(channel):
    global senB1
    #print("Hall sensor B1", senB1)
    senB1 += 1

def hall_sensor_A2(channel):
    global senA2
    #print("Hall sensor A2", senA2)
    senA2 += 1

def hall_sensor_B2(channel):
    global senB2
    #print("Hall sensor B2", senB2)
    senB2 += 1

# H bridges and encoder data gpio
GPIO.setup(23, GPIO.OUT)  #pin 16, digital io left motor HIGH = FORWARD
GPIO.setup(18, GPIO.OUT)  #pin 12, pwm left motor
GPIO.setup(16, GPIO.OUT)  #pin 36, digital io right motor LOW = FORWARD
GPIO.setup(12, GPIO.OUT)  #pin 32, pwm right motor
GPIO.setup(21, GPIO.IN)   #pin 40, hall sensor A1
GPIO.setup(20, GPIO.IN)   #pin 38, hall sensor B1
GPIO.setup(13, GPIO.IN)   #pin 33, hall sensor A2
GPIO.setup(6, GPIO.IN)    #pin 31, hall sensor B2

pwmright = GPIO.PWM(12, 5000) # motor2R
pwmright.start(0)
pwmleft = GPIO.PWM(18, 5000) # motor1L
pwmleft.start(0)
GPIO.output(23, GPIO.HIGH) # motor1L forward
GPIO.output(16, GPIO.LOW)  # motor2R forward

GPIO.add_event_detect(21, GPIO.RISING, callback = hall_sensor_A1)
GPIO.add_event_detect(20, GPIO.RISING, callback = hall_sensor_B1)
GPIO.add_event_detect(13, GPIO.RISING, callback = hall_sensor_A2)
GPIO.add_event_detect(6, GPIO.RISING, callback = hall_sensor_B2)

# Motor Control Initialization end
#####################################



#####################################
# Communication Intialization begin

# General Digital Logic Connections
# Rpi      Tiva
# BCM26 <- GDL[0] <- PC6
# BCM19 <- GDL[1] <- PC7 
# BCM5  <- GDL[2] <- PD6
# BCM0  <- GDL[3] <- PD7
# BCM22 <- GDL[4] <- PF4
GPIO.setup(26, GPIO.IN)
GPIO.setup(19, GPIO.IN)
GPIO.setup(5, GPIO.IN)
GPIO.setup(0, GPIO.IN)
GPIO.setup(22, GPIO.IN)

## UART init begin ##
#def UART_init():
ser = serial.Serial ("/dev/ttyAMA0", 9600, timeout=0.1)    #Open named port
print("Serial information: ")
print(ser)
print("Changing baudrate to 9,600")
ser.baudrate = 9600                     #Set baud rate to 9600
def UART_tx():
    print("Write until acknowledge", "T")
    ser.write("T")

def UART_rx():
    global motor_dir
    motor_dir = ser.read(1)
    #ser.write("K")

def UART_close():
    print("closing serial port")
    ser.close()
## UART init end ##

## SPI init begin ##
spi = spidev.SpiDev()
# (bus, chip select)
spi.open(0, 0) # 0 - bus, 1 - device
# set spi speed and mode
#spi.max_speed_hz = 500000
spi.max_speed_hz = 2000
#spi.mode = 0
## SPI init end ##
# Communication Intialization end
#####################################

##########################################################################
# Motor Funtions begin
##########################################################################
# Motor init
def motor_init():
    global senA1
    global senB1
    global senA2
    global senB2
    global motorLduty_straight
    global motorRduty_straight
    global motorLR
    print("senA1", senA1, "senB1", senB1)
    print("senA2", senA2, "senB2", senB2)
    GPIO.output(23, GPIO.HIGH) # motor1L forward
    GPIO.output(16, GPIO.LOW)  # motor2R forward
    pwmleft.ChangeDutyCycle(motorLduty_straight)
    pwmright.ChangeDutyCycle(motorRduty_straight)    
    time.sleep(2) # time for collecting encoder data
    pwmleft.ChangeDutyCycle(0)
    pwmright.ChangeDutyCycle(0)
    
    print("senA1", senA1, "senB1", senB1)
    print("senA2", senA2, "senB2", senB2)
    if (senA1 > senA2):
        motorLR = 0
        motor_Adjust()
    else:
        motorLR = 1
        motor_Adjust() 
    motorLduty_straight = motorLduty_straight + 20
    motorRduty_straight = motorRduty_straight + 20
    time.sleep(5)
    pwmleft.ChangeDutyCycle(motorLduty_straight)
    pwmright.ChangeDutyCycle(motorRduty_straight)

        
def motor_Adjust():
    global motorLR
    global senA1
    global senA2
    global motorLduty_straight
    global motorRduty_straight
    
    if (motorLR == 0): # motor L
        print("motorL is fast, adjust motorL")
        factor = float(senA2)/float(senA1)
    else: #motor R
        print("motorR is fast, adjust motorR")
        factor = float(senA1)/float(senA2)
            
    while factor < 0.99:
        print("factor", factor)
        if (motorLR == 0): #motor L
            motorLduty_straight = motorLduty_straight * factor
            print("motorL dutycycle", motorLduty_straight)
        elif (motorLR == 1):
            motorRduty_straight = motorRduty_straight * factor
            print("motorR dutycycle", motorLduty_straight)
        time.sleep(1)
        senA1 = 0
        senB1 = 0
        senA2 = 0
        senB2 = 0
        pwmleft.ChangeDutyCycle(motorLduty_straight)
        pwmright.ChangeDutyCycle(motorRduty_straight)
        time.sleep(2) # time for collecting encoder data
        pwmleft.ChangeDutyCycle(0)
        pwmright.ChangeDutyCycle(0)
        print("senA1", senA1, "senB1", senB1)
        print("senA2", senA2, "senB2", senB2)
        if (motorLR == 0): # motor L
            factor = float(senA2)/float(senA1)
        elif (motorLR == 1): # motor R
            factor = float(senA1)/float(senA2)
    # Adjusting Done
    print("Adjusted motorLduty", motorLduty_straight)
    print("Adjusted motorRduty", motorRduty_straight)
    pwmleft.ChangeDutyCycle(0)
    pwmright.ChangeDutyCycle(0)
    
    
        
def motor_control():
    global motor_dir
    global motorLR
    global motorLduty_straight
    global motorRduty_straight
    global IR1
    global IR2
    global IR3
    
    if IR1 > 100 or IR2 > 100 or IR3 > 100:
        print("Stop due to IRs")
        pwmleft.ChangeDutyCycle(0)
        pwmright.ChangeDutyCycle(0)
        time.sleep(1)
        print("Move back")
        GPIO.output(23, GPIO.LOW) # motor1L forward
        GPIO.output(16, GPIO.HIGH)  # motor2R forward
        pwmleft.ChangeDutyCycle(motorLduty_straight)
        pwmright.ChangeDutyCycle(motorRduty_straight)
        time.sleep(1)
        print("Stop moving back")
        pwmleft.ChangeDutyCycle(0)
        pwmright.ChangeDutyCycle(0)
        
    else:
        if motor_dir == "w":
            print("Start!")
            GPIO.output(23, GPIO.HIGH) # motor1L forward
            GPIO.output(16, GPIO.LOW)  # motor2R forward
            pwmleft.ChangeDutyCycle(motorLduty_straight)
            pwmright.ChangeDutyCycle(motorRduty_straight)
            current_motor_dir
        elif motor_dir == "a":
            print("Rotate left")
            GPIO.output(23, GPIO.LOW) # motor1L forward
            GPIO.output(16, GPIO.LOW)  # motor2R forward
            pwmleft.ChangeDutyCycle(motorLduty_straight-20)
            pwmright.ChangeDutyCycle(motorRduty_straight-20)                
        elif motor_dir == "s":
            print("Stop")
            pwmleft.ChangeDutyCycle(0)
            pwmright.ChangeDutyCycle(0)                
        elif motor_dir == "d":
            print("Rotate right")
            GPIO.output(23, GPIO.HIGH) # motor1L forward
            GPIO.output(16, GPIO.HIGH)  # motor2R forward
            pwmleft.ChangeDutyCycle(motorLduty_straight-20)
            pwmright.ChangeDutyCycle(motorRduty_straight-20)                
        elif motor_dir == "q":
            motorLR = 0
            motor_Adjust()                
        elif motor_dir == "e":
            motorLR = 1
            motor_Adjust()                
        elif motor_dir == "r":
            motorLduty_straight = motorLduty_straight * 2
            motorRduty_straight = motorRduty_straight * 2                
        elif motor_dir == "z":
            motorLduty_straight = motorLduty_straight / 2
            motorRduty_straight = motorRduty_straight / 2
    #time.sleep(1)
   
##########################################################################
# Motor Funtions end
##########################################################################


    
##########################################################################
# Communication functions begins
##########################################################################

# SPI
def SPI_mainloop():
    global IR1
    global IR2
    global IR3
    global Cell1
    global Cell2
    global Cell3
    try:
        count = 0
        IRcount = 1
        Cellcount = 1
        while count < 262:
            get = spi.xfer2([0xFF])
            if count < 256:
                print("LIDAR", count/2, hex(get[0]))
            elif count < 259:
                if IRcount == 1:
                    IR1 = get[0]
                    print("IR", IRcount, IR1)
                elif IRcount == 2:
                    IR2 = get[0]
                    print("IR", IRcount, IR2)
                elif IRcount == 3:
                    IR3 = get[0]
                    print("IR", IRcount, IR3)
                IRcount = IRcount + 1
            else:
                if Cellcount == 1:
                    Cell1 = get[0]
                    print("Cell", Cellcount, Cell1)
                elif Cellcount == 2:
                    Cell2 = get[0]
                    print("Cell", Cellcount, Cell2)
                elif Cellcount == 3:
                    Cell3 = get[0]
                    print("Cell", Cellcount, Cell3)
                Cellcount = Cellcount+1
            count = count + 1
        print("LIDAR,IR,Cell done")
    except KeyboardInterrupt:
        spi.close()    
######################################

# GDL
def GDL_mainloop():
    GDL = [0, 0, 0, 0, 0]
    GDL[0] = GPIO.input(26)
    GDL[1] = GPIO.input(19)
    GDL[2] = GPIO.input(5)
    GDL[3] = GPIO.input(0)
    GDL[4] = GPIO.input(22)
    if GDL[4] == 1:
            print("DANGER")
    count = 0;
    while count < 5:
        print("GDL", count, GDL[count])
        count = count + 1
    print("GDL done")

# Xbee
def Xbee_mainloop():
    #UART_tx()
    UART_rx()
    
##########################################################################
# Communication functions ends
##########################################################################

def __init__():
    try:
        global motor_dir
        motor_dir = ser.read(1)
        while motor_dir != "w":
            motor_dir = ser.read(1)
                
        #motor_init()
        while True:
            SPI_mainloop()
            GDL_mainloop()
            Xbee_mainloop()
            motor_control()
        sys.stdout.flush()
    except KeyboardInterrupt():
        exit_handler()
        

__init__()
