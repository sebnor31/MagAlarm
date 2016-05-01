
/*==== DECLARATION CONTROL ==================================================*/

#ifndef PER_TEST_MAIN_H
#define PER_TEST_MAIN_H


/*==== INCLUDES ==============================================================*/
#include "per_test_dma.h"


#define STROBE_TX                    0x03   // Strobe commands for the RFST
#define STROBE_RX                    0x02   // register
#define STROBE_IDLE                  0x04   // IDLE Strobe

#define IRQ_DONE                     0x10   // The IRQ_DONE bit in the RFIF-
// and RFIM-register
#define DMAARM_CHANNEL0              0x01   // The value to arm the DMA
// channel 0 in the DMAARM register

#define DMAARM_CHANNEL1              0X02

#define NUMBER_OF_MODES              2      // Operational mode constants
#define RADIO_MODE_TX                0x10
#define RADIO_MODE_RX                0x20


/* Some adjustable settings */
#define PACKET_LENGTH                54//42 //36//54//42//36//30//42//36//30//42//54           //54   for 24 sensors  // Payload length. Does not include
// 1 length byte (prefixing payload,
// containing this value) and 2
// appended bytes CRC. Does include
// 2 bytes network identifier and 4
// bytes sequence number, hence
// minimum value is 6.
#define RSSI_AVG_WINDOW_SIZE         32     // Size of ring buffer for RSSI
// values to average over (sliding
// window). Max 256

#define NETWORK_ID_KEY               0x5AA5 // Network ID key that identifies
// transmitter/receiver pair


/* Some NOT SO adjustable settings ** See also LOCALS section if manipulated */

// Preset frequency alternatives

#define NUMBER_OF_FREQUENCIES_CC2510  4
#define FREQUENCY_1_CC2510      2480000     // kHz. NOTE: If you want to alter
#define FREQUENCY_2_CC2510      2460000     // these values you will also have
#define FREQUENCY_3_CC2510      2440000     // to modify the register settings in
#define FREQUENCY_4_CC2510      2420000     // radioConfigure() in
// per_test_radio.c
#define NUMBER_OF_DATA_RATES_CC2510   3
#define DATA_RATE_1_CC2510       500000     // bps. NOTE: If you alter these
#define DATA_RATE_2_CC2510       250000     // values you will also have to
#define DATA_RATE_3_CC2510        10000     // modify register settings in
// radioConfigure() in
// per_test_radio.c

/*==== MACROS ================================================================*/
/*==== TYPES =================================================================*/
/*==== GLOBALS ================================================================*/
/*====  GLOBAL VARS ==========================================================*/



// Other global variables
extern BYTE radioPktBuffer[PACKET_LENGTH + 3];  // Buffer for packets to send or receive,
// sized to match the receiver's needs

//extern UINT16 SENCAL[12];  //calibration parameter sahadat 

static BOOL pktSentFlag = FALSE;            // Flag set whenever a packet is sent
static BOOL pktRcvdFlag = FALSE;            // Flag set whenever a packet is received
static BYTE mode;                           // Radio operating mode, either RX or TX

static DMA_DESC dmaConfig;                  // Struct for the DMA configuration

// Receiver variables for PER and RSSI statistics
extern UINT32 perRcvdSeqNum;            // The sequence number of the last
// received packet
extern UINT32 perExpectedSeqNum;        // The expected sequence number of the
// next packet
extern UINT32 perBadPkts;               // The total number of packets received
// with correct ID, but wrong CRC
extern UINT32 perRcvdPkts;              // The total number of received packets
// with correct ID, regardless of CRC
static INT16 perRssiOffset;             // RSSI offset for receiver, depends on
// chip model and data rate
static INT16 perRssiBuf[RSSI_AVG_WINDOW_SIZE] = {0};    // Ring buffer for RSSI
// values used for (sliding window)
// averaging
static UINT8 perRssiBufCounter;         // Counter to keep track of the oldest/
// newest byte in RSSI ring buffer
static INT16 perRssiSum;                // Sum of all RSSI values in buffer,
// as absolute RSSI value

/*==== FUNCTIONS =============================================================*/

/* See dma_config.c for description */
extern void dmaRadioSetup(UINT8 mode);

/* See radio_config.c for description */
extern void radioConfigure(UINT32 dataRate, UINT32 frequency);
extern void radioReconfigure(void);
extern void pktSetSeqNum(UINT16 seqNum);
extern UINT16 pktGetSeqNum(void);
extern BOOL pktCheckId(void);
extern BOOL pktCheckCrc(void);
extern BOOL pktCheckValidity(void);
extern INT16 convertRssiByte(BYTE RSSI_value);

/* see eTDS_4sensor.c */
extern void init_Sensor(void);
extern void InitLSM9DS1(void);
extern void init_ADC(void);
extern void InitSensorOutput(void);
extern void GetSensorOutput(void);
extern void GetSensorCal(void);    // sensor calibration data
extern UINT16 GetBatteryVoltage(void);

/*******************delay functions***************/

void delay_us(UINT16 time);
void delay_10us(UINT16 time);
void delay_ms(UINT16 time);

#endif  /* PER_TEST_MAIN_H */

/*==== END OF FILE ==========================================================*/
