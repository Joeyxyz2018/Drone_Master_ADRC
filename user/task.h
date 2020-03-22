#ifndef __TASK_H
#define __TASK_H

#include "protocol.h"
#include "imu.h"
#include "mpu6050.h"
#include "adrc.h"
#include "adc.h"

#define MOTOR1        (TIM1->CCR3) 
#define MOTOR2        (TIM1->CCR2)
#define MOTOR3        (TIM1->CCR4)
#define MOTOR4        (TIM1->CCR1)

#define LOWSPEED      100  //����
#define NORMALSPEED   400  //�����½��Ĵ����ٶ�,����ʧ�ر���
#define ERR_TIME      20   //û���յ���ȷң�����źŵĴ���
//LockMode
#define LOCKED        0    //����״̬���޲���
#define TOUNLOCK      1    //����״̬�ҳ��Խ���
#define UNLOCKED      2    //����״̬
#define LOCK_TIME     20   //����ʱ��,2��
//GlobalStat
//#define MOTOR_LOCK    0X01
#define FAIL_SAFE     0x80

extern short RCdata[];  //��control.c����
extern AxisInt gyro;  //��control.c����
extern ADRC_Param adrcRoll,adrcPitch;  //��control.c����
extern Quaternion Qpos;  //��control.c����
extern u8 GlobalStat;  //��control.c����

//��task.c��
void IMU_Processing(void);
void RC_Processing(void);
void RC_Monitor(void);
void Send_Data(void);
//��control.c��
void Para_Init(void);
void Motor_Iner_loop(void);
void Motor_Outer_loop(void);

#endif
