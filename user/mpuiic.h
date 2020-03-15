#ifndef __MPUIIC_H
#define __MPUIIC_H

#include "main.h"

#define	SCL_H   	SCL_PORT->BSRR = SCL_PIN    //SCL�ߵ�ƽ
#define	SCL_L   	SCL_PORT->BRR  = SCL_PIN    //SCL�͵�ƽ
#define	SDA_H   	SDA_PORT->BSRR = SDA_PIN    //SDA�ߵ�ƽ
#define	SDA_L   	SDA_PORT->BRR  = SDA_PIN    //SDA�͵�ƽ
#define	SDA_Read	SDA_PORT->IDR  & SDA_PIN    //SDA������

#define SCL_PORT     GPIOB
#define SDA_PORT     GPIOB
#define SCL_PIN      GPIO_PIN_8
#define SDA_PIN      GPIO_PIN_7
//IO��������
#define SDA_IN       GPIOB->MODER&=0xFFFF3FFF
#define SDA_OUT      GPIOB->MODER&=0xFFFF3FFF;GPIOB->MODER|=1<<14
#define MPU_ADDR 0x68
u8 IIC_Write_Reg(u8 reg,u8 data);  //IICдһ���ֽ�
u8 IIC_Read_Reg(u8 reg, u8 *data);  //IIC��һ���ֽ�
u8 IIC_Read_Len(u8 reg, u8 len, u8 *buf);
#endif
