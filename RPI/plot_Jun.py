import sys
import RPi.GPIO as GPIO
import spidev
import time
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import struct


# SPI0 ####################################################
# pin19 - BCM10 - SPI0 MOSI
# pin21 - BCM9  - SPI0 MISO
# pin23 - BCM11 - SPI0 SCLK
# pin24 - BCM8  - SPI0 CE0
spi = spidev.SpiDev()
spi.open(0, 0) # SPI0, using the device connected to CE0
#spi.max_speed_hz = 6000000 # 1.9MHz
#spi.max_speed_hz = 5000000 # 5MHz

spi.max_speed_hz = 2000000 # 1.9MHz
print('SPI0 as Master with Clock: ', spi.max_speed_hz)
##########################################################

# GPIO ####################################################
GPIO.setwarnings(False)
GPIO.setmode(GPIO.BCM)
# pin37 - BCM26 - GDL[0] - PC6: RPi ready to read
# pin35 - BCM19 - GDL[1] - PC7
# pin29 - BCM5  - GDL[2] - PD6
# pin27 - BCM0  - GDL[3] - PD7
# pin15 - BCM22 - GDL[4] - PF4
GPIO.setup(26, GPIO.OUT)
#GPIO.setup(26, GPIO.IN)
GPIO.setup(19, GPIO.IN)
GPIO.setup(5, GPIO.IN)
GPIO.setup(0, GPIO.IN)
GPIO.setup(22, GPIO.IN)
#GPIO.add_event_detect(26, GPIO.RISING, callback = Tiva)
print(GPIO)



def exit_handler():
    SPI_ready_to_read(False)
    print("will cancel all SPI actions")
    spi.close()
    print("will set the outputs to 0V")
    print("Goodbye")



def SPI_ready_to_read(ready):
    GPIO.output(26, ready)


def SPI_main_loop():
    SPI_ready_to_read(True)
    get = spi.xfer2([0xAC])
    while get[0] != 0xAC:
        get = spi.xfer2([0xAC])
    SPI_ready_to_read(False)

    #print('Start Reading')
    get = spi.xfer2([0xFF])
    num_total_buffer = get[0] << 8
    get = spi.xfer2([0xFF])
    num_total_buffer |= get[0]

    buffer_ = []
    IR_ = []
    cell_ = []

    count_buffer = 0
    while count_buffer < num_total_buffer:
        get = spi.xfer2([0xFF])
        buffer_.append(get[0])
        count_buffer = count_buffer + 1
    
    count_buffer = 0
    while count_buffer < 6:
        get = spi.xfer2([0xFF])
        if count_buffer < 3: # IR
            IR_.append(get[0])
            #print(IR_[counter_IR_Cell])
        else: # Cell
            cell_.append(get[0])
        count_buffer = count_buffer + 1

    print('# buffer:', hex(len(buffer_)))


    theta_ = []
    theta_binary = [0] * 4
    r_ = []

    count_buffer = 0
    while count_buffer < num_total_buffer:
        #num_sample_packet = buffer_[count_buffer]
        num_buffers_packet = 6 * buffer_[count_buffer]
        size_theta = 4 * buffer_[count_buffer]
        count_buffer = count_buffer + 1
        count_buffer_each_packet = 1

        while count_buffer_each_packet <= num_buffers_packet:
            
            if count_buffer == num_total_buffer: # Odd behavior, exit immediately
                plot_data_new = [[0], [0], [0,0,0], [0,0,0]]
                return plot_data_new 
            
            if count_buffer_each_packet <= size_theta: # [1] ~ [size_theta]
                if (count_buffer_each_packet % 4) == 1: # 
                    theta_binary[0] = buffer_[count_buffer]
                elif (count_buffer_each_packet % 4) == 2:
                    theta_binary[1] = buffer_[count_buffer]
                elif (count_buffer_each_packet % 4) == 3:
                    theta_binary[2] = buffer_[count_buffer]
                else: # counter_samples % 4 = 0
                    theta_binary[3] = buffer_[count_buffer]
                    float_binary_packed = struct.pack(b'BBBB', theta_binary[0], theta_binary[1], theta_binary[2], theta_binary[3])
                    theta_current = struct.unpack('>f', float_binary_packed)[0]
                    #print(hex(theta_binary[0]), hex(theta_binary[1]), hex(theta_binary[2]), hex(theta_binary[3]))
                    theta_.append(theta_current)
            else: # [size_theta+1] ~ [size_buffer]
                if (count_buffer_each_packet % 2) == 1:
                    r_current = buffer_[count_buffer] << 8
                else:
                    r_current |= buffer_[count_buffer]
                    #if counter_buffer == size_theta+2:
                    #print(hex(r_current))
                    r_.append(r_current)
            count_buffer_each_packet = count_buffer_each_packet + 1
            count_buffer = count_buffer + 1

    plot_data_new = [theta_, r_, IR_, cell_]
    return plot_data_new

def plot_init():

    plot_refs = [0, 0, 0]
    plot_axes = [0, 0, 0]
    plot_data = [[0.789, 1.04719],[1000, 6500], [0xAA, 0xBB, 0xCC],  [0xDD, 0xEE, 0xFF]]

    plt.ion()
    fig = plt.figure(figsize=(20,20)) 

    plot_axes[0] = plt.subplot(131, projection='polar')
    plot_axes[0].set_title('Lidar')
    plot_axes[0].set_theta_zero_location("N")
    plot_axes[0].set_theta_direction(-1)
    plot_axes[0].set_rmax(7000)
    plot_axes[0].autoscale(enable=False)    

    plot_axes[1] = plt.subplot(132)
    plot_axes[1].set_title('IR')
    plot_axes[1].set_ylim([0x00, 0xFF])

    plot_axes[2] = plt.subplot(133)
    plot_axes[2].set_title('Cell')
    plot_axes[2].set_ylim([0x00, 0xFF])

    plot_refs[0] = plot_axes[0].scatter(plot_data[0], plot_data[1], s=10)
    plot_refs[1], = plot_axes[1].plot(['IR1', 'IR2', 'IR3'], plot_data[2])
    plot_refs[2] = plot_axes[2].bar(['Cell1', 'Cell2', 'Cell3'], plot_data[3])

    fig.canvas.draw()
    fig.canvas.flush_events()
    plt.show(block=False)
    return fig, plot_axes, plot_refs, plot_data


def plot_update(fig, plot_axes, plot_refs, plot_data):

    plot_axes[0].clear()
    plot_axes[1].clear()
    plot_axes[2].clear()

    plot_axes[0].scatter(plot_data[0], plot_data[1], s=10)
    plot_axes[1].plot(['IR1', 'IR2', 'IR3'], [plot_data[2][0], plot_data[2][1], plot_data[2][2]])
    plot_axes[2].bar(['Cell1', 'Cell2', 'Cell3'], [0xCE, 0xCE, 0xCE])

    plot_axes[0].set_title('Lidar')
    plot_axes[0].set_theta_zero_location("N")
    plot_axes[0].set_theta_direction(-1)
    plot_axes[0].set_rmax(7000)
    plot_axes[0].autoscale(enable=False)    

    plot_axes[1].set_title('IR')
    plot_axes[1].set_ylim([0x00, 0xFF])

    plot_axes[2].set_title('Cell')
    plot_axes[2].set_ylim([0x00, 0xFF])
    
    fig.canvas.draw()
    fig.canvas.flush_events()
    
def test(count_test):
    new_data = [[], [], [0,0,0], [0,0,0]]

    new_data[0].append(0.7853 * count_test)
    new_data[1].append(100*count_test)
    
    new_data[2][0] = count_test
    new_data[2][1] = count_test + 10
    new_data[2][2] = count_test + 20

    new_data[3][0] = count_test
    new_data[3][1] = count_test + 10
    new_data[3][2] = count_test + 20

    return new_data


def __init__():
    try:
        fig, plot_axes, plot_refs, plot_data = plot_init()
        while True:
            plot_data = SPI_main_loop()
            plot_update(fig, plot_axes, plot_refs, plot_data)
            #time.sleep(1)

        sys.stdout.flush()
    except KeyboardInterrupt:
        exit_handler()


__init__()