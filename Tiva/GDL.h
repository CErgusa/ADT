#define LIDAR_PACKET_READY 0x01

void GDL_init(void);
void GDL_send(unsigned char data_max5bits);
unsigned char GDL_read(unsigned char index);
