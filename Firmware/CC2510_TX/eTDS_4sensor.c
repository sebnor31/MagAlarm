#include "ioCC2510.h"
#include "hal_defines.h"
//#include "sensor.h"
#include "per_test_dma.h"
#include "per_test_main.h"
#include "magneticsensor.h"
#include "ioCCxx10_bitdef.h"
#include "LSM9DS1_Registers.h"

#define BATT_VOLT      P0_0     // ADC for the battery voltage
#define DEN_DCDC       P0_5     // DCDC converter DEN
#define CS1            P0_6  //P0_1 sensor 1
#define CS2            P0_7  //P0_2 sensor 2

#define CS6            P1_0     // sensor LSM9DS1 accelerometer and gyroscope
#define CS5            P1_1     // sensor LSM9DS1 magnetometer
#define DEN_AG         P1_2     // LSM9DS1 DEN_AG pin
#define SCK            P1_3     // SPI CLK
#define MISO           P1_4     // SPI MISO
#define MOSI           P1_5     // SPI MOSI
#define CS3            P1_6     //P0_3 sensor 3
#define CS4            P1_7     //P0_4 sensor 4




#define CS_DISABLED     1
#define CS_ENABLED      0

#define DISABLED     0
#define ENABLED      1

#define SPI_BAUD_M  34
#define SPI_BAUD_E  13                  //230400 baud rate. change it to 12 if you want 115200 baud rate

/*
#define SPI_BAUD_M  34   
#define SPI_BAUD_E  12
*/
/******************************************************************************
 * FUNCTION PROTOTYPES
 */  
   
void spiWriteByte(uint8 write);
void spiReadByte(uint8 *read, uint8 write);



//Configure pins for the MCU modified sahadat
void init_Sensor(void) {
  P0SEL &= BIT5 |BIT6 | BIT7 ;
  P1SEL &=  BIT0 | BIT1 | BIT2 | BIT6 | BIT7;
  P1SEL = BIT3 | BIT4 | BIT5 ;                  // peripheral IO pins for SPI 
  P0SEL = BIT0;                                 // Peripheral ADC0
  P0DIR |= BIT5 |BIT6 | BIT7;                   // Output for the sensors chip select
  P1DIR |= BIT0 | BIT1 | BIT2 | BIT6 | BIT7;    // output for the sensors & LSM9DS1 Chip select
}


void InitSensorOutput(void)
{
  /**********************************Configure SPI******************************************/
// Configure USART0 for Alternative 2 => Port P1 (PERCFG.U0CFG = 1).
    PERCFG = 0X01;    
  
// Set USART to SPI mode and Master mode.
    U0CSR &= ~(U0CSR_MODE | U0CSR_SLAVE);
  
  // Set:
  // - mantissa value
  // - exponent value
  // - clock phase to be centered on first edge of SCK period
  // - negative clock polarity (SCK low when idle)
  // - bit order for transfers to LSB first
    U0BAUD =  SPI_BAUD_M;
    U0GCR = (U0GCR & ~(U0GCR_BAUD_E | U0GCR_CPOL | U0GCR_CPHA)) | U0GCR_ORDER | SPI_BAUD_E;

  
  /***********************************end of Configure SPI****************************************/
  CS1 = CS_DISABLED;
  CS2 = CS_DISABLED;
  CS3 = CS_DISABLED;
  CS4 = CS_DISABLED;
  CS5 = CS_DISABLED;
  CS6 = CS_DISABLED;
  // CNTRL register for the magnetometer values
    
    
    mgtWriteReg(MGT_CTRL1 , MGT_CTRL1_VAL, 1);
    delay_us(10);
    mgtWriteReg(MGT_CTRL1 , MGT_CTRL1_VAL, 2);
    delay_us(10);
    mgtWriteReg(MGT_CTRL1 , MGT_CTRL1_VAL, 3);
    delay_us(10);
    mgtWriteReg(MGT_CTRL1 , MGT_CTRL1_VAL, 4);
    delay_us(10);
    
    mgtWriteReg(MGT_CTRL2 , MGT_CTRL2_VAL, 1);
    delay_us(10);
    mgtWriteReg(MGT_CTRL2 , MGT_CTRL2_VAL, 2);
    delay_us(10);
    mgtWriteReg(MGT_CTRL2 , MGT_CTRL2_VAL, 3);
    delay_us(10);
    mgtWriteReg(MGT_CTRL2 , MGT_CTRL2_VAL, 4);
    delay_us(10);
    
    mgtWriteReg(MGT_CTRL3 , MGT_CTRL3_VAL, 1);
    delay_us(10);
    mgtWriteReg(MGT_CTRL3 , MGT_CTRL3_VAL, 2);
    delay_us(10);
    mgtWriteReg(MGT_CTRL3 , MGT_CTRL3_VAL, 3);
    delay_us(10);
    mgtWriteReg(MGT_CTRL3 , MGT_CTRL3_VAL, 4);
    delay_us(10);
    
    mgtWriteReg(MGT_CTRL4 , MGT_CTRL4_VAL, 1);
    delay_us(10);
    mgtWriteReg(MGT_CTRL4 , MGT_CTRL4_VAL, 2);
    delay_us(10);
    mgtWriteReg(MGT_CTRL4 , MGT_CTRL4_VAL, 3);
    delay_us(10);
    mgtWriteReg(MGT_CTRL4 , MGT_CTRL4_VAL, 4);
    delay_us(10);
    
    mgtWriteReg(MGT_CTRL5 , MGT_CTRL5_VAL, 1);
    delay_us(10);
    mgtWriteReg(MGT_CTRL5 , MGT_CTRL5_VAL, 2);
    delay_us(10);
    mgtWriteReg(MGT_CTRL5 , MGT_CTRL5_VAL, 3);
    delay_us(10);
    mgtWriteReg(MGT_CTRL5 , MGT_CTRL5_VAL, 4);
    delay_us(10);
    
    mgtWriteReg(MGT_CTRL6 , MGT_CTRL6_VAL, 1);
    delay_us(10);
    mgtWriteReg(MGT_CTRL6 , MGT_CTRL6_VAL, 2);
    delay_us(10);
    mgtWriteReg(MGT_CTRL6 , MGT_CTRL6_VAL, 3);
    delay_us(10);
    mgtWriteReg(MGT_CTRL6 , MGT_CTRL6_VAL, 4);
    delay_us(10);
    
    mgtWriteReg(MGT_CTRL7 , MGT_CTRL7_VAL, 1);
    delay_us(10);
    mgtWriteReg(MGT_CTRL7 , MGT_CTRL7_VAL, 2);
    delay_us(10);
    mgtWriteReg(MGT_CTRL7 , MGT_CTRL7_VAL, 3);
    delay_us(10);
    mgtWriteReg(MGT_CTRL7 , MGT_CTRL7_VAL, 4);
    delay_us(10);
    
    mgtWriteReg(MGT_INT_CTRL_M , MGT_INT_CTRL_M_VAL, 1);
    delay_us(10);
    mgtWriteReg(MGT_INT_CTRL_M , MGT_INT_CTRL_M_VAL, 2);
    delay_us(10);
    mgtWriteReg(MGT_INT_CTRL_M , MGT_INT_CTRL_M_VAL, 3);
    delay_us(10);
    mgtWriteReg(MGT_INT_CTRL_M , MGT_INT_CTRL_M_VAL, 4);
    delay_us(10);   
}



/*******************************LSM9DS1 initialization********************************/
void InitLSM9DS1(void)
{
  
  CS1 = CS_DISABLED;
  CS2 = CS_DISABLED;
  CS3 = CS_DISABLED;
  CS4 = CS_DISABLED;
  CS5 = CS_DISABLED;
  CS6 = CS_DISABLED;
  
  
  /* init for the Gyroscope configuration*/
  LSM9DWriteReg(LSM9DS1_CTRL_REG1_G, LSM9DS1_CTRL_REG1_G_VAL, 2);
  delay_us(10);
  LSM9DWriteReg(LSM9DS1_CTRL_REG2_G,LSM9DS1_CTRL_REG2_G_VAL,2);
  delay_us(10);
  LSM9DWriteReg(LSM9DS1_CTRL_REG3_G,LSM9DS1_CTRL_REG3_G_VAL,2);
  delay_us(10);
  LSM9DWriteReg(LSM9DS1_CTRL_REG4,LSM9DS1_CTRL_REG4_VAL,2);
  delay_us(10);
  LSM9DWriteReg(LSM9DS1_ORIENT_CFG_G,LSM9DS1_ORIENT_CFG_G_VAL,2);
  delay_us(10);
  
  /* end of initialization gyroscoper*/
   
  /* init for accelerometer */
  LSM9DWriteReg(LSM9DS1_CTRL_REG5_XL, LSM9DS1_CTRL_REG5_XL_VAL, 2);
  delay_us(10);
  LSM9DWriteReg(LSM9DS1_CTRL_REG6_XL, LSM9DS1_CTRL_REG6_XL_VAL, 2);
  delay_us(10);
  LSM9DWriteReg(LSM9DS1_CTRL_REG7_XL, LSM9DS1_CTRL_REG7_XL_VAL, 2);
  delay_us(10);
 
 /* end of accelerometer initialization */ 
  
  /* initialize LSM9DS1 magnetometer */
  DEN_AG = DISABLED;
  LSM9DWriteReg(LSM9DS1_CTRL_REG1_M, LSM9DS1_CTRL_REG1_M_VAL, 1);
  delay_us(10);
  LSM9DWriteReg(LSM9DS1_CTRL_REG2_M, LSM9DS1_CTRL_REG2_M_VAL, 1);
  delay_us(10);
  LSM9DWriteReg(LSM9DS1_CTRL_REG3_M, LSM9DS1_CTRL_REG3_M_VAL, 1);
  delay_us(10);
  LSM9DWriteReg(LSM9DS1_CTRL_REG4_M, LSM9DS1_CTRL_REG4_M_VAL, 1);
  delay_us(10);
  LSM9DWriteReg(LSM9DS1_CTRL_REG5_M, LSM9DS1_CTRL_REG5_M_VAL, 1);
  delay_us(10);
  
  /* end of init magnetometer  */
  
}




void GetSensorOutput(void)
{

  UINT8 i=7;
  UINT8 j=0;
  UINT8 k=0;
  uint16 *p, *q, *r, *s, *v, *w, *x, *y, *t, *u, *z ;
  UINT16 SEN_OUT[9];
  
    
     // delay_us(500);
      p = mgtReadAcc(1);
      //delay_us(10);
      SEN_OUT[k++]=p[0];
      SEN_OUT[k++]=p[1];
      SEN_OUT[k++]=p[2];
      
    
      q = mgtReadAcc(2);
      //delay_us(10);
      SEN_OUT[k++]=q[0];
      SEN_OUT[k++]=q[1];
      SEN_OUT[k++]=q[2];
      
/*      
      r = mgtReadAcc(3);
      //delay_us(10);
      SEN_OUT[k++]=r[0];
      SEN_OUT[k++]=r[1];
      SEN_OUT[k++]=r[2];
     
      
      s = mgtReadAcc(4);
      //delay_us(10);
      SEN_OUT[k++]= s[0];
      SEN_OUT[k++]=s[1];
      SEN_OUT[k++]=s[2];
     
      v = LSM3DAcc(1);
      //delay_us(10);
      SEN_OUT[k++]=v[0];
      SEN_OUT[k++]=v[1];
      SEN_OUT[k++]=v[2];
 
      w = LSM3DAcc(2);
      //delay_us(10);
      SEN_OUT[k++]=w[0];
      SEN_OUT[k++]=w[1];
      SEN_OUT[k++]=w[2];
      
     
      x = LSM3DAcc(3);
      //delay_us(10);
      SEN_OUT[k++]=x[0];
      SEN_OUT[k++]=x[1];
      SEN_OUT[k++]=x[2];
     
      
      y = LSM3DAcc(4);
      //delay_us(10);
      SEN_OUT[k++]= y[0];
      SEN_OUT[k++]=y[1];
      SEN_OUT[k]=y[2];
 */
 /*   
      t = ReadLSM9DS1(2,LSM9DS1_OUT_X_L_XL);                  // reading accelerometer
      //delay_us(10);
      SEN_OUT[k++]= t[0];
      SEN_OUT[k++]= t[1];
      SEN_OUT[k++]= t[2];
      
      u = ReadLSM9DS1(2,LSM9DS1_OUT_X_L_G);                   // reading gyroscope
      //delay_us(10);
      SEN_OUT[k++]= u[0];
      SEN_OUT[k++]= u[1];
      SEN_OUT[k]= u[2]; 
 
 */     
      z = ReadLSM9DS1(1,LSM9DS1_OUT_X_L_M);                   // reading LSM9D magnetometer
      //delay_us(10);
      SEN_OUT[k++]= z[0];
      SEN_OUT[k++]= z[1];
      SEN_OUT[k]= z[2]; 
      
   
      
       
      
      for (i=7;i<PACKET_LENGTH;i=i+2){
        UINT16 complement2;
        if (SEN_OUT[j]&0x8000!=0) {
    //      SEN_OUT[j]=SEN_OUT[j]|0x7000;
          complement2 = ~SEN_OUT[j]+0x0001;
          complement2 = 32768 - complement2;
        }
        else {
    //      SEN_OUT[j]=SEN_OUT[j]&0x8FFF;
          complement2 = SEN_OUT[j];
          complement2 = 32768 + complement2;
        }

        radioPktBuffer[i]= (BYTE)(complement2>>8); 
        radioPktBuffer[i+1]= (BYTE)(complement2);

        j=j+1;
      }
}


void init_ADC (void) {
  //  ADCCFG = 0x01;      //AIN on P0_0 are enabled.
  ADCCFG |= ADCCFG_0;  //AIN on P0_0 are enabled.
  /*  //ADCCON2 is used to control sequence conversion.
  ADCCON2 = 0xB5;     //[7:6]-reference. 10 - AVDD pin
  //[5:4]-11, 10, 01, 00 for 12, 10, 9, 7 bits.
  //[3:0]-sequence. 0101 - AIN5 and 0110 - AIN6
  */
  //ADCCON3 is used to control single conversion
  ADCCON3 = 0xB0;     //[7:6]-10 for AVDD reference
  //[5:4]-11 for 12 bits resolution
  //[3:0]-sequence. 0101 - AIN5 and 0110 - AIN6
  
  ADCCON1 = 0x33;     //[7]-EOC
  //[6]-Start Conv
  //[5:4]-start select. 11-by ADCCON1.ST pin
  //[3:2]-00 Normal
  //[1:0]-Reserved
}


UINT16 ADC_Sample(UINT8 Channel)
{
  UINT16 Data=0;
  
  //  ADCCON1 |= 0x40; // Start new ADC conversion
  
  ADCCON3 = (0xB0+Channel);
  while(!(ADCCON1 & 0x80));
  
  Data = (((UINT16)ADCH) << 8); // Get MSB ADC result
  Data |= ADCL;   // Get LSB ADC result
  if (Data & 0x8000)
  Data=0;       //remove unwanted negative value
  Data = Data >> 4;
  
  return(Data);
}

UINT16 GetBatteryVoltage(void)
{
  UINT16 ADC_Result;
  ADC_Result=ADC_Sample(0);
  return(ADC_Result);
}

/**************************************************************************//**
* @fn       mgtWriteReg(uint8 reg, uint8 val)
*
* @brief    Write one byte to a sensor register
*
* @param    reg     Register address
* @param    val     Value to write
* @param    sennum  choose the sensor
*
* @return   void
******************************************************************************/
void mgtWriteReg(uint8 reg, uint8 val, uint8 sennum)
{
    switch (sennum){
    case 1:
    CS1 = CS_ENABLED;
    break;
    case 2:
    CS2 = CS_ENABLED;
    break;
    case 3:
    CS3 = CS_ENABLED;
    break;
    case 4:
    CS4 = CS_ENABLED;
    break;
    default:
      break;
    }
    spiWriteByte(reg);      // Write address
    spiWriteByte(val);      // Write value
    switch (sennum){
    case 1:
    CS1 = CS_DISABLED;
    break;
    case 2:
    CS2 = CS_DISABLED;
    break;
    case 3:
    CS3 = CS_DISABLED;
    break;
    case 4:
    CS4 = CS_DISABLED;
    break;
    default:
      break;
    }
}



/**************************************************************************//**
* @fn       LSM9DWriteReg(uint8 reg, uint8 val, uint8 sennum)
*
* @brief    Write one byte to a sensor register
*
* @param    reg     Register address
* @param    val     Value to write
* @param    sennum  choose the sensor 1 = magnetometer; 2 = accel & gyro;
*
* @return   void
******************************************************************************/
void LSM9DWriteReg(uint8 reg, uint8 val, uint8 sennum)
{
    switch (sennum){
    case 1:
    CS5 = CS_ENABLED;
    break;
    case 2:
    DEN_AG = ENABLED;
    CS6 = CS_ENABLED;
    break;
    default:
      break;
    }
    spiWriteByte(reg);      // Write address
    spiWriteByte(val);      // Write value
    switch (sennum){
    case 1:
    CS5 = CS_DISABLED;
    break;
    case 2:
    CS6 = CS_DISABLED;
    DEN_AG = DISABLED;
    break;
    default:
      break;
    }
}


/**************************************************************************//**
* @fn       mgtReadAcc( uint8 sennum)
*
* @brief    Read each sensor
*
* @param    reg     Register address
* @param    val     Pointer to destination of read value
*
* @return   void
******************************************************************************/

uint16 * mgtReadAcc( uint8 sennum)
{
    static uint16 pBUF[3];
    uint8 readout[6] = {0,0,0,0,0,0};

    // Read all data from accelerometer
    switch (sennum){
    case 1:
    CS1 = CS_ENABLED;
    break;
    case 2:
    CS2 = CS_ENABLED;
    break;
    case 3:
    CS3 = CS_ENABLED;
    break;
    case 4:
    CS4 = CS_ENABLED;
    break;
    default:
    break;
    }
    
 
    spiWriteByte(0xC0|MGT_X_LSB);     // Write start address
    for(uint8 i = 0; i<6; i++)
    {
        spiReadByte((uint8 *)&readout[i], 0xFF); // Read byte
    }
    switch (sennum){
    case 1:
    CS1 = CS_DISABLED;
    break;
    case 2:
    CS2 = CS_DISABLED;
    break;
    case 3:
    CS3 = CS_DISABLED;
    break;
    case 4:
    CS4 = CS_DISABLED;
    break;
    default:
      break;
    }
 //   sennum = CS_DISABLED;

    // Use only most significant byte of each channel.
    pBUF[0] = (((uint8)readout[0]) | ((int16)(readout[1]) << 8) );
    pBUF[1] = (((uint8)readout[2]) | ((int16)(readout[3]) << 8) );
    pBUF[2] = (((uint8)readout[4]) | ((int16)(readout[5]) << 8) );
    
    return pBUF;

} // mgtReadAcc



/**************************************************************************//**
* @fn       LSM303DAcc( uint8 sennum)
*
* @brief    Read each sensor
*
* @param    reg     Register address
* @param    val     Pointer to destination of read value
*
* @return   void
******************************************************************************/

uint16 * LSM3DAcc( uint8 sennum)
{
    static uint16 pBUF[3];
    uint8 readout[6] = {0,0,0,0,0,0};

    // Read all data from accelerometer
    switch (sennum){
    case 1:
    CS1 = CS_ENABLED;
    break;
    case 2:
    CS2 = CS_ENABLED;
    break;
    case 3:
    CS3 = CS_ENABLED;
    break;
    case 4:
    CS4 = CS_ENABLED;
    break;
    default:
    break;
    }
    
 
    spiWriteByte(0xC0|ACC_X_LSB);     // Write start address
    for(uint8 i = 0; i<6; i++)
    {
        spiReadByte((uint8 *)&readout[i], 0xFF); // Read byte
    }
    switch (sennum){
    case 1:
    CS1 = CS_DISABLED;
    break;
    case 2:
    CS2 = CS_DISABLED;
    break;
    case 3:
    CS3 = CS_DISABLED;
    break;
    case 4:
    CS4 = CS_DISABLED;
    break;
    default:
      break;
    }
 //   sennum = CS_DISABLED;

    // Use only most significant byte of each channel.
    pBUF[0] = (((uint8)readout[0]) | ((int16)(readout[1]) << 8) );
    pBUF[1] = (((uint8)readout[2]) | ((int16)(readout[3]) << 8) );
    pBUF[2] = (((uint8)readout[4]) | ((int16)(readout[5]) << 8) );
    
    return pBUF;

} // LSM303DAcc


/**************************************************************************//**
* @fn       ReadLSM9DS1(uint8 sennum, uint8 reg_add)
*
* @brief    Read each sensor
*
* @param    reg     Register address
* @param    val     Pointer to destination of read value
*
* @return   void
******************************************************************************/

uint16 * ReadLSM9DS1(uint8 sennum, uint8 reg_add)
{
    static uint16 pBUF[3];
    uint8 readout[6] = {0,0,0,0,0,0};

    // Read all data from accelerometer
    switch (sennum){
    case 1:
    CS5 = CS_ENABLED;
    
    spiWriteByte(0xC0|reg_add);     // Write start address
    for(uint8 j = 0; j<6; j++)
    {
        spiReadByte((uint8 *)&readout[j], 0xFF); // Read byte
    }
    break;
    case 2:
    DEN_AG = ENABLED;
    CS6 = CS_ENABLED;
    spiWriteByte(0x80|reg_add);     // Write start address
    for(uint8 i = 0; i<6; i++)
    {
        spiReadByte((uint8 *)&readout[i], 0xFF); // Read byte
    }
    break;
    default:
    break;
    }
    
    switch (sennum){
    case 1:
    CS5 = CS_DISABLED;
    break;
    case 2:
    CS6 = CS_DISABLED;
    DEN_AG = DISABLED;
    break;
    default:
      break;
    }
 

    // Use only most significant byte of each channel.
    pBUF[0] = (((uint8)readout[0]) | ((int16)(readout[1]) << 8) );
    pBUF[1] = (((uint8)readout[2]) | ((int16)(readout[3]) << 8) );
    pBUF[2] = (((uint8)readout[4]) | ((int16)(readout[5]) << 8) );
    
    return pBUF;

} 


/**************************************************************************//**
* @fn       spiWriteByte(uint8 write)
*
* @brief    Write one byte to SPI interface
*
* @param    write   Value to write
******************************************************************************/
void spiWriteByte(uint8 write)
{
        U0CSR &= ~0x02;                 // Clear TX_BYTE
        U0DBUF = write;
        while (!(U0CSR & 0x02));        // Wait for TX_BYTE to be set
        
}


/**************************************************************************//**
* @fn       spiReadByte(uint8 *read, uint8 write)
*
* @brief    Read one byte from SPI interface
*
* @param    read    Read out value
* @param    write   Value to write
******************************************************************************/
void spiReadByte(uint8 *read, uint8 write)
{
        U0CSR &= ~0x02;                 // Clear TX_BYTE
        U0DBUF = write;                 // Write address to accelerometer
        while (!(U0CSR & 0x02));        // Wait for TX_BYTE to be set
        *read = U0DBUF;                 // Save returned value
        
}


/******************************************************************************/