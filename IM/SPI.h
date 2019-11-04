//------------SPI_Init------------
// Initialize the SPI
// Input: none
// Output: none
void SSI1_Init(void);

char SPI_InChar(void);

void SSI1_Write(unsigned char _data);

unsigned char SSI1_SendCommand(unsigned char command);

unsigned char SSI1_Read(void);

void SSI1_CopyDataToBuffer(unsigned char *_buffer);

void SSI0_Init(void);

void SSI0_Out(unsigned char code);
