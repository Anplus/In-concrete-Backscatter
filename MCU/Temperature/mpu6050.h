#ifndef __MPU6050_H
#define __MPU6050_H
#include <msp430.h>

//以下引脚可随意更改
#define MPU6050_SCL_H() P2OUT|=BIT4
#define MPU6050_SCL_L() P2OUT&=~BIT4

#define MPU6050_SDA_H() P2OUT|=BIT5
#define MPU6050_SDA_L() P2OUT&=~BIT5

#define MPU_IIC_OUT P2DIR |= BIT4+BIT5
#define MPU_IIC_IN P2DIR = 0x10     //0001 0000 此处引脚要按位改, 位7~位0, SCL改成哪位哪位改成1

#define SDA_DATA P2IN & BIT5

#define MPU6050_Address 0xD0        //mpu6050模块地址

#define SMPLRT_DIV 0x19 //陀螺仪采样率，典型值：0x07(125Hz)
#define CONFIG 0x1A //低通滤波频率，典型值：0x06(5Hz)
#define GYRO_CONFIG 0x1B //陀螺仪自检及测量范围，典型值：0x18(不自检，2000deg/s)
#define ACCEL_CONFIG 0x1C //加速计自检、测量范围及高通滤波频率，典型值：0x01(不自检，2G，5Hz)
#define PWR_MGMT_1 0x6B  //电源管理，典型值：0x00(正常启用)

/* 加速度相关寄存器地址 */
#define ACCEL_XOUT_H 0x3B
#define ACCEL_XOUT_L 0x3C
#define ACCEL_YOUT_H 0x3D
#define ACCEL_YOUT_L 0x3E
#define ACCEL_ZOUT_H 0x3F
#define ACCEL_ZOUT_L 0x40

/* 温度相关寄存器地址 */
#define TEMP_OUT_H 0x41
#define TEMP_OUT_L 0x42

/* 陀螺仪相关寄存器地址 */
#define GYRO_XOUT_H 0x43
#define GYRO_XOUT_L 0x44
#define GYRO_YOUT_H 0x45
#define GYRO_YOUT_L 0x46
#define GYRO_ZOUT_H 0x47
#define GYRO_ZOUT_L 0x48

void MPU6050_Init(void);
int MPU6050_GetData(unsigned char Reg_Address);
void MPU6050_GetAngle(float *Angle);
unsigned char MPU6050_ReadData(unsigned char Reg_Address);
void MPU6050_GetAcc(double *Acc);

#endif

