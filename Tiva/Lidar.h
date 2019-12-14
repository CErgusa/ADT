#include "UART1.h"
#include "tm4c123gh6pm.h"
#include "v8stdint.h"

/* INIT */
void Lidar_Init(void);
/********/


/* SCAN */
struct scan_node;

int scan_lidar(void);

void get_packet_header(struct scan_node * PacketHeader);

void get_packet(int * buf, struct scan_node * PacketHeader);

void reset_lidar_shit(int * buffer, struct scan_node * PacketHeader);

/********/


/* MISC */
void restart_lidar(void);

void stop_lidar(void);

void set_lidar_speed(void);

void health(void);

void device_status(void);
/********/


#define RECEIVED 0x01
#define NOT_RECEIVED 0
#define FAILED 0
#define DEADBEEF 0xBEEF
#define DOODBEEF 0xFEED
#define PACKET_HEADER 0x55AA

#define SCAN_NODE_OFFSET 0x05
#define SCAN_RESPONSE_HEADER 0xA5
#define SCAN_HEADER_SIZE 0x07
#define PACKET_FIRST_BYTE 0xA5
#define MAX_PACKET_SIZE 0x80
#define ZERO_PACKET 0x00
#define CLOUD_PACKET 0x01
#define SERIAL_NUM_SIZE 16

#define SCAN_BYTE0 0xA5
#define SCAN_BYTE1 0x5A
#define SCAN_BYTE2 0x05
#define SCAN_BYTE3 0x00
#define SCAN_BYTE4 0x00
#define SCAN_BYTE5 0x40
#define SCAN_BYTE6 0x81

#define END_OF_PACKET -1

#define PACKET_HEADER_FIRST 0xAA
#define PACKET_HEADER_SECOND 0x55

struct scan_node{
    int packet_type;
    int sample_quantity;
    int start_angle;
    int ending_angle;
};

struct device_info{
    uint8_t   model_number;
    uint8_t   hardware_version;
    uint8_t   serial_number[SERIAL_NUM_SIZE];
    uint16_t  firmware_version;
};

struct device_health{
    uint8_t   status;
    uint16_t  error_code;
};
