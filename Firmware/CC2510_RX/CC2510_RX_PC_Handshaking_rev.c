#include "per_test_main.h"
#include "iocc2510.h"

#define Flag_mode      P1_0
#define Flag_pktRcvd   P1_1
#define Flag_pktvalid  P1_2
#define Flag_uartRx    P1_3
#define Flag_sync0     P1_6
#define Flag_sync1     P1_7

UINT16 UART_sync;
UINT16 Standby_TH = 1;
UINT8  CHANNR_RX;
UINT8  CHANNR_HS;
UINT8  CHANNR_FILE = 10;
UINT8  Operation_Mode=Mode_TDS;
UINT8  timerExpired = 0;

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

void WDTInit(void)
{
  WDCTL = 0x00; //set 1s timer internal
  WDCTL = 0x08; //Start watchdog
}

void WDTStop(void)
{
  WDCTL = 0x00; //stop watchdog
}

void WDTCLR(void)
{
  WDCTL = 0xA0;
  WDCTL = 0x50; //Clear watchdog counter
}

void init_Timer1_300ms(void) {
  //Use the timer clock frequency (203.125kHz) by setting
  //timer diver = 128.
  T1CTL = 0x0C;  //suspend the timer 1 and set divider=128
  T1CNTL = 0x00;  //Clear the counter of timer
  T1CC0H = 0xEE;
  T1CC0L = 0x09;  //load the overflow value for 300ms timer
  TIMIF |= 0x40;
  IEN1 |= 0x02;
}

/*==== PUBLIC FUNCTIONS ======================================================*/


void main(void)
{
  UINT16 i,j;
  UINT8  HS_ERROR;

  // Power up [HS XOSC] (SLEEP.OSC_PD = 0)
  SLEEP &= ~0x04;
  // Wait until [HS XOSC] is stable (SLEEP.XOSC_STB = 1)
  while ( ! (SLEEP & 0x40) );
  delay_us(100);
  // Switch system clock source to [HS XOSC] (CLKCON.OSC = 0)
  CLKCON = 0x80;            //set Timer 1 tick speed to f/128
  // Wait until system clock source has actually changed (CLKCON.OSC = 0)
  while ( CLKCON & 0x40 );
  // Power down [HS RCOSC] (SLEEP.OSC_PD = 1)
  SLEEP |= 0x04;

  //  CLKCON = 0x80; // CLKCON = 10000000; System Clk Osc = XOSC
  //  delay_ms(1);
  //  while(!(SLEEP&0x40)); // waiting until XOSC powered up & stable
  //  asm("NOP"); // small wait
  //  delay_ms(1);

  P0DIR |= 0x01;  //P0_0 out as DAC_SYNC
  P0SEL &= ~0x01; //

  P1DIR |= 0xFF;  //
  P1SEL &= ~0xFF; //

  P0DIR |= 0x08;
  P0SEL &= ~0x08;

  uartMapPort(1,0);         //Map UART function to proper I/O port
  uartStartRxForIsr(0);     // Setup RX ISR
  uartInit(0);              // Initilize UART baud rate
  init_Timer1_300ms();      //Initlize Timer1

  delay_ms(10);

  //  WDTInit();

  // Set default frequency and data rate
  CHANNR_RX = 1;       //default data receiving channel
  CHANNR_HS = 0;       //default handshaking channel

  Operation_Mode=Mode_HS;   //Initial mode is handshaking mode

  radioConfigure(DATA_RATE_1_CC2510, FREQUENCY_1_CC2510);
  CHANNR = CHANNR_HS;

  // Set up the DMA to move packet data from radio to buffer
  dmaRadioSetup(RADIO_MODE_RX);

  mode = RADIO_MODE_RX;

  EA = 1;                             // Enable interrupts globally
  IEN2 |= 0x01;                       // Enable RF general interrupt
  RFIM = IRQ_DONE;                    // Mask IRQ_DONE flag only

  IRCON &= ~0x80;                     //clear interrupt flag
  WORIRQ &= ~0x01;

  UART_sync = 0x0000;

  uartRxFlag  = 0;

  while (TRUE) {

    switch (Operation_Mode)
    {
    //Handshaking mode
    case Mode_HS:
      {
        //Update with HS frequency whenever switched from data receiving
        //to handshaking mode
        DMAARM |= 0x83;            // Disarm current DMA channel 0,1
        RFST = STROBE_IDLE;        // Switch radio to IDLE
        delay_ms(10);

        CHANNR = CHANNR_HS;

        // Set up the DMA to move packet data from radio to buffer
        dmaRadioSetup(RADIO_MODE_RX);

        mode = RADIO_MODE_RX;

        while(Operation_Mode == Mode_HS)
        {
          Flag_mode = 0;

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
                delay_ms(10);

                // Set transmitter mode
                mode = RADIO_MODE_TX;

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
                radioPktBuffer[7] = CHANNR_RX;

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
                    }
                  }
                }

                DMAARM |= 0x81;            // Disarm current DMA channel 0

                //Send handshaking error message to PC/iPod
                for(i=0;i<1;i++)
                {
                  U0DBUF = HS_ERROR;
                  while(!UTX0IF);
                  UTX0IF = 0;
                  delay_ms(10);
                }


                /********************************/
                /********Handshaking ends********/
                /********************************/
              }
            }
          }
        }
        break;
      }

    //Data transmission mode
    case Mode_TDS:
      {
        //Update with RX frequency whenever switched from handshaking
        //to data receiving mode
        DMAARM |= 0x83;            // Disarm current DMA channel 0,1
        RFST = STROBE_IDLE;        // Switch radio to IDLE
        delay_ms(10);

        CHANNR = CHANNR_RX;

        mode = RADIO_MODE_RX;

        // Set up the DMA to move packet data from radio to buffer
        dmaRadioSetup(RADIO_MODE_RX);

        WDTInit();

        while(Operation_Mode == Mode_TDS)
        {
          Flag_mode = 1;

          DMAARM = DMAARM_CHANNEL0;
          RFST = STROBE_RX;
          // Poll for incoming packet delivered by radio + dma
          if (pktRcvdFlag) {

            WDTCLR(); //Reset watchdog counter if a pkt is received

            Flag_pktRcvd=1;
            pktRcvdFlag = FALSE;

            // Check if the received packet is a valid PER test packet
            if (pktCheckValidity()) {
              Flag_pktvalid = 1;
              //The received packet is a data packet
              if (uartRxFlag)
              {
                Flag_uartRx = 1;
                // Send received data to PC through UART
                uartRxFlag = 0;     //reset receive flag
                UTX0IF = 0;
                for (i=3;i<=PACKET_LENGTH;i++) {
                  U0DBUF = radioPktBuffer[i];
                  while(!UTX0IF);
                  UTX0IF = 0;
                }
              }
              Flag_uartRx = 0;
            }
            Flag_pktvalid = 0;
          }
          Flag_pktRcvd=0;
        }

        WDTStop(); //Stop watchdog
        break;
      }

    //Training data transfer mode
    case Mode_File:
      {
        //Disable UART interrupt.
        uartStopRxForIsr(0);

        //Send 0xA55A back to PC as ack packet
        UTX0IF = 0;
        U0DBUF = 0xA5;
        while(!UTX0IF);
        UTX0IF = 0;
        U0DBUF = 0x5A;
        while(!UTX0IF);
        UTX0IF = 0;

        DMAARM |= 0x83;            // Disarm current DMA channel 0,1
        RFST = STROBE_IDLE;        // Switch radio to IDLE
        delay_ms(10);

        CHANNR = CHANNR_FILE;       //switch to channel 10 to transmitt file data
        mode = RADIO_MODE_TX;      // Set transmitter mode

        //Switch to TX mode to send training data packet
        //Set up the DMA to move packet data from buffer to radio
        dmaFileSetup(mode);

        filePktBuffer[0]=PACKET_LENGTH_FILE;

        T1CNTL = 0x00;  //Clear the  timer
        T1CTL |= 0x02;  //Starts the 300ms timer
        timerExpired=0;

        while(!timerExpired)
        {
          Flag_mode = 0;

          i=0;
          while(i<PACKET_LENGTH_FILE)
          {
            while((!URX0IF)&&(!timerExpired));

            T1CNTL = 0x00;  //reset the timer

            if(timerExpired)
            {
              i=56; // if timer expired, stop receiving data
            }
            else
            {
              URX0IF = 0;
              filePktBuffer[i+1]=U0DBUF;// if timer is not expired, read data
              i++;
            }
          }

          //If timer is not expired, wirelessly transmit data packet 5 timers
          if(!timerExpired)
          {
            for (j=0;j<10;j++)
            {
              DMAARM = DMAARM_CHANNEL1;  // Arm DMA channel 1
              RFST = STROBE_TX;           // Switch radio to TX

              // Wait until the radio transfer is completed,
              // and then reset pktSentFlag
              while(!pktSentFlag);
              pktSentFlag = FALSE;

              delay_ms(1);
            }
          }
        }


        T1CTL &= ~0x03;   //stop timer
        uartStartRxForIsr(0);
        Operation_Mode = Mode_TDS;   //switch to handshaking mode
        break;
      }
    default:
      break;
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

#pragma vector=URX0_VECTOR
__interrupt void URX0_IRQ(void) {

  UINT8 temp;

  URX0IF = 0;

  //check if a sync word is received.
  if ((UART_sync==0xAAAA)||(UART_sync==0xCCCC)||(UART_sync==0xBBBB))
  {
    uartRxBuffer[uartRxIndex++] = U0DBUF;

    //If the whole packet is received?
    if (uartRxIndex >= SIZE_OF_UART_RX_BUFFER) {
      uartRxIndex = 0;
      //IEN0 &= ~0x08;

      //According to received preamble, set RF to HS or RX mode
      switch(UART_sync)
      {
      case 0xAAAA:
        {
          Flag_sync0 = 0;
          Flag_sync1 = 0;

          uartRxFlag = 1;
          Operation_Mode = Mode_TDS;    //set to TDS RX mode
          break;
        }
      case 0xCCCC:
        {
          Flag_sync0 = 0;
          Flag_sync1 = 1;

          // Update TDS parameters
          Standby_TH = (UINT16) uartRxBuffer[0];
          Standby_TH = (Standby_TH<<8);
          Standby_TH = Standby_TH + uartRxBuffer[1];
          CHANNR_RX = uartRxBuffer[2];
          Operation_Mode = Mode_HS;    //set to handshaking mode
          break;
        }
      case 0xBBBB:
        {
          Flag_sync0 = 1;
          Flag_sync1 = 0;

          Operation_Mode = Mode_File;    //set to data file transfer mode
          break;
        }
      default:
        break;
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

//Timer 1 interrupt
#pragma vector=T1_VECTOR
__interrupt void T1_IRQ(void){
  timerExpired = 1;
}

/*==== END OF FILE ==========================================================*/
