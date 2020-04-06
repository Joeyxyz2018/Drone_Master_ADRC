#include "task.h"
/**************�ļ�˵��**********************
Para_Init();
Motor_Iner_loop();
Motor_Outer_loop();
��task.c����ͷ�ļ�task.h
--------------��������װ�����---------------
F450���ᣬX�ͣ���ǰ�׺󣬴���ǰ��������Ϊ1��ʼ��ʱ����
********************************************/
short PwmOut[4]={0,0,0,0};  //��ֵ������ʱ�������PWM

/**********************
���Ʋ�����ʼ��
�������ϵ������һ��PWM���ں����
**********************/
void Para_Init(void)
{
	MOTOR1=PwmOut[0];MOTOR2=PwmOut[1];MOTOR3=PwmOut[2];MOTOR4=PwmOut[3];
	adrR.A=1;adrR.B=1;adrR.KpIn=0.7f;adrR.KdIn=0.01f;adrR.Kw=0;
	adrP.A=1;adrP.B=1;adrP.KpIn=0.2f;adrP.KdIn=0.00f;adrP.Kw=0;
}

/**********************
����ڻ��Կ��ſ���
**********************/
void Motor_Iner_loop(void)
{
	ADRC_LESO(&adrR,gx);
	ADRC_LESO(&adrP,gy);
	adrR.u=adrR.KpIn*(adrR.PosOut-gx)-adrR.KdIn*adrR.AccEst-adrR.Kw*adrR.w;
	adrP.u=adrP.KpIn*(adrP.PosOut-gy)-adrP.KdIn*adrP.AccEst-adrP.Kw*adrP.w;
	PwmOut[0]=RCdata[2]+DegToPwm(-adrR.u-adrP.u);
	PwmOut[1]=RCdata[2]+DegToPwm(-adrR.u+adrP.u);
	PwmOut[2]=RCdata[2]+DegToPwm(+adrR.u+adrP.u);
	PwmOut[3]=RCdata[2]+DegToPwm(+adrR.u-adrP.u);
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
		adrR.PosOut=PwmToDegAdd(RCdata[0]);
		adrP.PosOut=PwmToDegAdd(RCdata[1]);
	}
	else
	{
		Quaternion Qexp;
		float Hroll=PwmToRadAdd(RCdata[0])/2;
		float Hpitch=PwmToRadAdd(RCdata[1])/2;
		float Hyaw=PwmToRadAdd(RCdata[3])/2;
		Qexp.q0=Mcos(Hroll)*Mcos(Hpitch)*Mcos(Hyaw)+Msin(Hroll)*Msin(Hpitch)*Msin(Hyaw);
		Qexp.q1=Msin(Hroll)*Mcos(Hpitch)*Mcos(Hyaw)-Mcos(Hroll)*Msin(Hpitch)*Msin(Hyaw);
		Qexp.q2=Mcos(Hroll)*Msin(Hpitch)*Mcos(Hyaw)+Msin(Hroll)*Mcos(Hpitch)*Msin(Hyaw);
		Qexp.q3=Mcos(Hroll)*Mcos(Hpitch)*Msin(Hyaw)-Msin(Hroll)*Msin(Hpitch)*Mcos(Hyaw);
		Quaternion Qerr=Quaternion_Error(Qexp,Qpos);
		adrR.PosOut=adrR.KpOut*RadToDeg(Masin(Qerr.q1));
		adrP.PosOut=adrP.KpOut*RadToDeg(Masin(Qerr.q2));
	}
}
