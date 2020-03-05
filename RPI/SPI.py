##########################################################################
# SPI communication functions begins
##########################################################################
import spidev

class Spi():
    def __init__(self):
        self.spi = spidev.SpiDev()

        # (bus, chip select)
        self.spi.open(0, 0) # 0 - bus, 1 - device
        # set spi speed and mode

        self.spi.max_speed_hz = 2000
    
    def SPI_getSpi(self):
        return self.spi
        
    # SPI
    def SPI_mainloop(self):
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
                get = self.spi.xfer2([0xFF])
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
            self.spi.close()

##########################################################################
# SPI communication functions ends
##########################################################################
