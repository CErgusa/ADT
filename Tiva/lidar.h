/* INIT */
void lidar_init(void);
/********/


/* SCAN */
struct scan_node;

void lidar_scan_command(void);

int lidar_scan_response(void);

void get_packet_header(struct scan_node * PacketHeader);

unsigned char lidar_get_packet(unsigned char *buffer, unsigned char *raw);

void lidar_sync_packet(unsigned char *buffer);

/********/


/* MISC */
void restart_lidar(void);

void lidar_stop_command(void);

void lidar_cleanup(void);

void set_lidar_speed(void);

void health(void);

void device_status(void);
/********/
#define NUMBER_OF_SAMPLES_INDEX 2
#define STARTING_ANGLE_INDEX 3
#define ENDING_ANGLE_INDEX 4
#define FIRST_DATA_SAMPLE_INDEX 6
#define HEADER_LENGTH 5

#define RECEIVED 0x01
#define NOT_RECEIVED 0
#define FAILED 0
#define DEADBEEF 0xBEEF
#define DOODBEEF 0xFEED
#define PACKET_HEADER 0x55AA

#define SCAN_NODE_OFFSET 0x05
#define SCAN_RESPONSE_HEADER 0xA5
#define PACKET_FIRST_BYTE 0xA5
#define MAX_LIDAR_PACKET_SIZE 90 // 10 + 80
#define ZERO_PACKET 0x00
#define CLOUD_PACKET 0x01
#define SERIAL_NUM_SIZE 16



#define END_OF_PACKET -1

#define PACKET_HEADER_FIRST 0xAA
#define PACKET_HEADER_SECOND 0x55

struct scan_node{
    int packet_header;
    char packet_type;
    char sample_quantity;
    int start_angle;
    int ending_angle;
    int check_code;
};

//struct device_info{
//    uint8_t   model_number;
//    uint8_t   hardware_version;
//    uint8_t   serial_number[SERIAL_NUM_SIZE];
//    uint16_t  firmware_version;
//};

//struct device_health{
//    uint8_t   status;
//    uint16_t  error_code;
//};
