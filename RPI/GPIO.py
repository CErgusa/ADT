import RPi.GPIO as GPIO

def GPIO_init():
    GPIO.setwarnings(False)
    GPIO.setmode(GPIO.BCM)

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
