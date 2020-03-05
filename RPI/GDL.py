import RPi.GPIO as GPIO

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
    count = 0
    while count < 5:
        print("GDL", count, GDL[count])
        count = count + 1
    print("GDL done")
