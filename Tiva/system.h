#define ERROR 0
#define NO_ERROR 1
#define MAX_PACKET_SIZE 167 // Size[1] + Angle[40*2] + Dist40*2] + IR[3] + Voltage[3]

void system_init(void);

int system_engine(void);
