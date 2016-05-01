#include "ioCC2510.h"
#include "per_test_main.h"
#include "DAC_defines.h"

void DAC_Reg_WR(UINT8 Reg_addr,UINT8 DAC_addr, UINT16 data)
{
  UINT8 DAC_buffer[3];
  UINT8 i,j;

  DAC_buffer[0]=((0x07&Reg_addr)<<3)+(0x07&DAC_addr);   //DB23=0 - write, DB22=0- reserved, DB21-DB19-register address
                                                      //DB18-16 - DAC address
  DAC_buffer[1]=(data&0xFF00)>>8;
  DAC_buffer[2]=data&0x00FF;

  DAC_SCLK = LOW;
  DAC_SYNC = LOW;
  for(i=0;i<3;i++)
  {
    for(j=0;j<8;j++)
    {
      if (!(DAC_buffer[i]&0x80))
        DAC_SDIN = LOW;
      else
        DAC_SDIN = HIGH;

      DAC_SCLK = HIGH;
      DAC_SCLK = LOW;
      DAC_buffer[i]=DAC_buffer[i]<<1;

    }
  }
  DAC_SYNC = HIGH;
}

void DACInit(void)
{
  UINT8 Reg_addr;
  UINT8 DAC_addr;
  UINT16 data;

  DAC_CLR = HIGH;     //disable clear function
  DAC_LDAC = LOW;     //DAC update individually
  
  //set output power range
  Reg_addr = Reg_OR;
  DAC_addr = DAC_ALL;
  data = OP_10V;
  DAC_Reg_WR(Reg_addr, DAC_addr, data);

  //power on DAC A, B, C and internal reference
  Reg_addr = Reg_Power;
  DAC_addr = 0;
  data = PUREF+PUA+PUB+PUC;
  DAC_Reg_WR(Reg_addr, DAC_addr, data);

  //initiate DAC value
  Reg_addr = Reg_DAC;
  DAC_addr = DAC_ALL;
  data = 0x9990;    //output 6V at all channels
  DAC_Reg_WR(Reg_addr, DAC_addr, data);
}
