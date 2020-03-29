#ifndef __NIMING_H
#define __NIMING_H

#include "usart.h"

#define SENDBUF_SIZE   128
//FcnWord
#define P_STAT         0x01  //״̬
#define P_ATTI         0x02  //��̬
#define P_SENSOR       0x04  //������
#define P_CTRL         0x08  //ң��
#define P_MOTOR        0x10  //����
#define P_REQ_CTRL     0xA0  //������
#define P_ROL_CTRL     0xA1  //ROL����
#define P_ROL_STAT     0xA2  //ROL״̬
#define P_PIT_CTRL     0xA3  //PIT����
#define P_PIT_STAT     0xA4  //PIT״̬
//GlobalStat
#define MOTOR_LOCK     0x01
#define FAIL_SAFE      0x10
#define RC_RECEIVE     0x80
//ReqMsg
#define REQ_ROL_CTRL   0x01
#define REQ_ROL_STAT   0x02
#define REQ_PIT_CTRL   0x04
#define REQ_PIT_STAT   0x08

extern u8 FcnWord;  //��task.c����
extern u8 LenWord;  //��task.c����
extern u8 RxTemp[12];  //��task.c����
extern u8 GlobalStat;  //��control.c,task.c����

void Protocol_Init(void);
void XDAA_Send_S16_Data(s16 *data,u8 len,u8 fcn);
void XDAA_Send_U8_Data(u8 *data,u8 len,u8 fcn);
void Total_Send(void);

#endif
