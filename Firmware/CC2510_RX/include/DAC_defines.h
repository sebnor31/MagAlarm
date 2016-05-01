
//define pins for DAC serial interface
#define DAC_SYNC    P0_0   //out, DAC sync, low active
#define DAC_SDO     P0_1   //in
#define DAC_SCLK    P1_0   //out
#define DAC_SDIN    P1_1   //out
#define DAC_LDAC    P1_2   //out, DAC load, low active
#define DAC_CLR     P1_3   //out, DAC clear, low active

//use UART 1 alternate 2
#define UART_CT     P1_4
#define UART_RT     P1_5
#define UART_TX     P1_6
#define UART_RX     P1_7

//define register values for DAC
#define Reg_DAC     0x00
#define Reg_OR      0x01
#define Reg_Power   0x02
#define Reg_Control 0x03

#define DAC_A       0x00
#define DAC_B       0x01
#define DAC_C       0x02
#define DAC_D       0x03
#define DAC_ALL     0x04

#define OP_10V      0x01
#define OP_10_8V    0x02

#define PUA         0x01
#define PUB         0x02
#define PUC         0x04
#define PUD         0x08
#define PUREF       0x10