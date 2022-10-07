#include <msp430.h>
#include "MPU6050.h"
#include "math.h"
#include "oled.h"
#include "type.h"

/* ��������������ֵ������оƬ�̶�������Ӳ�����е����� */
#define X_ACCEL_OFFSET -540
#define Y_ACCEL_OFFSET -120
#define Z_ACCEL_OFFSET 120



//IIC��ʼ
void MPU6050_IIC_Start(void)
{
    MPU_IIC_OUT;
    MPU6050_SDA_H();
    MPU6050_SCL_H();
    MPU6050_SDA_L();
    MPU6050_SCL_L();
}

//IIC����
void MPU6050_IIC_Stop(void)
{
    MPU_IIC_OUT;
    MPU6050_SDA_L();
    MPU6050_SCL_H();
    MPU6050_SDA_H();
}

//IIC����һ���ֽ�����
void MPU6050_IIC_SendByte(unsigned char Byte)
{
    unsigned char i;
    MPU_IIC_OUT;
    for(i=0;i<8;i++)
    {
        if(Byte & (0x80 >> i))
        {
            MPU6050_SDA_H();
        }
        else
        {
            MPU6050_SDA_L();
        }
        MPU6050_SCL_H();
        MPU6050_SCL_L();
    }
}

//IIC����һ���ֽ�����
unsigned char MPU6050_IIC_ReceiveByte(void)
{
    unsigned char Byte = 0x00, i;
    MPU_IIC_OUT;
    MPU6050_SDA_H();
    MPU_IIC_IN;
    for(i=0;i<8;i++)
    {
        MPU6050_SCL_H();
        if(SDA_DATA)
        {
            Byte |= (0x80>>i);
        }
        MPU6050_SCL_L();
    }
    return Byte;
}

//IIC����Ӧ��
void MPU6050_IIC_SendAck(unsigned char AckBit)
{
    MPU_IIC_OUT;
    if(AckBit)
    {
        MPU6050_SDA_H();
    }
    else
    {
        MPU6050_SDA_L();
    }
    MPU6050_SCL_H();
    MPU6050_SCL_L();
}

//IIC����Ӧ��
unsigned char MPU6050_IIC_ReceiveAck(void)
{
    unsigned char AckBit;
    MPU_IIC_IN;
    MPU6050_SCL_H();
    if(SDA_DATA)
    {
        AckBit = 1;
    }
    else
    {
        AckBit = 0;
    }
    MPU6050_SCL_L();
    return AckBit;
}
//д�Ĵ���
void MPU6050_WriteData(unsigned char Reg_Address, unsigned char Data)
{
    MPU6050_IIC_Start();
    MPU6050_IIC_SendByte(MPU6050_Address);
    MPU6050_IIC_ReceiveAck();
    MPU6050_IIC_SendByte(Reg_Address);
    MPU6050_IIC_ReceiveAck();
    MPU6050_IIC_SendByte(Data);
    MPU6050_IIC_ReceiveAck();
    MPU6050_IIC_Stop();
}
//���Ĵ���
unsigned char MPU6050_ReadData(unsigned char Reg_Address)
{
    unsigned char Data;
    MPU6050_IIC_Start();
    MPU6050_IIC_SendByte(MPU6050_Address);
    MPU6050_IIC_ReceiveAck();
    MPU6050_IIC_SendByte(Reg_Address);
    MPU6050_IIC_ReceiveAck();
    MPU6050_IIC_Stop();
    MPU6050_IIC_Start();
    MPU6050_IIC_SendByte(MPU6050_Address+1);        //��ģʽһ��Ϊ�豸��ַ��1
    MPU6050_IIC_ReceiveAck();
    Data = MPU6050_IIC_ReceiveByte();
    MPU6050_IIC_SendAck(1);
    MPU6050_IIC_Stop();
    return Data;
}
//mpu6050��ʼ��
void MPU6050_Init(void)
{
    MPU6050_WriteData(PWR_MGMT_1, 0x00);        //����
    MPU6050_WriteData(SMPLRT_DIV, 0x07);    //�����ǲ����ʣ�����ֵ��0x07(125Hz)
    MPU6050_WriteData(CONFIG, 0x06);        //��ͨ�˲�Ƶ�ʣ�����ֵ��0x06(5Hz)
    MPU6050_WriteData(GYRO_CONFIG, 0x18);   //�������Լ켰������Χ������ֵ��0x18(���Լ죬2000deg/s)
    MPU6050_WriteData(ACCEL_CONFIG, 0x01);  //���ټ��Լ졢������Χ����ͨ�˲�Ƶ�ʣ�����ֵ��0x01(���Լ죬2G��5Hz)
}
//����ԭʼ����
int MPU6050_GetData(unsigned char Reg_Address)
{
    unsigned char DH, DL;
    int Data;
    DH = MPU6050_ReadData(Reg_Address);
    DL = MPU6050_ReadData(Reg_Address+1);
    Data = (DH << 8) + DL;
//    OLED_ShowNum(0,0,DH,8,16);
//    OLED_ShowNum(0,2,DL,8,16);
    return Data;
}
//��ȡ���
void MPU6050_GetAngle(float *Angle)
{
    Angle[0] = (acos((MPU6050_GetData(ACCEL_XOUT_H) + X_ACCEL_OFFSET) / 16384.0))*57.29577;
    Angle[1] = (acos((MPU6050_GetData(ACCEL_YOUT_H) + Y_ACCEL_OFFSET) / 16384.0))*57.29577;
    Angle[2] = (acos((MPU6050_GetData(ACCEL_ZOUT_H) + Z_ACCEL_OFFSET) / 16384.0))*57.29577;
}
//��ȡ���ٶ�
void MPU6050_GetAcc(double *Acc)
{
    Acc[0] = (MPU6050_GetData(ACCEL_XOUT_H) ) / 16384.0;
    Acc[1] = (MPU6050_GetData(ACCEL_YOUT_H) ) / 16384.0;
    Acc[2] = (MPU6050_GetData(ACCEL_ZOUT_H) ) / 16384.0;
}



























