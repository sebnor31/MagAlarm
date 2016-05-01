#include "ioCC2510.h"
#include "hal_defines.h"
#include "sensor.h"


UINT16 sensor_output[17];

void delay_us(UINT16 time)
{
  UINT16 i;
  for (i=0;i<time;i++)
  {;}
}

void delay_ms(UINT16 time)
{
  UINT16 i;
  UINT16 j;
  for (i=0;i<time;i++)
  {
    for (j=0;j<895;j++)
    {;}
  }
}

void initST (void) {
  UINT16 event0 = 0x013A;
  WOREVT1 = event0 >> 8; // Set EVENT0, high byte
  WOREVT0 = event0; // Set EVENT0, low byte
  WORCTL |= 0x01;
  WORIRQ  |= 0x10;
  EA |= 1;
  STIE |= 1;
}

void initADC (void) {
  ADCCFG = 0x01;
  ADCCON2 = 0xB0; 
  ADCCON1 = 0x33;
}

void initUSART (void) {
  U0CSR = 0x80;
  U0GCR = 9;
  U0BAUD = 131;  //Set baud rage 19200
}

void SEN_reset(UINT8 board) {
  switch (board) {
  case 1:
  default:
    MR_RST_1 = 1;
    delay_us(2);
    MR_RST_1 = 0; 
    break;
  case 0 :
    MR_RST_2 = 1;
    delay_us(2);
    MR_RST_2 = 0;
    break;
  }
}

void SEN_axis(UINT8 Axis, UINT8 board) {
  switch (board) {
  case 1:
  default:
    MUX_D0_1 = (BOOL) Axis;
    MUX_D1_1 = (BOOL) (Axis>>1);
    break;
  case 0 :
    MUX_D0_2 = (BOOL) Axis;
    MUX_D1_2 = (BOOL) (Axis>>1);
    break;
  }
}

void TX_USART(UINT16 data) {
  U0DBUF = data >> 8;  
  while (UTX0IF==0);
  UTX0IF = 0;
  U0DBUF = data;
  while (UTX0IF==0);
  UTX0IF = 0;  
}

UINT16 ADC_Sample(void) {
  UINT16 Data=0;
  
  ADCCON1 |= 0x40; // Start new ADC conversion
  while(!(ADCCON1 & 0x80));
  
  Data = (((UINT16)ADCH) << 8);// Get MSB ADC result
  Data |= ADCL; // Get LSB ADC result
  if (Data & 0x8000)
    Data = 0;   //Remove unwanted negative value
  Data = Data >> 4;  
  
  return(Data);
}


void main( void ) {
  
  UINT8 board;
  UINT8 axis;
  UINT8 i;
  
  // ======== OSCILLATOR Config. ========
  CLKCON = 0x80; // CLKCON = 10000000; System Clk Osc = XOSC
  while(!(SLEEP&0x40)); // waiting until XOSC powered up & stable
  asm("NOP"); // small wait

  P0DIR |= 0x89; // 
  P0SEL &= ~0x89; // P0_0,P0_3,P0_7 output

  P1DIR |= 0x3B; // 
  P1SEL &= ~0x3B; // P1_0,P1_3,P1_4,P1_5 output
  
  P2DIR |= 0x18; // 
  P2SEL &= ~0x18; // P2_3,P2_4  output
  
  MR_RST_1 = 0;   
  MR_RST_2 = 0;     
  MR_PWR = 1;
  
  
//  initST();
  initADC();  
//  initUSART();

//  IRCON &= ~0x80;  //clear interrupt flag
//  WORIRQ &= ~0x01;

  while(1)
  {
    
//    SLEEP |= 0x02;  
//    PCON |= 0x01; // Enter PM{0 ¨C 2}    
//    asm("NOP");
//    LedGlint();
/*    i=0;
    RLED = !RLED; 
    MR_PWR = 0;
    delay_us(200);
    
    for (board = 0; board<2; board++) {      
      MR_MUX = (BOOL) board;  //Select sensor board 1
      SEN_reset(board);  //Generate set/reset pulse
      
      for (axis=0;axis<3;axis++) {
        SEN_axis(axis, board);
        delay_ms(1);
        sensor_output[i]=ADC_Sample();
        i=i+1;
      }
      
      SEN_axis(0, board);
    }
    
    MR_PWR = 1;
    delay_ms(100);
    */
    MR_PWR = !MR_PWR;
    delay_ms(1);
  }
}

#pragma vector=ST_VECTOR
__interrupt void ST_IRQ(void){
  IRCON &= ~0x80;  //clear interrupt flag
  WORIRQ &= ~0x01;
}
