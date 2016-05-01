#ifndef SENSOR_H
#define SENSOR_H

//Battery status
#define LV        P0_6   //in, Low battery signal
#define V_SW      P0_5   //in, Battery voltage

//I2C signal
#define SCL       P1_2   //inout, I2C DATA
#define SDA       P1_3   //inout, I2C CLK

//Chip select
#define CS        P1_0   //out, sensor selection

//Right sensors
#define R_INT1    P1_7  //in, sensor 1 interrupt
#define R_INT2    P1_6  //in, sensor 2 interrupt
#define R_DRDY1   P1_5  //in, sensor 1 ready
#define R_DRDY2   P1_4  //in, sensor 2 ready

//Left sensors
#define L_INT1    P0_3  //in, sensor 1 interrupt
#define L_INT2    P0_2  //in, sensor 2 interrupt
#define L_DRDY1   P0_1  //in, sensor 1 ready
#define L_DRDY2   P0_0  //in, sensor 2 ready

//I2S interface
#define I2S_CK    P2_3  //out, I2S CK
#define I2S_WS    P2_2  //out, I2S WS
#define I2S_RX    P2_1  //in,  I2S RX
#define I2S_TX    P2_0  //out, I2S TX

// define address
#define ADDR_VDD_RD   0x1F   //ADDR=DVDD, read
#define ADDR_VDD_WR   0x1E   //ADDR=DVDD, write
#define ADDR_GND_RD   0x1D   //ADDR=GND, read
#define ADDR_GND_WR   0x1C   //ADDR=GND, write

// Register map
#define INFO    0x0D  //More infor
#define WIA     0x0F  //Who I am
#define DATAX   0x10  //X ouput value
#define DATAY   0x12  //Y ouput value
#define DATAZ   0x14  //Z ouput value
#define INS1    0x16  //Interrupt source 1
#define STAT1   0x18  //Status
#define INL     0x1A  //Interrupt release
#define CNTL1   0x1B  //Control setting 1
#define CNTL2   0x1C  //Control setting 2
#define CNTL3   0x1D  //Control setting 3
#define INC1    0x1E  //Interrupt control 1
#define ITHR1   0x26  //Interrupt threshold
#define PRET    0x30  //Preset time
#define CNTL4   0x5C  //Control setting 4
#define TEMP    0x60  //Temperature value
#define OFFX    0x6C  //Offset X value
#define OFFY    0x72  //Offset Y value
#define OFFZ    0x78  //Offset Z value
#define SENSX   0x96  //Sensitivity X value
#define SENSY   0x98  //Sensitivity Y value
#define SENSZ   0x9A  //Sensitivity Z value
#define GAIN_PARAX   0x9D  //Axis interference X value
#define GAIN_PARAY   0x9F  //Axis interference Y value
#define GAIN_PARAZ   0xA1  //Axis interference Z value
#define VER     0xE8  //Firmware version
#define SN      0xEA  //Serial number

#define Left    0
#define Right   1
#define End     0
#define Mid     1
#define X       0
#define Y       1
#define Z       2
#define SET     1
#define RESET   0
#define ON      0
#define OFF     1
#define LOW      0
#define HIGH     1

extern void delay_us(UINT16 time);
extern void delay_ms(UINT16 time);

#endif
