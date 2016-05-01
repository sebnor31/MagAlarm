#ifndef __LSM9DS1_Registers_H__
#define __LSM9DS1_Registers_H__

/////////////////////////////////////////
// LSM9DS1_ Accel/Gyro (XL/G) Registers //
/////////////////////////////////////////
#define LSM9DS1_ACT_THS				0x04
#define LSM9DS1_ACT_DUR				0x05
#define LSM9DS1_INT_GEN_CFG_XL		0x06
#define LSM9DS1_INT_GEN_THS_X_XL	0x07
#define LSM9DS1_INT_GEN_THS_Y_XL	0x08
#define LSM9DS1_INT_GEN_THS_Z_XL	0x09
#define LSM9DS1_INT_GEN_DUR_XL		0x0A
#define LSM9DS1_REFERENCE_G			0x0B
#define LSM9DS1_INT1_CTRL			0x0C
#define LSM9DS1_INT2_CTRL			0x0D
#define LSM9DS1_WHO_AM_I_XG			0x0F
#define LSM9DS1_CTRL_REG1_G			0x10
#define LSM9DS1_CTRL_REG2_G			0x11
#define LSM9DS1_CTRL_REG3_G			0x12
#define LSM9DS1_ORIENT_CFG_G		0x13

/****************gyroscoper configuration values************/
#define LSM9DS1_CTRL_REG1_G_VAL		0XA0    //0XA3	//0b10100000 //0b10100011

/* ODR_G2 | ODR_G1 | ODR_G0 | FS_G1 | FS_G0 | 0 | BW_G1 | BW_G0
101(476Hz)00(245 dps)0(default)11() change to 00 for more noise*/

#define LSM9DS1_CTRL_REG2_G_VAL         0X00     //0b00000000

/* 0(1) | 0(1) | 0(1) | 0(1) | INT_SEL1 | INT_SEL0 | OUT_SEL1 | OUT_SEL0 

*/
#define LSM9DS1_CTRL_REG3_G_VAL	        0X00		//0b00000000

/*
LP_mode | HP_EN | 0(1) | 0(1) | HPCF3_G | HPCF2_G | HPCF1_G |HPCF0_G

*/
#define LSM9DS1_ORIENT_CFG_G_VAL        0X00		//0b00000000

/*

0(1)| 0(1) | SignX_G | SignY_G | SignZ_G | Orient_2 | Orient_1 | Orient_0
*/


#define LSM9DS1_CTRL_REG4_VAL   0X38		//0b00111000

/*
0(1) |0(1)|Zen_G | Yen_G | Xen_G | 0(1) | LIR_XL1 | 4D_XL1

*/

/*******************gyroscope configuration values ends******/



#define LSM9DS1_INT_GEN_SRC_G		0x14
#define LSM9DS1_OUT_TEMP_L			0x15
#define LSM9DS1_OUT_TEMP_H			0x16
#define LSM9DS1_STATUS_REG_0		0x17
#define LSM9DS1_OUT_X_L_G			0x18
#define LSM9DS1_OUT_X_H_G			0x19
#define LSM9DS1_OUT_Y_L_G			0x1A
#define LSM9DS1_OUT_Y_H_G			0x1B
#define LSM9DS1_OUT_Z_L_G			0x1C
#define LSM9DS1_OUT_Z_H_G			0x1D
#define LSM9DS1_CTRL_REG4			0x1E
#define LSM9DS1_CTRL_REG5_XL		0x1F
#define LSM9DS1_CTRL_REG6_XL		0x20
#define LSM9DS1_CTRL_REG7_XL		0x21

/******************** values for accel configuration*************************/
#define LSM9DS1_CTRL_REG5_XL_VAL        0X38		//0b00111000

/*
DEC_1 | DEC_0 | Zen_XL | Yen_XL | Xen_XL | 0(1) | 0(1) | 0(1)

*/
#define LSM9DS1_CTRL_REG6_XL_VAL	0X62    //0XA2	//0b01100010 //0b10100010

/*
ODR_XL2 | ODR_XL1 | ODR_XL0 | FS1_XL | FS0_XL | BW_SCAL_ODR | BW_XL1 | BW_XL0
101(476Hz)00(+-2G) 0(default) 10(105Hz)
011(119Hz)00(+-2G) 0(default) 10(105Hz)
*/

#define LSM9DS1_CTRL_REG7_XL_VAL	0X20 //0XA0	//0b10100000	// 0b00100000

/*
HR | DCF1 | DCF0 | 0(1) | 0(1) | FDS | 0(1) | HPIS1

1 | 01(100Hz)|00(default)|0|0
*/

/************end of accel configuration********************************/

#define LSM9DS1_CTRL_REG8			0x22
#define LSM9DS1_CTRL_REG9			0x23
#define LSM9DS1_CTRL_REG10			0x24
#define LSM9DS1_INT_GEN_SRC_XL		        0x26
#define LSM9DS1_STATUS_REG_1		        0x27
#define LSM9DS1_OUT_X_L_XL			0x28
#define LSM9DS1_OUT_X_H_XL			0x29
#define LSM9DS1_OUT_Y_L_XL			0x2A
#define LSM9DS1_OUT_Y_H_XL			0x2B
#define LSM9DS1_OUT_Z_L_XL			0x2C
#define LSM9DS1_OUT_Z_H_XL			0x2D
#define LSM9DS1_FIFO_CTRL			0x2E
#define LSM9DS1_FIFO_SRC			0x2F
#define LSM9DS1_INT_GEN_CFG_G		0x30
#define LSM9DS1_INT_GEN_THS_XH_G	0x31
#define LSM9DS1_INT_GEN_THS_XL_G	0x32
#define LSM9DS1_INT_GEN_THS_YH_G	0x33
#define LSM9DS1_INT_GEN_THS_YL_G	0x34
#define LSM9DS1_INT_GEN_THS_ZH_G	0x35
#define LSM9DS1_INT_GEN_THS_ZL_G	0x36
#define LSM9DS1_INT_GEN_DUR_G		0x37

///////////////////////////////
// LSM9DS1 Magneto Registers //
///////////////////////////////
#define LSM9DS1_OFFSET_X_REG_L_M	0x05
#define LSM9DS1_OFFSET_X_REG_H_M	0x06
#define LSM9DS1_OFFSET_Y_REG_L_M	0x07
#define LSM9DS1_OFFSET_Y_REG_H_M	0x08
#define LSM9DS1_OFFSET_Z_REG_L_M	0x09
#define LSM9DS1_OFFSET_Z_REG_H_M	0x0A
#define LSM9DS1_WHO_AM_I_M			0x0F
#define LSM9DS1_CTRL_REG1_M			0x20
#define LSM9DS1_CTRL_REG2_M			0x21
#define LSM9DS1_CTRL_REG3_M			0x22
#define LSM9DS1_CTRL_REG4_M			0x23
#define LSM9DS1_CTRL_REG5_M			0x24

/*****************configuration values for magnetometer*************/
#define LSM9DS1_CTRL_REG1_M_VAL		0X7C	//0b01111100
/*
TEMP_COMP | OM1 | OM0 | DO2 | DO1 | DO0 | 0(1) | ST
*/
#define LSM9DS1_CTRL_REG2_M_VAL		0X20	//0b00100000

/*
0(1) | FS1 | FS0 | 0(1) | REBOOT | SOFT_RST | 0(1) | 0(1)
0 | 01 (8 Gauss) | 0 (normal mode)| 0 (soft reset) | 0 | 0
*/

#define LSM9DS1_CTRL_REG3_M_VAL		0x00//0X84	//0b10000100

/*
I2C_DISABLE | 0(1)|LP | 0(1) | 0(1) | SIM | MD1 | MD0

*/

#define LSM9DS1_CTRL_REG4_M_VAL		0X0C	//0b00001100

/*
0(1)|0(1)|0(1) |0(1) |OMZ1 |OMZ0 |BLE |0

*/

#define LSM9DS1_CTRL_REG5_M_VAL		0x00//0X40	//0b00000000

/*
0(1)| BDU | 0(1) | 0(1) | 0(1) | 0(1) | 0(1) | 0(1)

*/

/**********end of magnetometer configuration**************/



#define LSM9DS1_STATUS_REG_M		        0x27
#define LSM9DS1_OUT_X_L_M			0x28
#define LSM9DS1_OUT_X_H_M			0x29
#define LSM9DS1_OUT_Y_L_M			0x2A
#define LSM9DS1_OUT_Y_H_M			0x2B
#define LSM9DS1_OUT_Z_L_M			0x2C
#define LSM9DS1_OUT_Z_H_M			0x2D
#define LSM9DS1_INT_CFG_M			0x30
#define LSM9DS1_INT_SRC_M			0x30
#define LSM9DS1_INT_THS_L_M			0x32
#define LSM9DS1_INT_THS_H_M			0x33

////////////////////////////////
// LSM9DS1 WHO_AM_I Responses //
////////////////////////////////
#define LSM9DS1_WHO_AM_I_AG_RSP		0x68
#define LSM9DS1_WHO_AM_I_M_RSP		0x3D

#endif