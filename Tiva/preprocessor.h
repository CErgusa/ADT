#include "Lidar.h"

static int packet_to_send[MAX_PACKET_SIZE];

void PP_Get(int data);
void PP_Send(int data);

void clear_pts(void);

