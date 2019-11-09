#include "UART1.h"
#include "tm4c123gh6pm.h"
#include "v8stdint.h"

void Lidar_Init(void);

int scan_lidar(void);

void restart_lidar(void);

void stop_lidar(void);

void set_lidar_speed(void);

void health(void);

void device_status(void);

#define RECEIVED 1
#define NOT_RECEIVED 0
#define FAILED 0

#define SCAN_RESPONSE_HEADER 0x5A
#define SCAN_HEADER_SIZE 0X07
#define PACKET_FIRST_BYTE 0XA5
#define MAX_PACKET_SIZE 0X80
#define ZERO_PACKET 0X00
#define CLOUD_PACKET 0X01
#define PAKET_HEADER 0X55
#define SERIAL_NUM_SIZE 16


struct scan_node{
    uint16_t  packet_header;
    uint8_t   packet_type;
    uint8_t   sample_quantity;
    uint16_t  start_angle;
    uint16_t  ending_angle;
    uint16_t  buffer[MAX_PACKET_SIZE];
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

struct scan_data{
    uint8_t rotation;
    uint32_t frequency;
    uint8_t sample_rate;
};

struct point_data{
    uint8_t quality;
    float   angle;
    float   distance;
    bool    startBit;
};
