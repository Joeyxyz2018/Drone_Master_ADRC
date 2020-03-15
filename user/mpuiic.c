#include "mpuiic.h"

void IIC_Delay(void)
{
	__nop();__nop();__nop();
	__nop();__nop();__nop();
	__nop();__nop();__nop();
}

//����IIC��ʼ�ź�
void IIC_Start(void)
{
	SDA_OUT;
	SDA_H;
	SCL_H;
	IIC_Delay();
 	SDA_L;
	IIC_Delay();
	SCL_L;
}
//����IICֹͣ�ź�
void IIC_Stop(void)
{
	SCL_L;
	SDA_OUT;
	SDA_L;
 	IIC_Delay();
	SCL_H;
	IIC_Delay();
	SDA_H;
}
//�ȴ�Ӧ���źŵ���
//����ֵ:0,�ɹ�
u8 IIC_Wait_Ack(void)
{
	SDA_IN;
	SCL_L;
	IIC_Delay();
	SCL_H;
	IIC_Delay();
	if(SDA_Read)
	{
		SCL_L;
		return 1;
	}
	SCL_L;
	return 0;
}
//����ACKӦ��
void IIC_Ack(void)
{
	SCL_L;
	SDA_OUT;
	SDA_L;
	IIC_Delay();
	SCL_H;
	IIC_Delay();
	SCL_L;
}
//������ACKӦ��		    
void IIC_NAck(void)
{
	SCL_L;
	SDA_OUT;
	SDA_H;
	IIC_Delay();
	SCL_H;
	IIC_Delay();
	SCL_L;
}
//IIC����һ���ֽ�
void IIC_Write_Byte(u8 txd)
{
	u8 t=8;
	SCL_L;
	SDA_OUT;
	while(t--)
	{
		if(txd&0x80)
			SDA_H;
		else
			SDA_L;
		txd<<=1;
		SCL_H;
		IIC_Delay();
		SCL_L;
		IIC_Delay();
	}
}
//��һ���ֽ�
//ack=1ʱ����ACK;ack=0ʱ����nACK
//����ֵ:��ȡ��һ���ֽ�
u8 IIC_Read_Byte(unsigned char ack)
{
	u8 t=8,receive=0;
	SDA_IN;
	SCL_L;
	while(t--)
	{
		IIC_Delay();
		SCL_H;
		receive<<=1;
		if(SDA_Read) receive++;
		IIC_Delay();
		SCL_L;
	}
	if (!ack)
		IIC_NAck();
	else
		IIC_Ack();
	return receive;
}
//IICдһ���ֽ�
//reg:�Ĵ�����ַ
//data:����
//����ֵ:0,�ɹ�
u8 IIC_Write_Reg(u8 reg, u8 data)
{
	IIC_Start();
	IIC_Write_Byte((MPU_ADDR << 1) | 0);
	if (IIC_Wait_Ack())
	{
		IIC_Stop();
		return 1;
	}
	IIC_Write_Byte(reg);
	if (IIC_Wait_Ack())
	{
		IIC_Stop();
		return 2;
	}
	IIC_Write_Byte(data);
	if (IIC_Wait_Ack())
	{
		IIC_Stop();
		return 3;
	}
	IIC_Stop();
	return 0;
}
//IIC��һ���ֽ� 
//reg:�Ĵ�����ַ 
//����ֵ:0,�ɹ�
u8 IIC_Read_Reg(u8 reg, u8 *data)
{
	IIC_Start();
	IIC_Write_Byte((MPU_ADDR << 1) | 0);
	if (IIC_Wait_Ack())
	{
		IIC_Stop();
		return 1;
	}
	IIC_Write_Byte(reg);
	if (IIC_Wait_Ack())
	{
		IIC_Stop();
		return 2;
	}
	IIC_Start();
	IIC_Write_Byte((MPU_ADDR << 1) | 1);
	if (IIC_Wait_Ack())
	{
		IIC_Stop();
		return 3;
	}
	*data = IIC_Read_Byte(0);
	IIC_Stop();
	return 0;
}
//IIC������
//reg:Ҫ��ȡ�ļĴ�����ַ
//len:Ҫ��ȡ�ĳ���
//buf:��ȡ�������ݴ洢��
//����ֵ:0,�ɹ�
u8 IIC_Read_Len(u8 reg, u8 len, u8 *buf)
{
	IIC_Start();
	IIC_Write_Byte((MPU_ADDR << 1) | 0);
	if (IIC_Wait_Ack())
	{
		IIC_Stop();
		return 1;
	}
	IIC_Write_Byte(reg);
	if (IIC_Wait_Ack())
	{
		IIC_Stop();
		return 2;
	}
	IIC_Start();
	IIC_Write_Byte((MPU_ADDR << 1) | 1);
	if (IIC_Wait_Ack())
	{
		IIC_Stop();
		return 3;
	}
	while (len)
	{
		if (len == 1)*buf = IIC_Read_Byte(0);
		else *buf = IIC_Read_Byte(1);
		len--;
		buf++;
	}
	IIC_Stop();
	return 0;
}
