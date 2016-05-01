/*==== DECLARATION CONTROL ===================================================*/
/*==== INCLUDES ==============================================================*/

#include "per_test_main.h"
#include "iocc2510.h"
#include "sensor.h"
#include "ioCCxx10_bitdef.h"

// Initialization of source buffers and DMA descriptor for the DMA transfer to sleep register
unsigned char __xdata PM2_BUF[7] = {0x06,0x06,0x06,0x06,0x06,0x06,0x04};
unsigned char __xdata dmaDesc[8] = {0x00,0x00,0xDF,0xBE,0x00,0x07,0x20,0x42};

UINT8 TimerExpires = 0;
UINT8 TimerCounter = 0;
INT16 Stdby_Upth=2048;               // Up threshold
INT16 Stdby_Lowth=0;                 // Low threshold


void delay_us(UINT16 time)
{
  UINT16 i;
  for (i=0;i<time;i++)
  {;}
}

void delay_10us(UINT16 time)
{
  UINT16 i;
  UINT8 j;
  for (i=0;i<time;i++)
  {
    for (j=0;j<12;j++)
    {;}
  }
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

void STInit_IDLE (void) {
  // Set up 1 second sleep timer for IDLE condition
  UINT16 event0 = 0x041A;     // 1Hz wakeup to blink the LED or check sensor output.
  WOREVT1 = event0 >> 8;      // Set EVENT0, high byte
  WOREVT0 = event0;           // Set EVENT0, low byte
  WORCTL |= 0x01;
  WORIRQ  |= 0x10;
  EA |= 1;
  STIE |= 1;
}

void init_Timer1(void) {
  // Use the lowest timer clock frequency (1.587kHz) by setting
  // Both clock divider and timer diver = 128.
  T1CTL = 0x0C;               //suspend the timer 1 and set divider=128
  T1CNTL = 0x00;              //Clear the counter of timer
  
  //  T1CC0H = 0xB9;
  //  T1CC0L = 0xF7;            //load the overflow value for 30s timer
  //  T1CC0H = 0x1E;
  //  T1CC0L = 0xFE;            //load the overflow value for 5s timer, testing purpose
  
  T1CC0H = 0x06;
  T1CC0L = 0x32;              //load for 1s timer to receive the paramete packet
  
  TIMIF |= 0x40;
  IEN1 |= 0x02;
}

void init_Timer1_20ms(void) {
  // Use the lowest timer clock frequency (1.587kHz) by setting
  // both clock divider and timer diver = 128.
  T1CTL = 0x0C;               //suspend the timer 1 and set divider=128
  T1CNTL = 0x00;              //Clear the counter of timer
  T1CC0H = 0x0F;
  T1CC0L = 0xDE;              //load the overflow value for 20ms timer
  TIMIF |= 0x40;
  IEN1 |= 0x02;
}

void STInit (void) {
  // UINT16 event0 = 0x0036;    //20 Hz sampling
  //UINT16 event0 = 0x0015;     //50 Hz sampling
  UINT16 event0 = 0x000C;    //90 Hz sampling           sahadat
  WOREVT1 = event0 >> 8;      // Set EVENT0, high byte
  WOREVT0 = event0;           // Set EVENT0, low byte
  WORCTL |= 0x01;
  WORIRQ  |= 0x10;
  EA |= 1;
  STIE |= 1;
}

void WDTInit(void)
{
  WDCTL = 0x00; // Set 1s timer internal
  WDCTL = 0x08; // Start watchdog
}

void WDTCLR(void)
{
  WDCTL = 0xA0;
  WDCTL = 0x50; // Clear watchdog counter
}

UINT8 Standby_Check(void)
{
  UINT8 Standby_flag;
  UINT16 X_Sensor;
  UINT16 Y_Sensor;
  UINT16 Z_Sensor;
  
  // Right back sensor
  X_Sensor = (UINT16)(radioPktBuffer[25]<<8);
  X_Sensor += radioPktBuffer[26];
  Y_Sensor = (UINT16)(radioPktBuffer[27]<<8);
  Y_Sensor += radioPktBuffer[28];
  Z_Sensor = (UINT16)(radioPktBuffer[29]<<8);
  Z_Sensor += radioPktBuffer[30];
  
  // Left back sensor
  //  X_Sensor = (UINT16)(radioPktBuffer[13]<<8);
  //  X_Sensor += radioPktBuffer[14];
  //  Y_Sensor = (UINT16)(radioPktBuffer[15]<<8);
  //  Y_Sensor += radioPktBuffer[16];
  //  Z_Sensor = (UINT16)(radioPktBuffer[17]<<8);
  //  Z_Sensor += radioPktBuffer[18];
  
  Standby_flag = (X_Sensor>Stdby_Upth||X_Sensor<Stdby_Lowth)
    || (Y_Sensor>Stdby_Upth||Y_Sensor<Stdby_Lowth) ||
      (Z_Sensor>Stdby_Upth||Z_Sensor<Stdby_Lowth);
  
  return Standby_flag;
}

void CLK_XOSC(void)
{
  // Power up [HS XOSC] (SLEEP.OSC_PD = 0)
  SLEEP &= ~0x04;
  // Wait until [HS XOSC] is stable (SLEEP.XOSC_STB = 1)
  while ( ! (SLEEP & 0x40) );
  delay_10us(10);
  // Switch system clock source to [HS XOSC] (CLKCON.OSC = 0)
  CLKCON = 0x80;
  // Wait until system clock source has actually changed (CLKCON.OSC = 0)
  while ( CLKCON & 0x40 );
  // Power down [HS RCOSC] (SLEEP.OSC_PD = 1)
  SLEEP |= 0x04;
}

void CLK_RCOSC(void)
{
  // Power up [HS RCOSC] (SLEEP.OSC_PD = 0)
  SLEEP &= ~0x04;
  // Wait until [HS RCOSC] is stable (SLEEP.HFRC_STB = 1)
  while ( ! (SLEEP & 0x20) );
  delay_10us(10);
  // Switch system clock source to HS RCOSC (CLKCON.OSC = 1),
  // and set max CPU clock speed (CLKCON.CLKSPD = 1).
  CLKCON = (CLKCON & ~0x07) | 0x40 | 0x01;
  // Wait until system clock source has actually changed (CLKCON.OSC = 1)
  while ( !(CLKCON & 0x40) );
  // Power down [HS XOSC] (SLEEP.OSC_PD = 1)
  SLEEP |= 0x04;
}

void Enter_PM2(void) //From application notes swra162b power mode
{
  // Store current DMA channel 0 descriptor and abort any ongoing transfers,
  // if the channel is in use.
  unsigned char storedDescHigh = DMA0CFGH;
  unsigned char storedDescLow = DMA0CFGL;
  DMAARM |= 0x81;
  // Update descriptor with correct source.
  dmaDesc[0] = (unsigned int)& PM2_BUF >> 8;
  dmaDesc[1] = (unsigned int)& PM2_BUF;
  // Associate the descriptor with DMA channel 0 and arm the DMA channel
  DMA0CFGH = (unsigned int)&dmaDesc >> 8;
  DMA0CFGL = (unsigned int)&dmaDesc;
  DMAARM = 0x01;
  
  // NOTE! At this point, make sure all interrupts that will not be used to
  // wake from PM are disabled as described in the "Power Management Control"
  // chapter of the data sheet.
  // The following code is timing critical and should be done in the
  // order as shown here with no intervening code.
  // Align with positive 32 kHz clock edge as described in the
  // "Sleep Timer and Power Modes" chapter of the data sheet.
  char temp = WORTIME0;
  while(temp == WORTIME0);
  // Make sure XOSC is powered down when entering PM{2 - 3} and that the
  // flash cache is disabled.
  MEMCTR |= 0x02;
  SLEEP = 0x06;
  // Enter power mode as described in chapter "Power Management Control"
  // in the data sheet. Make sure DMA channel 0 is triggered just before
  // setting PCON.IDLE.
  asm("NOP");
  asm("NOP");
  asm("NOP");
  
  if(SLEEP & 0x03)
  {
    asm("MOV 0xD7,#0x01"); // DMAREQ = 0x01;
    asm("NOP"); // Needed to perfectly align the DMA transfer.
    asm("ORL 0x87,#0x01"); // PCON |= 0x01;
    asm("NOP");
  } // End of timing critical code
  ////////////////////////////////////////////////////////////////////////////
  // Enable Flash Cache.
  MEMCTR &= ~0x02;
  // Update DMA channel 0 with original descriptor and arm channel if it was
  // in use before PM was entered.
  DMA0CFGH = storedDescHigh;
  DMA0CFGL = storedDescLow;
  DMAARM = 0x01;
}

/*==== PUBLIC FUNCTIONS ======================================================*/

void main(void)
{
  UINT8 seqNum;                       // Sequence number for TX packet
  UINT16 BatteryVolt;
  UINT16 count;
  
  UINT8  HS_flag=0;                   // Handshaking flag
  UINT16 Standby_TH=1024;             // Standby threshold updated wirelessly
  UINT32 CHANNR_TX=0;                 // TX frequency updated wirelessly
  UINT32 CHANNR_HS=0;
  UINT8  i=0;
  UINT8  Standby=0;
  UINT8  Standby_flag=0;
  UINT8  Standby_counter=0;
  UINT8  Standby_reset=0;
  
  delay_ms(1000);
  
  CLK_XOSC();
  
  
  // Initialize sensor
  init_Sensor();
  
  // Initilize watchdog
  WDTInit();
  
  // Initialize sleep timer
  STInit();  //sahadat
  
  InitSensorOutput();                                   // initialize LSM303D sensors
  
  
 // Initialize ADC
  init_ADC();      //never activate it because problem with the sensor CS4 sahadat
  
  // initialize LSDM9DS1
  InitLSM9DS1();
  
  
  // Set default frequency and data rate for handshaking
  radioConfigure(DATA_RATE_1_CC2510, FREQUENCY_1_CC2510);
  CHANNR = CHANNR_HS;
  
  // Set up the DMA to move packet data from buffer to radio
  dmaRadioSetup(RADIO_MODE_TX);
  
  // Configure interrupt for every time a packet is sent
  IEN2 |= 0x01;                                         // Enable RF general interrupt
  RFIM = IRQ_DONE;                                      // Mask IRQ_DONE flag only
  EA = 1;                                               // Enable interrupts globally
  
  IRCON &= ~0x80;                                       // Clear interrupt flag
  WORIRQ &= ~0x01;
  
  
  /*********************************************/
  /*********************************************/
  /****Hand shaking portion starts from here****/
  /*********************************************/
  /*********************************************/
  T1CTL |= 0x02;                  // sahadat                     // Starts the 1s timer
  HS_flag=0;
  
   while (HS_flag == 0)
  {
    /****************************************************/
    /****************************************************/
    /************Initiate Handshaking *******************/
    /****************************************************/
    /****************************************************/
    
    // Handshaking request packet (0xCCCC); receiver receives from radioPktBuffer[3]
    radioPktBuffer[0] = PACKET_LENGTH;                  // Length byte
    radioPktBuffer[1] = (BYTE) (NETWORK_ID_KEY>>8);     // Network identifier
    radioPktBuffer[2] = (BYTE) NETWORK_ID_KEY;
    radioPktBuffer[3] = 0xCC;
    radioPktBuffer[4] = 0xCC;
    
    CHANNR = CHANNR_HS;
    
    // Switch to TX mode;
    mode = RADIO_MODE_TX;
    // Set up the DMA to move packet data from buffer to radio
    dmaRadioSetup(RADIO_MODE_TX);
    
  
    // Transmit HS request packet 5 times
    for (i=0;i<5;i++)
    {
      // Send the packet with old frequency
      DMAARM = DMAARM_CHANNEL0;          // Arm DMA channel 1
      RFST = STROBE_TX;                  // Switch radio to TX
      
      while(!pktSentFlag);  //sahadat
      pktSentFlag = FALSE;
    }
   
    /****************************************************/
    /****************************************************/
    /*******Wait for parameter packet from receiver******/
    /****************************************************/
    /****************************************************/
    // Switch to RX mode;
    mode = RADIO_MODE_RX;
    // Set up the DMA to move packet data from radio to buffer
    dmaRadioSetup(RADIO_MODE_RX);
    
    IRCON &= ~0x80;                     //clear interrupt flag
    WORIRQ &= ~0x01;
    
    // Start receiving
    DMAARM = DMAARM_CHANNEL0;           // Arm DMA channel 0
    RFST   = STROBE_RX;                 // Switch radio to RX
    
    // Wait till either 1s timer is expired or a valid handshaking
    // parameter packet is received.
    while((!TimerExpires)&&(!HS_flag))
    {
      if (pktRcvdFlag) {
        pktRcvdFlag = FALSE;
        // Check if the received packet is a valid packet
        if (pktCheckValidity()) {
          // Check if the received packet is a handshaking parameter packet
          if ((radioPktBuffer[3]==0x5A) && (radioPktBuffer[4]==0xA5))
          {
            // if it is a handshaking packet, set the flag
            HS_flag = 1;
          }
        }
      }
    }
    
    TimerExpires = 0;
    DMAARM |= 0x81;                       // Disarm current DMA channel 0
    RFST = STROBE_IDLE;                   // Switch radio to IDLE
    delay_ms(1);
    
    // if have tried handshaking for > 60 times (60s)
    // enter PM02 and toggle LED
    if (TimerCounter >= 60)
    {
      RFST = STROBE_IDLE;                 // Switch radio to IDLE
      STInit_IDLE();                      //Initialize sleep timer
      
      while(1)
      {
        CLK_RCOSC();                      // Switch to High speed RC osc before entering PM2
        LV=LOW;
        delay_ms(5);
        LV=HIGH;
        Enter_PM2(); //sahadat
      }
    }
  }   
    
    /****************************************************/
    /****************************************************/
    /******************Send ACK packet*******************/
    /****************************************************/
    /****************************************************/
    
    // If handshaking packet is received, transmit ACK packet
    // Then update the parameter
    if (HS_flag)
    {
      // ACK packet
      // Delay_ms(10);
      radioPktBuffer[0] = PACKET_LENGTH;                  // Length byte
      radioPktBuffer[1] = (BYTE) (NETWORK_ID_KEY>>8);     // Network identifier
      radioPktBuffer[2] = (BYTE) NETWORK_ID_KEY;
      radioPktBuffer[3] = 0xA5;
      radioPktBuffer[4] = 0x5A;
      
      // Switch to TX mode;
      mode = RADIO_MODE_TX;
      
      // Set up the DMA to move packet data from buffer to radio
      dmaRadioSetup(RADIO_MODE_TX);
      
      // Transmit ACK packet 5 times
      for (i=0;i<5;i++)
      {
        // Send the packet with old frequency
        DMAARM |= DMAARM_CHANNEL0;  // Arm DMA channel 0
        RFST = STROBE_TX;           // Switch radio to TX
        
        while(!pktSentFlag);
        pktSentFlag = FALSE;
      }
      
      // Update the parameter
      // Update standby threshold (0-1024)
      Standby_TH = (radioPktBuffer[5]<<8)+radioPktBuffer[6];
      Stdby_Upth = 2048 + Standby_TH;
      Stdby_Lowth = 2048 - Standby_TH;
      
      CHANNR_TX = radioPktBuffer[7];
    }
    
    /****************************************************/
    /****************************************************/
    /****************************************************/
    /****************************************************/

  
  /*********************************************/
  /*********************************************/
  /************* Hand shaking ends here ********/
  /*********************************************/
  /*********************************************/
  
  
  mode = RADIO_MODE_TX;
  CHANNR = CHANNR_TX;
  
  // Set up the DMA to move packet data from buffer to radio
  dmaRadioSetup(RADIO_MODE_TX);
  
  // Construct the packet to be transmitted in buffer
  radioPktBuffer[0] = PACKET_LENGTH;                  // Length byte
  radioPktBuffer[1] = (BYTE) (NETWORK_ID_KEY>>8);     // Network identifier
  radioPktBuffer[2] = (BYTE) NETWORK_ID_KEY;
  
  IRCON &= ~0x80;  // Clear interrupt flag
  WORIRQ &= ~0x01;
  
  
  while (TRUE) {
    
    if(Standby==1)
    {
      //Standby the sleep timer at 1 second interval
      STInit_IDLE();                      //Initialize sleep timer
      Standby_counter = 0;
      Standby_reset = 0;
      
      while(Standby==1)
      {
        RFST = STROBE_IDLE;                 // Switch radio to IDLE
        CLK_RCOSC(); //Switch to High speed RC osc before entering PM2
        LV=LOW;
        delay_ms(5);
        LV=HIGH;
        Enter_PM2();    //sahadat
        
        //Reinialize clock
        CLK_XOSC();
        radioReconfigure();
        
        //Sample the sensor output at 1 Hz
        GetSensorOutput();
        //Check sensor output to decide to quit standby mode or not
        Standby_flag = 0;
        Standby_flag = Standby_Check();
        
        //Set the standby_reset after sensor output falls below threshold
        //It allows to detect another standby/on command.
        if (!Standby_flag)
        {
          Standby_counter = 0;
          Standby_reset = 1;
        }
        
        // Mask standby flag is standby_reset is not set.
        if(Standby_flag&&Standby_reset)
        {
          Standby_counter ++;
        }
        
        //if the sensor output is higher than threshlold
        //during 3 consecutive checks (3 secs), quit standby
        if(Standby_counter >= 3)
        {
          Standby = 0;
        }
        
        radioPktBuffer[3] = Standby;
        radioPktBuffer[4] = seqNum++;
        
        // Send the packet
        DMAARM |= DMAARM_CHANNEL0;  // Arm DMA channel 0
        RFST = STROBE_TX;           // Switch radio to TX
        
        // Wait until the radio transfer is completed,
        // and then reset pktSentFlag
        while(!pktSentFlag);
        pktSentFlag = FALSE;
        
      }
    }
    
    //Active mode
    else
    {
      //Reinialize clock
      CLK_XOSC();
      radioReconfigure();
      
      //Initialize watchdog
      WDTInit();
      
      //Initialize timer at 50 Hz rate. use Timer 1 to be more accurate
      init_Timer1_20ms();
      
      T1CTL |= 0x02;                      //Starts the 20ms timer
      seqNum = 0;
      
      // Mask the standby_flag until the sensor output falls below threhsold
      Standby_reset=0;
      
      while(Standby==0)
      {
        // Get battery voltage
        BatteryVolt=GetBatteryVoltage();
        
        if (BatteryVolt<=1400)
          count = count+1;
        else
          count = 0;
        
        if (count==50)
        {
          count = 0;
          LV=LOW;
        }
        
        // Get sensor output
        GetSensorOutput();
        
        Standby_flag=0;
        Standby_flag = Standby_Check();
        
        //Set the standby_reset after sensor output falls below threshold
        //It allows to detect another standby/on command.
        if (!Standby_flag)
        {
          Standby_counter = 0;
          Standby_reset = 1;
        }
        
        // Mask standby flag is standby_reset is not set.
        if(Standby_flag&&Standby_reset)
        {
          Standby_counter ++;
        }
        
        //if the sensor output is higher than threshlold
        //during 150 consecutive checks (3 sec), enter standby mode
        if(Standby_counter>=150)
        {
          Standby = 1;
        }
        
        // Set correct sequence number to packet
        radioPktBuffer[3] = Standby;
        radioPktBuffer[4] = seqNum++;
        radioPktBuffer[5]= (BYTE) (BatteryVolt>>8);
        radioPktBuffer[6]= (BYTE) (BatteryVolt);
        
        //Echo threshold and standby_counter for debugging purpose
        //          radioPktBuffer[7]= (BYTE) (Stdby_Upth>>8);
        //          radioPktBuffer[8]= (BYTE) (Stdby_Upth);
        //          radioPktBuffer[9]= (BYTE) (Stdby_Lowth>>8);
        //          radioPktBuffer[10]= (BYTE) (Stdby_Lowth);
        //          radioPktBuffer[11]= 0;
        //          radioPktBuffer[12]= Standby_counter;
        
        // Send the packet
        DMAARM |= DMAARM_CHANNEL0;  // Arm DMA channel 0
        RFST = STROBE_TX;           // Switch radio to TX
        
        // Wait until the radio transfer is completed,
        // and then reset pktSentFlag
        while(!pktSentFlag);
        pktSentFlag = FALSE;
        
        WDTCLR(); //Reset watchdog counter
        
        LV=HIGH;
        
        //      CLK_RCOSC(); //Switch to High speed RC osc before entering PM2
        //      Enter_PM2();
        
        //Enter PM0 which can be waken up by Timer 1
        SLEEP = (SLEEP & 0xFC) | 0x00;
        // Set PCON.IDLE to enter PM0
        PCON |= 0x01;
      }
    }
  }
}


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

/***********Sleep timer interrupt to recover from PM2 to active mode************/

#pragma vector=ST_VECTOR
__interrupt void ST_IRQ(void){
  IRCON &= ~0x80;             // clear interrupt flag
  WORIRQ &= ~0x01;
}

// Timer 1 interrupt
#pragma vector=T1_VECTOR
__interrupt void T1_IRQ(void){
  TimerExpires = 1;
  TimerCounter++;
}

/*==== END OF FILE ==========================================================*/
