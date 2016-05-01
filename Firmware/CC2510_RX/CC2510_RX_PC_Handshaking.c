#include "per_test_main.h"
#include "iocc2510.h"
#include "DAC_defines.h"

UINT16 UART_sync;
UINT16 Standby_TH = 1;
UINT8  Channel = 1;
UINT8  HS_Status = 0;
UINT32 Freq_RX;
UINT32 Freq_HS;

void delay_us(UINT16 time)
{
  UINT16 i;
  for (i=0;i<time;i++)
  {;}
}

void delay_ms(UINT16 time)
{
  UINT16 i;
  UINT8 j;
  for (i=0;i<time;i++)
  {
    for (j=0;j<100;j++)
    {;}
  }
}

void WDTInit(void)
{
  WDCTL = 0x00; //set 1s timer internal
  WDCTL = 0x08; //Start watchdog
}

void WDTCLR(void)
{
  WDCTL = 0xA0;
  WDCTL = 0x50; //Clear watchdog counter
}

/*==== PUBLIC FUNCTIONS ======================================================*/


void main(void)
{
  UINT16 i;
  UINT8  HS_ERROR;

  CLKCON = 0x80; // CLKCON = 10000000; System Clk Osc = XOSC
  delay_ms(1);
  while(!(SLEEP&0x40)); // waiting until XOSC powered up & stable
  asm("NOP"); // small wait
  delay_ms(1);

  P0DIR |= 0x01;  //P0_0 out as DAC_SYNC
  P0SEL &= ~0x01; //

  P1DIR |= 0x0F;  //
  P1SEL &= ~0x0F; //

  P0DIR |= 0x08;
  P0SEL &= ~0x08;

  uartMapPort(1,0);    //Map UART function to proper I/O port
  uartStartRxForIsr(0);    // Setup RX ISR
  uartInit(0);    // Initilize UART baud rate
  delay_ms(10);

  WDTInit();

  // Set default frequency and data rate
  Freq_RX = FREQUENCY_1_CC2510;       //default data receiving frequency
  Freq_HS = FREQUENCY_1_CC2510;       //default handshaking frequency

  radioConfigure(DATA_RATE_1_CC2510, Freq_HS);

  // Set up the DMA to move packet data from radio to buffer
  dmaRadioSetup(RADIO_MODE_RX);

  mode = RADIO_MODE_RX;

  EA = 1;                             // Enable interrupts globally
  IEN2 |= 0x01;                       // Enable RF general interrupt
  RFIM = IRQ_DONE;                    // Mask IRQ_DONE flag only

  IRCON &= ~0x80;               //clear interrupt flag
  WORIRQ &= ~0x01;

  UART_sync = 0x0000;

  uartRxFlag  = 0;

  while (TRUE) {

    //Handshaking mode
    if(HS_Status == 1)
    {
      //Update with HS frequency whenever switched from data receiving
      //to handshaking mode
      radioConfigure(DATA_RATE_1_CC2510, Freq_HS);

      while(HS_Status == 1)
      {

        WDTCLR(); //Reset watchdog counter
        DMAARM = DMAARM_CHANNEL0;
        RFST = STROBE_RX;
        // Poll for incoming packet delivered by radio + dma
        if (pktRcvdFlag) {
          pktRcvdFlag = FALSE;

          // Check if the received packet is a valid PER test packet
          if (pktCheckValidity()) {

            if ((radioPktBuffer[3]==0xCC)&&(radioPktBuffer[4]==0xCC))
            {
              //Handshaking request packet is received from headset

              /********************************/
              /********************************/
              /********Handshaking*************/
              /********************************/
              /********************************/

              DMAARM |= 0x81;            // Disarm current DMA channel 0
              RFST = STROBE_IDLE;        // Switch radio to IDLE
              delay_ms(100);

              // Set transmitter mode
              mode = RADIO_MODE_TX;

              //Handshaking works at default frequency
              radioConfigure(DATA_RATE_1_CC2510, Freq_HS);

              //Switch to TX mode to send parameter packet
              //Set up the DMA to move packet data from buffer to radio
              dmaRadioSetup(RADIO_MODE_TX);

              //Initialization packet
              radioPktBuffer[0] = PACKET_LENGTH;                  // Length byte
              radioPktBuffer[1] = (BYTE) (NETWORK_ID_KEY>>8);     // Network identifier
              radioPktBuffer[2] = (BYTE) NETWORK_ID_KEY;
              radioPktBuffer[3] = 0x5A;                           // Handshaking preamble
              radioPktBuffer[4] = 0xA5;                           // Handshaking preamble
              radioPktBuffer[5] = (BYTE) (Standby_TH>>8);
              radioPktBuffer[6] = (BYTE) Standby_TH;
              radioPktBuffer[7] = Channel;

              for (i=0;i<5;i++)
              {
                DMAARM = DMAARM_CHANNEL0;  // Arm DMA channel 0
                RFST = STROBE_TX;           // Switch radio to TX

                // Wait until the radio transfer is completed,
                // and then reset pktSentFlag
                while(!pktSentFlag);
                pktSentFlag = FALSE;
              }

              //Switch back to RX mode to receive ACK packet
              // Set transmitter or receiver mode
              mode = RADIO_MODE_RX;

              // Set up the DMA to move packet data from radio to buffer
              dmaRadioSetup(RADIO_MODE_RX);

              DMAARM = DMAARM_CHANNEL0;
              RFST = STROBE_RX;

              delay_ms(200);

              HS_ERROR=1;

              // Check the ACK packet
              if (pktRcvdFlag)
              {
                pktRcvdFlag = FALSE;
                // Check if the received packet is a valid packet
                if (pktCheckValidity()) {
                  // Check if the received packet is a handshaking packet (0xA5 and 0x5A)
                  // Different from TX preamble intentionally.
                  if ((radioPktBuffer[3]==0xA5)&&(radioPktBuffer[4]==0x5A))
                  {
                    // ACK received correctly
                    HS_ERROR=0;

                    //Update the receiver frequency buffer with
                    //the value set in handshaking packet
                    switch (Channel) {
                    case 1:                         // 2480 MHz
                    default:
                      Freq_RX = FREQUENCY_1_CC2510;
                      break;
                    case 2:                         // 2460 MHz
                      Freq_RX = FREQUENCY_2_CC2510;
                      break;
                    case 3:                         // 2440 MHz
                      Freq_RX = FREQUENCY_3_CC2510;
                      break;
                    case 4:                         // 2420 MHz
                      Freq_RX = FREQUENCY_4_CC2510;
                      break;
                    }
                    //Update receiver frequency
                    radioConfigure(DATA_RATE_1_CC2510, Freq_RX);
                  }
                }
              }

              DMAARM |= 0x81;            // Disarm current DMA channel 0
              delay_ms(10);

              //Send handshaking error message to PC/iPod
              U0DBUF = HS_ERROR;
              while(!UTX0IF);
              UTX0IF = 0;


              /********************************/
              /********Handshaking ends********/
              /********************************/
            }
          }
        }
      }
    }

    //Data transmission mode
    else
    {

      //Update with RX frequency whenever switched from handshaking
      //to data receiving mode
      radioConfigure(DATA_RATE_1_CC2510, Freq_RX);

      while(HS_Status==0)
      {
        WDTCLR(); //Reset watchdog counter

        DMAARM = DMAARM_CHANNEL0;
        RFST = STROBE_RX;
        // Poll for incoming packet delivered by radio + dma
        if (pktRcvdFlag) {
          pktRcvdFlag = FALSE;

          // Check if the received packet is a valid PER test packet
          if (pktCheckValidity()) {

            //The received packet is a data packet
            if (uartRxFlag)
            {
              // Send received data to PC through UART
              uartRxFlag = 0;     //reset receive flag
              UTX0IF = 0;
              for (i=3;i<=PACKET_LENGTH;i++) {
                U0DBUF = radioPktBuffer[i];
                while(!UTX0IF);
                UTX0IF = 0;
              }
            }
          }
        }
      }
    }
  }
}



/*==== PRIVATE FUNCTIONS =====================================================*/



/*==== INTERRUPT SERVICE ROUTINES ============================================*/

/******************************************************************************
* @fn  rf_IRQ
*
* @brief
*      The only interrupt flag which throws this interrupt is the IRQ_DONE interrupt.
*      So this is the code which runs after a packet has been received or
*      transmitted.
*
* Parameters:
*
* @param  void
*
* @return void
*
******************************************************************************/
#pragma vector=RF_VECTOR
__interrupt void rf_IRQ(void) {
    RFIF &= ~IRQ_DONE;        // Tx/Rx completed, clear interrupt flag
    S1CON &= ~0x03;           // Clear the general RFIF interrupt registers

  if (mode == RADIO_MODE_RX) {
    pktRcvdFlag = TRUE;
  }
  else {
    pktSentFlag = TRUE;
  }
}


// The UARTx RX ISR assumes that the code in Figure 18 has initialized the
// UART RX session, by enabling the UART RX interrupt. Then this UART RX ISR
// will receive the data based in interrupt request generation by the
// USART peripheral.
// The code implements the following steps:
// 1. Clear UARTx RX Interrupt Flag (TCON.URXxIF = 0)
// 2. Read UxDBUF and store the value in the allocated UART RX target buffer
// 3. If all UART data received, stop this UART RX session
// Note that in order to start another UART RX session the application
// just needs to re-enable the UART RX interrupt(IEN0.URXxIE = 1).

/*
#pragma vector=URX0_VECTOR
__interrupt void URX0_IRQ(void) {
URX0IF = 0;
uartRxBuffer[uartRxIndex++] = U0DBUF;
if (uartRxIndex >= SIZE_OF_UART_RX_BUFFER) {
uartRxIndex = 0;
//IEN0 &= ~0x04;
uartRxFlag = 1;
  }
}*/

#pragma vector=URX0_VECTOR
__interrupt void URX0_IRQ(void) {

  UINT8 temp;

  URX0IF = 0;

  //check if a sync word is received.
  if ((UART_sync==0xAAAA)||(UART_sync==0xCCCC))
  {
    uartRxBuffer[uartRxIndex++] = U0DBUF;

    //If the whole packet is received?
    if (uartRxIndex >= SIZE_OF_UART_RX_BUFFER) {
      uartRxIndex = 0;
      //IEN0 &= ~0x08;

      //According to received preamble, set RF to HS or RX mode
      if (UART_sync==0xAAAA)
      {
        uartRxFlag = 1;
        HS_Status = 0;
      }
      else
      {
        // Update TDS parameters
        Standby_TH = (UINT16) uartRxBuffer[0];
        Standby_TH = (Standby_TH<<8);
        Standby_TH = Standby_TH + uartRxBuffer[1];
        Channel = uartRxBuffer[2];
        HS_Status = 1;
      }

      UART_sync = 0x0101;
    }
  }
  else
  {
    temp = U0DBUF;          // if not a sync word, update the UART_sync
    UART_sync = (UART_sync<<8)+temp;
    uartRxIndex = 0;
  }
}


/*==== END OF FILE ==========================================================*/
