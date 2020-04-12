#include "task.h"
/**************�ļ�˵��**********************
Para_Init();
Motor_Iner_loop();
Motor_Outer_loop();
��task.c����ͷ�ļ�task.h
--------------��������װ�����---------------
X������,����󷽵�����Ϊ1��ʼ˳ʱ����,1���������Ϊ��ʱ��
********************************************/
short PwmOut[4]={0,0,0,0};  //��ֵ������ʱ�������PWM

/**********************
���Ʋ�����ʼ��
**********************/
void Para_Init(void)
{
	MOTOR1=PwmOut[0];MOTOR2=PwmOut[1];MOTOR3=PwmOut[2];MOTOR4=PwmOut[3];
	adrR.KpOut=2.0f;adrR.KpIn=0.7f;adrR.KdIn=0.02f;adrR.Kw=0;
	adrP.KpOut=2.0f;adrP.KpIn=0.7f;adrP.KdIn=0.02f;adrP.Kw=0;
	Kyaw=1;
	RolBias=20;PitBias=0;YawBias=0;
}

/**********************
����ڻ��Կ��ſ���
**********************/
void Motor_Iner_loop(void)
{
	ADRC_LESO(&adrR,gyrox);
	ADRC_LESO(&adrP,gyroy);
	adrR.u=adrR.KpIn*(adrR.PosOut-gyrox)-adrR.KdIn*adrR.AccEst-adrR.Kw*adrR.w;
	adrP.u=adrP.KpIn*(adrP.PosOut-gyroy)-adrP.KdIn*adrP.AccEst-adrP.Kw*adrP.w;
	PwmOut[0]=RCdata[2]+DegToPwm(-adrR.u-adrP.u-YawOut);
	PwmOut[1]=RCdata[2]+DegToPwm(-adrR.u+adrP.u+YawOut);
	PwmOut[2]=RCdata[2]+DegToPwm(+adrR.u+adrP.u-YawOut);
	PwmOut[3]=RCdata[2]+DegToPwm(+adrR.u-adrP.u+YawOut);
	if(!(GlobalStat & MOTOR_LOCK))
	{
		MOTOR1=0;
		MOTOR2=0;
		MOTOR3=0;
		MOTOR4=0;
	}
	else if(RCdata[2]<=LOWSPEED)
	{
		MOTOR1=LOWSPEED;
		MOTOR2=LOWSPEED;
		MOTOR3=LOWSPEED;
		MOTOR4=LOWSPEED;
	}
	else
	{
		MOTOR1=LIMIT(PwmOut[0],LOWSPEED,1000);
		MOTOR2=LIMIT(PwmOut[1],LOWSPEED,1000);
		MOTOR3=LIMIT(PwmOut[2],LOWSPEED,1000);
		MOTOR4=LIMIT(PwmOut[3],LOWSPEED,1000);
	}
}

/**********************
����⻷��������
**********************/
void Motor_Outer_loop(void)
{
	if(GlobalStat & SPEED_MODE)
	{
		adrR.PosOut=PwmToDegAdd(RCdata[0])/adrR.KpIn;
		adrP.PosOut=PwmToDegAdd(RCdata[1])/adrP.KpIn;
	}
	else
	{
		float RolExp=PwmToDegAdd(RCdata[0]);
		float PitExp=PwmToDegAdd(RCdata[1]);
		float roll=Matan2(2*(Qpos.q0*Qpos.q1+Qpos.q2*Qpos.q3),1-2*(Qpos.q1*Qpos.q1+Qpos.q2*Qpos.q2))*57.3f;
		float pitch=Masin(2*(Qpos.q0*Qpos.q2-Qpos.q1*Qpos.q3))*57.3f;
		adrR.PosOut=adrR.KpOut*(RolExp-roll);
		adrP.PosOut=adrP.KpOut*(PitExp-pitch);
	}
	adrR.PosOut+=RolBias;
	adrP.PosOut+=PitBias;
	YawOut=Kyaw*PwmToDegAdd(RCdata[3])+YawBias;
}
