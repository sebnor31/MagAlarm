/*==== DECLARATION CONTROL ===================================================*/
/*==== INCLUDES ==============================================================*/

#include "per_test_dma.h"
#include "per_test_main.h"
#include "ioCC2510.h"


/*==== GLOBAL VARS============================================================*/

BYTE radioPktBuffer[PACKET_LENGTH + 3];
BYTE filePktBuffer[PACKET_LENGTH_FILE + 3];


/*==== PUBLIC FUNCTIONS ======================================================*/

/******************************************************************************
* @fn  dmaRadioSetup
*
* @brief
*      This funtion configures the static dmaConfig struct with the correct
*      values for receiving or transmitting data using the DMA.
*
* Parameters:
*
* @param  BYTE mode
*           Either RADIO_MODE_TX or RADIO_MODE_RX to select transmitter or
*           receiver settings
*
* @return void
*
******************************************************************************/
void dmaRadioSetup(BYTE mode)
{
    // Some configuration that are common for both TX and RX:

    // CPU has priority over DMA
    // Use 8 bits for transfer count
    // No DMA interrupt when done
    // DMA triggers on radio
    // Single transfer per trigger.
    // One byte is transferred each time.
    dmaConfig.PRIORITY       = DMA_PRI_LOW;
    dmaConfig.M8             = DMA_M8_USE_8_BITS;
    dmaConfig.IRQMASK        = DMA_IRQMASK_DISABLE;
    dmaConfig.TRIG           = DMA_TRIG_RADIO;
    dmaConfig.TMODE          = DMA_TMODE_SINGLE;
    dmaConfig.WORDSIZE       = DMA_WORDSIZE_BYTE;

    if (mode == RADIO_MODE_TX) {
        // Transmitter specific DMA settings

        // Source: radioPktBuffer
        // Destination: RFD register
        // Use the first byte read + 1
        // Sets the maximum transfer count allowed (length byte + data)
        // Data source address is incremented by 1 byte
        // Destination address is constant
        SET_WORD(dmaConfig.SRCADDRH, dmaConfig.SRCADDRL, radioPktBuffer);
        SET_WORD(dmaConfig.DESTADDRH, dmaConfig.DESTADDRL, &X_RFD);
        dmaConfig.VLEN           = DMA_VLEN_FIRST_BYTE_P_1;
        SET_WORD(dmaConfig.LENH, dmaConfig.LENL, (PACKET_LENGTH + 1));
        dmaConfig.SRCINC         = DMA_SRCINC_1;
        dmaConfig.DESTINC        = DMA_DESTINC_0;

    }
    else if (mode == RADIO_MODE_RX) {
        // Receiver specific DMA settings:

        // Source: RFD register
        // Destination: radioPktBuffer
        // Use the first byte read + 3 (incl. 2 status bytes)
        // Sets maximum transfer count allowed (length byte + data + 2 status bytes)
        // Data source address is constant
        // Destination address is incremented by 1 byte for each write
        SET_WORD(dmaConfig.SRCADDRH, dmaConfig.SRCADDRL, &X_RFD);
        SET_WORD(dmaConfig.DESTADDRH, dmaConfig.DESTADDRL, radioPktBuffer);
        dmaConfig.VLEN           = DMA_VLEN_FIRST_BYTE_P_3;
        SET_WORD(dmaConfig.LENH, dmaConfig.LENL, (PACKET_LENGTH + 3));
        dmaConfig.SRCINC         = DMA_SRCINC_0;
        dmaConfig.DESTINC        = DMA_DESTINC_1;
    }

    // Save pointer to the DMA configuration struct into DMA-channel 0
    // configuration registers
    SET_WORD(DMA0CFGH, DMA0CFGL, &dmaConfig);

    return;
}


//Setup DMA channel 1 to transfer file data between buffer and RFD register
void dmaFileSetup(BYTE mode)
{
    // Some configuration that are common for both TX and RX:

    // CPU has priority over DMA
    // Use 8 bits for transfer count
    // No DMA interrupt when done
    // DMA triggers on radio
    // Single transfer per trigger.
    // One byte is transferred each time.
    dmaConfigFile.PRIORITY       = DMA_PRI_LOW;
    dmaConfigFile.M8             = DMA_M8_USE_8_BITS;
    dmaConfigFile.IRQMASK        = DMA_IRQMASK_DISABLE;
    dmaConfigFile.TRIG           = DMA_TRIG_RADIO;
    dmaConfigFile.TMODE          = DMA_TMODE_SINGLE;
    dmaConfigFile.WORDSIZE       = DMA_WORDSIZE_BYTE;

    if (mode == RADIO_MODE_TX) {
        // Transmitter specific DMA settings

        // Source: filePktBuffer
        // Destination: RFD register
        // Use the first byte read + 1
        // Sets the maximum transfer count allowed (length byte + data)
        // Data source address is incremented by 1 byte
        // Destination address is constant
        SET_WORD(dmaConfigFile.SRCADDRH, dmaConfigFile.SRCADDRL, filePktBuffer);
        SET_WORD(dmaConfigFile.DESTADDRH, dmaConfigFile.DESTADDRL, &X_RFD);
        dmaConfigFile.VLEN           = DMA_VLEN_FIRST_BYTE_P_1;
        SET_WORD(dmaConfigFile.LENH, dmaConfigFile.LENL, (PACKET_LENGTH_FILE + 1));
        dmaConfigFile.SRCINC         = DMA_SRCINC_1;
        dmaConfigFile.DESTINC        = DMA_DESTINC_0;

    }
    else if (mode == RADIO_MODE_RX) {
        // Receiver specific DMA settings:

        // Source: RFD register
        // Destination: filePktBuffer
        // Use the first byte read + 3 (incl. 2 status bytes)
        // Sets maximum transfer count allowed (length byte + data + 2 status bytes)
        // Data source address is constant
        // Destination address is incremented by 1 byte for each write
        SET_WORD(dmaConfigFile.SRCADDRH, dmaConfigFile.SRCADDRL, &X_RFD);
        SET_WORD(dmaConfigFile.DESTADDRH, dmaConfigFile.DESTADDRL, filePktBuffer);
        dmaConfigFile.VLEN           = DMA_VLEN_FIRST_BYTE_P_3;
        SET_WORD(dmaConfigFile.LENH, dmaConfigFile.LENL, (PACKET_LENGTH_FILE + 3));
        dmaConfigFile.SRCINC         = DMA_SRCINC_0;
        dmaConfigFile.DESTINC        = DMA_DESTINC_1;
    }

    // Save pointer to the DMA configuration struct into DMA-channel 1
    // configuration registers
    SET_WORD(DMA1CFGH, DMA1CFGL, &dmaConfigFile);

    return;
}

/*==== END OF FILE ==========================================================*/
