/*
 * mpu6050.c
 *
 *  Created on: 2022. okt. 8.
 *      Author: NUC
 */

/********************************************************************************
Product: MPU6050
Module:
Created: 4/6/2015, by KienLTb
Description: Driver MPU6050 for MSP430
********************************************************************************/
/*-----------------------------------------------------------------------------*/
/* Header inclusions */
/*-----------------------------------------------------------------------------*/
#include <msp430>
#include <math.h> // For atan();
#include "mpu6050.h"

/*-----------------------------------------------------------------------------*/
/* Local Constant definitions */
/*-----------------------------------------------------------------------------*/
/* AFS_SEL | Full Scale Range | LSB Sensitivity
* --------+------------------+----------------
* 0       | +/- 2g           | 16384 LSB/mg
* 1       | +/- 4g           | 8192 LSB/mg
* 2       | +/- 8g           | 4096 LSB/mg
* 3       | +/- 16g          | 2043 LSB/mg
*/
#define SCALED_ACC_2G       16384
#define SCALED_ACC_4G       8192
#define SCALED_ACC_8G       4096
#define SCALED_ACC_16G      2043

/* FS_SEL | Full Scale Range   | LSB Sensitivity
* -------+--------------------+----------------
* 0      | +/- 250 degrees/s  | 131 LSB/deg/s
* 1      | +/- 500 degrees/s  | 65.5 LSB/deg/s
* 2      | +/- 1000 degrees/s | 32.8 LSB/deg/s
* 3      | +/- 2000 degrees/s | 16.4 LSB/deg/s
*/
#define SCALED_GYRO_250     131.0
#define SCALED_GYRO_500     65.5
#define SCALED_GYRO_1000    32.8
#define SCALED_GYRO_2000    16.4

/*
*   Note:
*          |   ACCELEROMETER    |           GYROSCOPE
* DLPF_CFG | Bandwidth | Delay  | Bandwidth | Delay  | Sample Rate
* ---------+-----------+--------+-----------+--------+-------------
* 0        | 260Hz     | 0ms    | 256Hz     | 0.98ms | 8kHz
* 1        | 184Hz     | 2.0ms  | 188Hz     | 1.9ms  | 1kHz
* 2        | 94Hz      | 3.0ms  | 98Hz      | 2.8ms  | 1kHz
* 3        | 44Hz      | 4.9ms  | 42Hz      | 4.8ms  | 1kHz
* 4        | 21Hz      | 8.5ms  | 20Hz      | 8.3ms  | 1kHz
* 5        | 10Hz      | 13.8ms | 10Hz      | 13.4ms | 1kHz
* 6        | 5Hz       | 19.0ms | 5Hz       | 18.6ms | 1kHz
* 7        |   -- Reserved --   |   -- Reserved --   | Reserved
*/


/*-----------------------------------------------------------------------------*/
/* Local Macro definitions */
/*-----------------------------------------------------------------------------*/
// Note: This Macro use for Bigendian but MSP430 is little endien => revert
#define CONVERT_TO_16BIT(MSB, LSB)      (((WORD)(MSB) << 8) | (WORD)(LSB))
#define ABS(x) (x < 0 ? -x : x)
/*-----------------------------------------------------------------------------*/
/* Local Data type definitions */
/*-----------------------------------------------------------------------------*/
#define dt 0.015f
/*-----------------------------------------------------------------------------*/
/* Global variables */
/*-----------------------------------------------------------------------------*/

/* Offset value to calibrate Gyro */
static int16 Gyro_OffsetValueX = 0;
static int16 Gyro_OffsetValueY = 0;
static int16 Gyro_OffsetValueZ = 0;

/* Off set value to calibrate Acc*/
static int16 Acc_OffsetValueX = 0;
static int16 Acc_OffsetValueY = 0;
static int16 Acc_OffsetValueZ = 0;

/* Scale Value config for ACC - default is 2G*/
static float Acc_scaleValue  = SCALED_ACC_2G;

/* Scale Value config for GYRO - default is 2G*/
static float Gyro_scaleValue = SCALED_GYRO_250;

/*-----------------------------------------------------------------------------*/
/* Function prototypes */
/*-----------------------------------------------------------------------------*/

VOID MPU6050_Init(BYTE ACC_SCALE_CONFIG, BYTE GYRO_SCALE_CONFIG);

BYTE MPU6050_CheckI2C(VOID);

/* MPU6050 test configure of register*/
BYTE MPU6050_TestRegConfig(VOID);

VOID MPU6050_Calibrate_Gyro(VOID);
/* Raw Acc Value*/
VOID MPU6050_GetAccValueRaw(PVOID pValue);

/* Value in degree/s */
VOID PMU6050_AccConvertData(ACC_DATA_RAW rawValue, PVOID scaledData);

/* Raw Gyro Value to m/s^2*/
VOID MPU6050_GetGyroValueRaw(PVOID pValue);

/* Convert to m/s^2*/
VOID PMU6050_GyroConvertData(GYRO_DATA_RAW rawValue, PVOID scaledData);

/*-----------------------------------------------------------------------------*/
/* Function implementations                                                    */
/*-----------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------
Function    : MPU6050_Init
Purpose     : Init and configure for MPU6050
Parameters  : None
Return      : Note
--------------------------------------------------------------------------------*/
VOID MPU6050_Init(BYTE ACC_SCALE_CONFIG, BYTE GYRO_SCALE_CONFIG)
{

        I2C_WriteByte(0x07, MPU6050_ADDRESS, MPU6050_SIGNAL_PATH_RESET);

        // Reset MPU6050;
        I2C_WriteByte(DEVICE_RESET, MPU6050_ADDRESS, MPU6050_PWR_MGMT_1);

        //Set sample rate
        I2C_WriteByte(SET_SAMPLE_RATE_1000HZ, MPU6050_ADDRESS, MPU6050_SMPLRT_DIV);

        //Config Low pass filter
        I2C_WriteByte(EXT_SYNC_SET_INPUT_DISABLE + DLPF_CFG_BAND_WIDTH_10HZ, MPU6050_ADDRESS, MPU6050_CONFIG);

        //Config Accel
        BYTE AccConfig;
        switch(ACC_SCALE_CONFIG)
        {
            case  ACC_CONFIG_2G:
                Acc_scaleValue =   SCALED_ACC_2G;
                AccConfig      =   AFS_SEL_SCALE_2G;
                break;
            case  ACC_CONFIG_4G:
                Acc_scaleValue =   SCALED_ACC_4G;
                AccConfig      =   AFS_SEL_SCALE_4G;
                break;
            case  ACC_CONFIG_8G:
                Acc_scaleValue =   SCALED_ACC_8G;
                AccConfig      =   AFS_SEL_SCALE_8G;
                break;
            case  ACC_CONFIG_16G:
                Acc_scaleValue =   SCALED_ACC_16G;
                AccConfig      =   AFS_SEL_SCALE_16G;
                break;
        }
        I2C_WriteByte(AccConfig, MPU6050_ADDRESS, MPU6050_ACCEL_CONFIG);

        //Congfig Gyro
        BYTE GyroConfig;
        switch(GYRO_SCALE_CONFIG)
        {
            case GYRO_CONFIG_250:
                Gyro_scaleValue =  SCALED_GYRO_250;
                GyroConfig      =   PS_SEL_SCALE_250;
                break;
            case GYRO_CONFIG_500:
                Gyro_scaleValue =  SCALED_GYRO_500;
                GyroConfig      =   PS_SEL_SCALE_500;
                break;
            case GYRO_CONFIG_1000:
                Gyro_scaleValue =  SCALED_GYRO_1000;
                GyroConfig      =   PS_SEL_SCALE_1000;
                break;
            case GYRO_CONFIG_2000:
                Gyro_scaleValue =  SCALED_GYRO_2000;
                GyroConfig      =   PS_SEL_SCALE_2000;
                break;
        }
        I2C_WriteByte(GyroConfig, MPU6050_ADDRESS, MPU6050_GYRO_CONFIG);

        //Enable Interrupt
        I2C_WriteByte(0x01, MPU6050_ADDRESS,MPU6050_INT_ENABLE);

        // Enable MPU6050;
        I2C_WriteByte(CLKSEL_0, MPU6050_ADDRESS, MPU6050_PWR_MGMT_1);

        /*  Config another Register*/
//          I2C_WriteByte(0x01, MPU6050_ADDRESS,MPU6050_INT_ENABLE);
//          I2C_WriteByte(0x00, MPU6050_ADDRESS,MPU6050_TEMP_OUT_H);
//          I2C_WriteByte(0x00, MPU6050_ADDRESS,MPU6050_TEMP_OUT_L);
//          I2C_WriteByte(0x00, MPU6050_ADDRESS,MPU6050_GYRO_XOUT_H);
//          I2C_WriteByte(0x00, MPU6050_ADDRESS,MPU6050_GYRO_XOUT_L);
//          I2C_WriteByte(0x00, MPU6050_ADDRESS,MPU6050_GYRO_YOUT_H);
//          I2C_WriteByte(0x00, MPU6050_ADDRESS,MPU6050_GYRO_YOUT_L);
//          I2C_WriteByte(0x00, MPU6050_ADDRESS,MPU6050_GYRO_ZOUT_H);
//          I2C_WriteByte(0x00, MPU6050_ADDRESS,MPU6050_GYRO_ZOUT_L);
//          I2C_WriteByte(0x00, MPU6050_ADDRESS,MPU6050_ACCEL_XOUT_H);
//          I2C_WriteByte(0x00, MPU6050_ADDRESS,MPU6050_ACCEL_XOUT_L);
//          I2C_WriteByte(0x00, MPU6050_ADDRESS,MPU6050_ACCEL_YOUT_H);
//          I2C_WriteByte(0x00, MPU6050_ADDRESS,MPU6050_ACCEL_YOUT_L);
//          I2C_WriteByte(0x00, MPU6050_ADDRESS,MPU6050_ACCEL_ZOUT_H);
//          I2C_WriteByte(0x00, MPU6050_ADDRESS,MPU6050_ACCEL_ZOUT_L);
//          I2C_WriteByte(0x00, MPU6050_ADDRESS,MPU6050_FF_THR);
//          I2C_WriteByte(0x00, MPU6050_ADDRESS,MPU6050_FF_DUR);
//          I2C_WriteByte(0x00, MPU6050_ADDRESS,MPU6050_MOT_THR);
//          I2C_WriteByte(0x00, MPU6050_ADDRESS,MPU6050_MOT_DUR);
//          I2C_WriteByte(0x00, MPU6050_ADDRESS,MPU6050_ZRMOT_THR);
//          I2C_WriteByte(0x00, MPU6050_ADDRESS,MPU6050_ZRMOT_DUR);
//          I2C_WriteByte(0x00, MPU6050_ADDRESS,MPU6050_TEMP_OUT_H);
//
//          I2C_WriteByte(0x00, MPU6050_ADDRESS,MPU6050_FIFO_EN);
//          I2C_WriteByte(0x00, MPU6050_ADDRESS,MPU6050_I2C_SLV0_ADDR);
//          I2C_WriteByte(0x00, MPU6050_ADDRESS,MPU6050_I2C_SLV0_REG);
//          I2C_WriteByte(0x00, MPU6050_ADDRESS,MPU6050_I2C_MST_CTRL);
//          I2C_WriteByte(0x00, MPU6050_ADDRESS,MPU6050_I2C_SLV0_ADDR);
//          I2C_WriteByte(0x00, MPU6050_ADDRESS,MPU6050_I2C_SLV0_REG);
//          I2C_WriteByte(0x00, MPU6050_ADDRESS,MPU6050_I2C_SLV0_CTRL);
//          I2C_WriteByte(0x00, MPU6050_ADDRESS,MPU6050_I2C_SLV1_ADDR);
//          I2C_WriteByte(0x00, MPU6050_ADDRESS,MPU6050_I2C_SLV1_REG);
//          I2C_WriteByte(0x00, MPU6050_ADDRESS,MPU6050_I2C_SLV1_CTRL);
//          I2C_WriteByte(0x00, MPU6050_ADDRESS,MPU6050_I2C_SLV2_ADDR);
//          I2C_WriteByte(0x00, MPU6050_ADDRESS,MPU6050_I2C_SLV2_REG);
//          I2C_WriteByte(0x00, MPU6050_ADDRESS,MPU6050_I2C_SLV2_CTRL);
//          I2C_WriteByte(0x00, MPU6050_ADDRESS,MPU6050_I2C_SLV3_ADDR);
//          I2C_WriteByte(0x00, MPU6050_ADDRESS,MPU6050_I2C_SLV3_REG);
//          I2C_WriteByte(0x00, MPU6050_ADDRESS,MPU6050_I2C_SLV3_CTRL);
//          I2C_WriteByte(0x00, MPU6050_ADDRESS,MPU6050_I2C_SLV4_ADDR);
//          I2C_WriteByte(0x00, MPU6050_ADDRESS,MPU6050_I2C_SLV4_REG);
//          I2C_WriteByte(0x00, MPU6050_ADDRESS,MPU6050_I2C_SLV4_DO);
//          I2C_WriteByte(0x00, MPU6050_ADDRESS,MPU6050_I2C_SLV4_CTRL);
//          I2C_WriteByte(0x00, MPU6050_ADDRESS,MPU6050_I2C_SLV4_DI);
//          I2C_WriteByte(0x00, MPU6050_ADDRESS,MPU6050_INT_PIN_CFG);

        __delay_cycles(100000);
}
/*--------------------------------------------------------------------------------
Function    : MPU6050_CheckI2C
Purpose     : Check I2C communication
Parameters  : None
Return      : Value of WHO_AM_I registor (0x68)
-------------------------------------------------------------------------------*/
BYTE MPU6050_CheckI2C(VOID)
{
        BYTE byBuff;
        I2C_ReadData(&byBuff, MPU6050_ADDRESS, MPU6050_WHO_AM_I,1);
        return (byBuff);
}

/*--------------------------------------------------------------------------------
Function    : MPU6050_TestRegConfig
Purpose     : Check the config of some register:
- MPU6050_RA_SMPLRT_DIV == 0x01:
-  MPU6050_RA_CONFIG == 0x03;
- MPU6050_RA_GYRO_CONFIG == 0x01;
- MPU6050_RA_ACCEL_CONFIG == 0x00;
Parameters  : None
Return      : 0 if pass and 1 if failt
-------------------------------------------------------------------------------*/

BYTE MPU6050_TestRegConfig(VOID)
{
        BYTE byBuff, ret;
        I2C_ReadData(&byBuff, MPU6050_ADDRESS, MPU6050_SMPLRT_DIV,1);
        ret =  (byBuff == 0x01 ? 0: 1);
        __delay_cycles(10000);

        I2C_ReadData(&byBuff, MPU6050_ADDRESS, MPU6050_CONFIG,1);
        ret =  (byBuff == 0x03 ? 0: 1);
        __delay_cycles(10000);


        I2C_ReadData(&byBuff, MPU6050_ADDRESS, MPU6050_GYRO_CONFIG,1);
        ret =  (byBuff == 0x01 ? 0: 1);
        __delay_cycles(10000);

        I2C_ReadData(&byBuff, MPU6050_ADDRESS, MPU6050_ACCEL_CONFIG,1);
        ret =  (byBuff == 0x00 ? 0: 1);
        __delay_cycles(10000);

        I2C_ReadData(&byBuff, MPU6050_ADDRESS, MPU6050_PWR_MGMT_1,1);
        ret =  (byBuff == 0x00 ? 0: 1);
        __delay_cycles(10000);

        return ret;
}

/*--------------------------------------------------------------------------------
Function    : MPU6050_GetAccValueRaw
Purpose     : Get raw value x, y, z of accel
Parameters  : PACC_DATA_RAW - pointer to a struct store acc raw data
Return      : NULL
--------------------------------------------------------------------------------*/
VOID MPU6050_GetAccValueRaw(PVOID pValue)
{
        BYTE pBuff[6];
        I2C_ReadData(pBuff, MPU6050_ADDRESS, MPU6050_ACCEL_XOUT_H, 6);

        ((PACC_DATA_RAW)pValue)->x = (int16)(CONVERT_TO_16BIT(pBuff[1], pBuff[0]) - Acc_OffsetValueX);
        ((PACC_DATA_RAW)pValue)->y = (int16)(CONVERT_TO_16BIT(pBuff[3], pBuff[2]) - Acc_OffsetValueY);
        ((PACC_DATA_RAW)pValue)->z = (int16)(CONVERT_TO_16BIT(pBuff[5], pBuff[4]) - Acc_OffsetValueZ);
}

/*--------------------------------------------------------------------------------
Function    : PMU6050_AccConvertData
Purpose     : Scaled data to radian value
Parameters  : ACC_DATA_RAW , PVOID scaledData
Return      : NULL
--------------------------------------------------------------------------------*/

VOID PMU6050_AccConvertData(ACC_DATA_RAW rawValue, PVOID scaledData)
{
        ((PACC_DATA_SCALED)scaledData)->x = (float)rawValue.x / Acc_scaleValue;
        ((PACC_DATA_SCALED)scaledData)->y = (float)rawValue.y / Acc_scaleValue;
        ((PACC_DATA_SCALED)scaledData)->z = (float)rawValue.z / Acc_scaleValue;

}
/*--------------------------------------------------------------------------------
Function    : PMU6050_GetAccValueAngle
Purpose     : Get the rotation angle of sensor (compare with x, y, z axis) in degre
Parameters  : PACC_DATA_ANGLE - pointer to a struct store angle rotation
Return      : NULL
--------------------------------------------------------------------------------*/

VOID PMU6050_GetRotationAngle(ACC_DATA_RAW raw, PVOID pRotationAngle)
{

        /* Caculate the angle rotation */
        /* 180/PI = 57.296 */
        /*Fix: use atan2 -> result in -pi -> pi */
        float x_angle = 57.296 * atan2((float)raw.y, sqrt((float)raw.z*(float)raw.z + (float)raw.x*(float)raw.x));
        if(x_angle < 0) x_angle += 360.0;

        float y_angle = 57.296 * atan2((float)raw.x, sqrt((float)raw.z*(float)raw.z + (float)raw.y*(float)raw.y));
        if(y_angle < 0) y_angle += 360.0;

        float z_angle = 57.296 * atan2((float)raw.z, sqrt(((float)raw.x*(float)raw.x + (float)raw.y*(float)raw.y)));
        if(x_angle < 0) x_angle += 360.0;

        ((PANGLE)pRotationAngle)->x = x_angle;
        ((PANGLE)pRotationAngle)->y = y_angle;
        ((PANGLE)pRotationAngle)->z = z_angle;
}

/*--------------------------------------------------------------------------------
Function    : MPU6050_GetGyroValueRaw
Purpose     : Get raw value x, y, z of Gyro
Parameters  : PGYRO_DATA_RAW - pointer to struct store Gyro data
Return      : NULL
--------------------------------------------------------------------------------*/
VOID MPU6050_GetGyroValueRaw(PVOID pValue)
{
        BYTE pBuff[6];
        I2C_ReadData(pBuff, MPU6050_ADDRESS, MPU6050_GYRO_XOUT_H, 6);

        ((PGYRO_DATA_RAW)pValue)->x = (int16)(CONVERT_TO_16BIT(pBuff[1], pBuff[0]) - Gyro_OffsetValueX);
        ((PGYRO_DATA_RAW)pValue)->y = (int16)(CONVERT_TO_16BIT(pBuff[3], pBuff[2]) - Gyro_OffsetValueY);
        ((PGYRO_DATA_RAW)pValue)->z = (int16)(CONVERT_TO_16BIT(pBuff[5], pBuff[4]) - Gyro_OffsetValueZ);

}

/*--------------------------------------------------------------------------------
Function    : PMU6050_GetGyroValueAngle
Purpose     : Get value x, y, z of Gyro
Parameters  : PGYRO_DATA_ANGLE - pointer to struct store Gyro data
Return      : NULL
--------------------------------------------------------------------------------*/
VOID PMU6050_GetGyroValueAngle(PVOID pValue)
{
        ;
}

/*--------------------------------------------------------------------------------
Function    : PMU6050_GyroConvertData
Purpose     : Convert data to m/s^2 by divide to scale
Parameters  : GYRO_DATA_RAW rawValue, PVOID scaledData
Return      : NULL
--------------------------------------------------------------------------------*/
VOID PMU6050_GyroConvertData(GYRO_DATA_RAW rawValue, PVOID scaledData)
{
        ((PGYRO_DATA_SCALED)scaledData)->x = (float)rawValue.x / Gyro_scaleValue;
        ((PGYRO_DATA_SCALED)scaledData)->y = (float)rawValue.y / Gyro_scaleValue;
        ((PGYRO_DATA_SCALED)scaledData)->z = (float)rawValue.z / Gyro_scaleValue;
}

/*--------------------------------------------------------------------------------
Function    :  MPU6050_Calibrate_Gyro
Purpose     :  Get the value to calibrate Gyro
Parameters  : NULL
Return      : NULL
--------------------------------------------------------------------------------*/

VOID MPU6050_Calibrate_Gyro(VOID)
{
        int i;
        int x = 0;
        int y = 0;
        int z = 0;
        BYTE pBuff[6];

        Gyro_OffsetValueX = 0;
        Gyro_OffsetValueY = 0;
        Gyro_OffsetValueZ = 0;
        for(i = 0; i < 2000; i++)
        {

                I2C_ReadData(pBuff, MPU6050_ADDRESS, MPU6050_GYRO_XOUT_H, 6);

                x = (int16)(CONVERT_TO_16BIT(pBuff[1], pBuff[0]));
                y = (int16)(CONVERT_TO_16BIT(pBuff[3], pBuff[2]));
                z = (int16)(CONVERT_TO_16BIT(pBuff[5], pBuff[4]));

                Gyro_OffsetValueX = (Gyro_OffsetValueX + x) >> 1;
                Gyro_OffsetValueY = (Gyro_OffsetValueY + y) >> 1;
                Gyro_OffsetValueZ = (Gyro_OffsetValueZ + z) >> 1;
        }
//        Gyro_OffsetValueX = (int16)(SumGyroX/200);
//        Gyro_OffsetValueY = (int16)(SumGyroY/200);
//        Gyro_OffsetValueZ = (int16)(SumGyroZ/200);
}




/*--------------------------------------------------------------------------------
Function    : ComplementaryFilter
Purpose     : Calculate angle use both acc and gyro
Parameters  : Raw data (ADC 16bit) of acc and gyro
Return      : ANGLE
--------------------------------------------------------------------------------*/

VOID Complementary_Filter(ACC_DATA_RAW accData, GYRO_DATA_RAW gyroData, PVOID pAngle)
{
        float pitchAcc, rollAcc;

        /* Angle around the X axis */
        ((PANGLE)pAngle)->x += ((float)gyroData.x / Gyro_scaleValue)*dt; // dt define as 0.015s
        /* Angle around the Y axis*/
        ((PANGLE)pAngle)->y += ((float)gyroData.y / Gyro_scaleValue)*dt;

        // Compensate for drift with accelerometer data if !bullshit
        // Sensitivity = -2 to 2 G at 16Bit -> 2G = 32768 && 0.5G = 8192
        long forceMagnitudeApprox = ABS(accData.x) + ABS(accData.y) + ABS(accData.z);
        if (forceMagnitudeApprox > 8192 && forceMagnitudeApprox < 32768)
        {
                // Turning around the X axis results in a vector on the Y-axis
                pitchAcc = 57.296 * atan2((float)accData.y, sqrt((float)accData.z*(float)accData.z + (float)accData.x*(float)accData.x));
                ((PANGLE)pAngle)->x = ((PANGLE)pAngle)->x * 0.98 + pitchAcc * 0.02;

                // Turning around the Y axis results in a vector on the X-axis
                rollAcc = 57.296 * atan2((float)accData.x, sqrt((float)accData.z*(float)accData.z + (float)accData.y*(float)accData.y));
                ((PANGLE)pAngle)->y = ((PANGLE)pAngle)->y* 0.98 + rollAcc * 0.02;
        }

}



