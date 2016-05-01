/**************************************************************************//**
    @file       magneticsensor.h

    @brief      Header file for LSM303D. @Note This header file
                does not include all register addresses for the LSM303D.

******************************************************************************/
#ifndef magneticsensor_H
#define magneticsensor_H


/******************************************************************************
 * INCLUDES
 */
#include "hal_types.h"

/******************************************************************************
 * DEFINES
 */

// BMA250 addressing space
#define MGT_CHIPID                  0x00    // Always 0x03
#define MGT_X_LSB                   0x08    // ACC_X_LSB[7:6] = 2 LSb of X acceleration data sahadat
#define MGT_X_MSB                   0x09    // ACC_X_MSB[7:0] = 8 MSb of X data sahadat
#define MGT_Y_LSB                   0x0A        //sahadat
#define MGT_Y_MSB                   0x0B        // sahadat
#define MGT_Z_LSB                   0x0C        // sahadat
#define MGT_Z_MSB                   0x0D        // sahadat

#define ACC_X_LSB                   0x28    // LSM303D accelerometer data registers
#define ACC_X_MSB                   0x29    
#define ACC_Y_LSB                   0x2A        
#define ACC_Y_MSB                   0x2B        
#define ACC_Z_LSB                   0x2C        
#define ACC_Z_MSB                   0x2D       
   
   
   
#define MGT_CTRL0                   0x1F
#define MGT_CTRL1                   0x20
#define MGT_CTRL2                   0x21
#define MGT_CTRL3                   0x22
#define MGT_CTRL4                   0x23
#define MGT_CTRL5                   0x24
#define MGT_CTRL6                   0x25
#define MGT_CTRL7                   0x26
#define MGT_INT_CTRL_M              0x12

#define MGT_CTRL0_VAL               0x00        // CHECK THE VALUES IN THE DATASHEET OF LSM303D TO CHANGE THE CONFIGURATION
#define MGT_CTRL1_VAL               0x6F        //0x5F        // not updated untill the value is read--- 50Hz power mode 0110(100Hz)1(contineous updat)1(AZEN)1(AYEN)1(AZEN)
#define MGT_CTRL2_VAL               0x40        //0xC0        // SPI 4 wire interface 50Hz antialias filter for accelerometer, +-2g // 01(194Hz anti alias)000(+-2G)0(default)0(selftest disable)0(SPI 4 wire)
#define MGT_CTRL3_VAL               0x00        // nothing no interrupt
#define MGT_CTRL4_VAL               0x00        // no interrupt
#define MGT_CTRL5_VAL               0x74            //0x70        // 50Hz magnetic resolution ; high resoulution // 0 (temp dissable)11(high resolution magnetic data)101(100Hz magnetic data)0(LIR2)0(LIR1)
#define MGT_CTRL6_VAL               0x40        // +- 8gauss for the magnetic resolution //0(default)10(+-8Gauss)00000(default)
#define MGT_CTRL7_VAL               0x00        // contineous magnetic sensor mode
#define MGT_INT_CTRL_M_VAL          0x00           //0xE0

#define MGT_INT_ENABLE0             0x16  // change it if you want to use it as interrupt basis
#define MGT_INT_ENABLE1             0x17  // change it if you want to use it as interrupt basis

// Range selection definitions
#define ACC_RANGE_2G                0x18    //  3.91 mg/LSB sahadat
#define ACC_RANGE_4G                0x05    //  7.81 mg/LSB
#define ACC_RANGE_8G                0x08    // 15.62 mg/LSB
#define ACC_RANGE_16G               0x0C    // 31.25 mg/LSB

// Filtered bandwidth selection (delta_t = time between successive acc samples)
#define ACC_BW_7_81HZ               0x08    // 7.81Hz bandwidth (delta_t = 64 ms)
#define ACC_BW_15_63HZ              0x09    // delta_t = 32   ms
#define ACC_BW_31_25HZ              0x0A    // delta_t = 16   ms
#define ACC_BW_62_5HZ               0x0B    // delta_t =  8   ms
#define ACC_BW_125HZ                0x0C    // delta_t =  4   ms
#define ACC_BW_250HZ                0x00    // delta_t =  2   ms sahadat
#define ACC_BW_500HZ                0x0E    // delta_t =  1   ms
#define ACC_BW_1000HZ               0x0F    // delta_t =  0.5 ms

#define ACC_PM_SUSP                 0x80    // Power mode register (0x11), bit 7
#define ACC_PM_LP                   0x40    // Low power mode
#define ACC_PM_SLEEP_10MS           0x14
#define ACC_PM_SLEEP_25MS           0x16
#define ACC_PM_SLEEP_50MS           0x18

// Interrupt enable bitmasks (for use with registers ACC_INT_ENABLEx [x=0,1] )
#define ACC_INT_FLAT_EN             0x80    // Bit in register 0x16
#define ACC_INT_ORIENT_EN           0x40    //          "
#define ACC_INT_S_TAP_EN            0x20    //          "
#define ACC_INT_D_TAP_EN            0x10    //          "
#define ACC_INT_SLOPE_Z_EN          0x04    //          "
#define ACC_INT_SLOPE_Y_EN          0x02    //          "
#define ACC_INT_SLOPE_X_EN          0x01    //          "
#define ACC_INT_DATA_EN             0x10    // Bit in register 0x17
#define ACC_INT_LOW_EN              0x08    //          "
#define ACC_INT_HIGH_Z_EN           0x04    //          "
#define ACC_INT_HIGH_Y_EN           0x02    //          "
#define ACC_INT_HIGH_X_EN           0x01    //          "

// Interrupt mapping bitmasks (for use with registers ACC_INT_MAPPINGx [x=0,1,2] )
#define ACC_INT_MAP_FLAT            0x80    // For pin INT1 (INT2), bit in register 0x19 (0x1B)
#define ACC_INT_MAP_ORIENT          0x40    //                   "
#define ACC_INT_MAP_S_TAP           0x20    //                   "
#define ACC_INT_MAP_D_TAP           0x10    //                   "
#define ACC_INT_MAP_SLOPE           0x04    //                   "
#define ACC_INT_MAP_HIGH            0x02    //                   "
#define ACC_INT_MAP_LOW             0x01    //                   "
#define ACC_INT1_MAP_DATA           0x01    // New data IRQ to pin INT1, bit in register 0x1A
#define ACC_INT2_MAP_DATA           0x80    // New data IRQ to pin INT2, bit in register 0x1A

// Interrupt source bitmasks (for use with register ACC_INT_SOURCE)
#define ACC_INT_SRC_DATA_FILT       0x20
#define ACC_INT_SRC_TAP_FILT        0x01
#define ACC_INT_SRC_SLOPE_FILT      0x04
#define ACC_INT_SRC_HIGH_FILT       0x02
#define ACC_INT_SRC_LOW_FILT        0x01

// Interrupt pin behavior bitmasks (for use with register (Open drive/push-pull and active level 0/1)
#define ACC_INT2_OD                 0x08
#define ACC_INT2_LVL                0x04
#define ACC_INT1_OD                 0x02
#define ACC_INT1_LVL                0x01

// Perform soft reset
#define ACC_SOFTRESET_EN            0xB6    // Soft reset by writing 0xB6 to softreset register
/*sahadat 
*/ 

/******************************************************************************
 * MACROS
 */
// Wait 1 [ms]
#define WAIT_1MS()      {for(unsigned short i=0;i<32000;i++)asm("NOP"); }

// Wait t [ms]
#define WAIT_MS(t)                      \
    do{                                 \
        for(uint8 i = 0; i<t; i++)      \
            WAIT_1MS();                 \
    }while(0)


/******************************************************************************
 * FUNCTION PROTOTYPES
 */
void mgtInit(void);
void mgtStop(uint8 sennum);
void mgtWriteReg(uint8 reg, uint8 val, uint8 sennnum);
void mgtReadReg(uint8 reg, uint8 *pVal, uint8 sennnum);
//void mgtReadAcc(int8 *pX1Val, int8 *pY1Val, int8 *pZ1Val,int8 *pX2Val, int8 *pY2Val, int8 *pZ2Val, uint8 sennum);
uint16 * mgtReadAcc(uint8 sennum);
void mgtReadAcc16(int16 *pXVal, int16 *pYVal, int16 *pZVal, uint8 sennnum);
void LSM9DWriteReg(uint8 reg, uint8 val, uint8 sennum);
uint16 * ReadLSM9DS1(uint8 sennum, uint8 reg_add);
uint16 * LSM3DAcc( uint8 sennum);

/******************************************************************************

written by Md Nazmus Sahadat
GTBionics Laboratory
******************************************************************************/


#endif
