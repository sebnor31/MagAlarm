#include "ioCC2510.h"
#include "per_test_main.h"

BYTE uartRxBuffer[SIZE_OF_UART_RX_BUFFER];
BYTE uartRxIndex;
BYTE uartRxFlag;

// This function maps/connects the UART to the desired SoC I/O port.
// The application should call this function with "uartPortAlt" = 1 or 2,
// and "uartNum" = 0 or 1.
void uartMapPort(unsigned char uartPortAlt, unsigned char uartNum) {
  // If UART Port Alternative 1 desired
  if(uartPortAlt == 1) {
    // If UART0 desired
    if (uartNum == 0) {
      // Configure UART0 for Alternative 1 => Port P0 (PERCFG.U0CFG = 0)
      PERCFG &= ~0x01;
      // Configure relevant Port P0 pins for peripheral function:
      // P0SEL.SELP0_2/3/4/5 = 1 => RX = P0_2, TX = P0_3, CT = P0_4, RT = P0_5
      P0SEL |= 0x3C;
      // Configure relevant Port P1 pins back to GPIO function
      P1SEL &= ~0x3C;
      // Else (UART1 desired)
    } else {
      // Configure UART1 for Alternative 1 => Port P0 (PERCFG.U1CFG = 0)
      PERCFG &= ~0x02;
      // Configure relevant Port P0 pins for peripheral function:
      // P0SEL.SELP0_2/3/4/5 = 1 => CT = P0_2, RT = P0_3, TX = P0_4, RX = P0_5
      P0SEL |= 0x3C;
      // Configure relevant Port P1 pins back to GPIO function
      P1SEL &= ~0xF0;
    }
    // Else (UART Port Alternative 2 desired)
  } else {
    // If UART0 desired
    if (uartNum == 0) {
      // Configure UART0 for Alternative 2 => Port P1 (PERCFG.U0CFG = 1)
      PERCFG |= 0x01;
      // P1SEL.SELP1_2/3/4/5 = 1 => CT = P1_2, RT = P1_3, RX = P1_4, TX = P1_5
      P1SEL |= 0x3C;
      // Configure relevant Port P0 pins back to GPIO function
      P0SEL &= ~0x3C;
      // Else (UART1 desired)
    } else {
      // Configure UART1 for Alternative 2 => Port P1 (PERCFG.U1CFG = 1)
      PERCFG |= 0x02;
      // P1SEL.SELP1_4/5/6/7 = 1 => CT = P1_4, RT = P1_5, TX = P1_6, RX = P1_7
      P1SEL |= 0xF0;
      // Configure relevant Port P0 pins back to GPIO function
      P0SEL &= ~0x3C;
    }
  }
}



// This function initializes the UART RX session, by simply enabling the
// corresponding UART interrupt, and leave the sample reception to the
// UART ISR. Before this function is called the application must initialize
// the UART peripheral

// The code implements the following steps:
// 1. Initialize the UART RX buffer index.
// 2. Clear UART RX Interrupt Flag (TCON.URXxIF = 0)
// 3. Enable UART RX and Interrupt (IEN0.URXxIE = 1, UxCSR.RE = 1)
// 4. Enable global interrupt (IEN0.EA = 1)

void uartStartRxForIsr(unsigned char uartNum) {
  uartRxIndex = 0;
  if (uartNum == 0) {
    URX0IF = 0;
    U0CSR |= 0x40;
    IEN0 |= 0x04;
  } else {
    URX1IF = 0;
    U1CSR |= 0x40;
    IEN0 |= 0x08;
  }
}

// Disable the uart interrupt
void uartStopRxForIsr(unsigned char uartNum) {
  if (uartNum == 0) {
    IEN0 &= ~0x04;
  } else {
    IEN0 &= ~0x08;
  }
}

void uartInit (unsigned char uartNum) {
  if (uartNum == 0)
  {
    U0CSR |= 0x80;  //Select UART mode
    U0GCR = 15;
    U0BAUD = 34;  //Set baud rage 921600
  }
  else
  {
    U1CSR |= 0x80;  //Select UART mode
//    U1GCR = 15;
//    U1BAUD = 34;  //Set baud rage 921600

  U1GCR = 12;
  U1BAUD = 34;  //Set baud rage 115200
  }
}
