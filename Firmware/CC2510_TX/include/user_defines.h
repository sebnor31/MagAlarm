/*==== DECLARATION CONTROL ==================================================*/

#ifndef HAL_DEFINES_H
#define HAL_DEFINES_H

/*==== INCLUDES ==============================================================*/
/*==== CONSTS ================================================================*/

/* Common values */
/* ============= */
#ifndef FALSE
   #define FALSE 0
#endif

#ifndef TRUE
   #define TRUE 1
#endif

#ifndef NULL
   #define NULL 0
#endif

#ifndef HIGH
   #define HIGH 1
#endif

#ifndef LOW
   #define LOW 0
#endif


// Pin
#define PIN7     0x80
#define PIN6     0x40
#define PIN5     0x20
#define PIN4     0x10
#define PIN3     0x08
#define PIN2     0x04
#define PIN1     0x02
#define PIN0     0x01

// I/O pins
#define MR_RST_1  P1_3  //out
#define MUX_D0_1  P1_4  //out
#define MUX_D1_1  P1_5  //out

#define MR_RST_2  P0_7  //out
#define MUX_D0_2  P2_3  //out
#define MUX_D1_2  P2_4  //out

#define MR_MUX    P1_0  //out
#define MR_PWR    P0_3  //out
#define MR_OUT    P0_0  //in


/******************************************************************************
*******************              Chip revisions             *******************
******************************************************************************/
#define REV_A   0x00
#define REV_B   0x01
#define REV_C   0x02
#define REV_D   0x03
#define REV_E   0x04

/******************************************************************************
*******************              Specific values            *******************
******************************************************************************/
#define NBR_OF_INTERRUPTS 18

/*==== TYPES =================================================================*/

/* Common types */
/* ============= */

/* Boolean */
typedef unsigned char       BOOL;

/* Data */
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef unsigned long       DWORD;

/* Unsigned numbers */
typedef unsigned char       UINT8;
typedef unsigned short      UINT16;
typedef unsigned long       UINT32;

/* Signed numbers */
typedef signed char         INT8;
typedef signed short        INT16;
typedef signed long         INT32;


/*==== EXPORTS ===============================================================*/


/*==== MACROS ===============================================================*/

/******************************************************************************
*******************        Bit, byte and word macros        *******************
******************************************************************************/

// Bit mask
#define BM( b )       ( 0x01 << ( b ))

#define HIBYTE(a)     (BYTE) ((WORD)(a) >> 8 )
#define LOBYTE(a)     (BYTE)  (WORD)(a)

#define SET_WORD(regH, regL, word) \
   do{                             \
      (regH) = HIBYTE( word );     \
      (regL) = LOBYTE( word );     \
   }while(0)

// Macro to read a word out
// Must not be used for all registers as e.g. Timer1 and Timers2 require that regL is read first
#define GET_WORD(regH, regL, word) \
   do{                             \
      word = (WORD)regH << 8;      \
      word |= regL;                \
   }while(0)


#endif /* HAL_DEFINES_H */

/*==== END OF FILE ==========================================================*/










