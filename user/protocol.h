#ifndef __NIMING_H
#define __NIMING_H

#include "usart.h"

#define SENDBUF_SIZE   128
//FcnWord
#define P_STAT         0x01  //状态
#define P_ATTI         0x02  //姿态
#define P_QUATERNION   0x20  //四元数
#define P_SENSOR       0x04  //传感器
#define P_CTRL         0x08  //遥控
#define P_MOTOR        0x10  //油门
#define P_REQ_CTRL     0xA0  //读参数
#define P_ROL_CTRL     0xA1  //ROL参数
#define P_ROL_STAT     0xA2  //ROL状态
#define P_PIT_CTRL     0xA3  //PIT参数
#define P_PIT_STAT     0xA4  //PIT状态
//GlobalStat
#define MOTOR_LOCK     0x01  //已解锁
#define ACC_CALI       0x02  //准备校准加速度计
#define GYRO_CALI      0x04  //准备校准陀螺仪
#define SPEED_MODE     0x20  //速度模式
#define TX_BUSY        0x40  //串口数据正通过DMA发送
#define FAIL_SAFE      0x80  //飞行故障
#define REQ_MODE_ATTI  0x40
#define REQ_MODE_SPEED 0x80
//ReqMsg1
#define REQ_STAT       0x01
#define REQ_ATTI       0x02
#define REQ_SENSOR     0x04
#define REQ_RC         0x08
#define REQ_MOTOR      0x10
#define REQ_QUATERNION 0x20
//ReqMsg2
#define REQ_ROL_CTRL   0x01
#define REQ_ROL_STAT   0x02
#define REQ_PIT_CTRL   0x04
#define REQ_PIT_STAT   0x08
#define REQ_ACC_CALI   0x40
#define REQ_GYRO_CALI  0x80

//数据拆分宏定义，在发送大于1字节的数据类型时，比如int16、float等，需要把数据拆分成单独字节进行发送
#define BYTE0(dwTemp)       ( *( (char *)(&dwTemp)    ) )
#define BYTE1(dwTemp)       ( *( (char *)(&dwTemp) + 1) )
#define BYTE2(dwTemp)       ( *( (char *)(&dwTemp) + 2) )
#define BYTE3(dwTemp)       ( *( (char *)(&dwTemp) + 3) )

extern u8 FcnWord;  //被task.c调用
extern u8 LenWord;  //被task.c调用
extern u8 RxTemp[12];  //被task.c调用
extern u8 GlobalStat;  //被control.c,task.c调用
extern u8 RcvCnt;

void Protocol_Init(void);
void XDAA_Send_S16_Data(s16 *data,u8 len,u8 fcn);
void XDAA_Send_U8_Data(u8 *data,u8 len,u8 fcn);
void Total_Send(void);

#endif
