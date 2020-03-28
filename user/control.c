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
	adrcRoll.KpOut=0;adrcRoll.KpIn=0.5;adrcRoll.KdIn=0;
	adrcPitch.KpOut=0;adrcPitch.KpIn=0.5;adrcPitch.KdIn=0;
}

/**********************
����ڻ��Կ��ſ���
**********************/
void Motor_Iner_loop(void)
{
	if(!(GlobalStat & MOTOR_LOCK))
	{
		MOTOR1=0;
		MOTOR2=0;
		MOTOR3=0;
		MOTOR4=0;
		return;
	}
	if(RCdata[2]<=LOWSPEED)
	{
		MOTOR1=LOWSPEED;
		MOTOR2=LOWSPEED;
		MOTOR3=LOWSPEED;
		MOTOR4=LOWSPEED;
		return;
	}
	float gx=GyroToDeg(gyro.x);
	float gy=GyroToDeg(gyro.y);
	ADRC_LESO(adrcRoll.u,gx,1,&adrcRoll.gEst,&adrcRoll.AccEst,&adrcRoll.w);
	ADRC_LESO(adrcPitch.u,gy,1,&adrcPitch.gEst,&adrcPitch.AccEst,&adrcPitch.w);
	adrcRoll.u=adrcRoll.PosOut-adrcRoll.KpIn*gx-adrcRoll.KdIn*adrcRoll.AccEst;
	adrcPitch.u=adrcPitch.PosOut-adrcPitch.KpIn*gy-adrcPitch.KdIn*adrcPitch.AccEst;
	PwmOut[0]=RCdata[2]+DegToPwm(-adrcRoll.u-adrcPitch.u);
	PwmOut[1]=RCdata[2]+DegToPwm(-adrcRoll.u+adrcPitch.u);
	PwmOut[2]=RCdata[2]+DegToPwm(+adrcRoll.u+adrcPitch.u);
	PwmOut[3]=RCdata[2]+DegToPwm(+adrcRoll.u-adrcPitch.u);
	MOTOR1=LIMIT(PwmOut[0],LOWSPEED,1000);
	MOTOR2=LIMIT(PwmOut[1],LOWSPEED,1000);
	MOTOR3=LIMIT(PwmOut[2],LOWSPEED,1000);
	MOTOR4=LIMIT(PwmOut[3],LOWSPEED,1000);
}

#define SPEED  //���ٶ�ģʽ
/**********************
����⻷��������
**********************/
void Motor_Outer_loop(void)
{
#ifdef SPEED
	adrcRoll.PosOut=PwmToDegAdd(RCdata[0]);
	adrcPitch.PosOut=PwmToDegAdd(RCdata[1]);
#else
	Quaternion Qexp;
	float Hroll=PwmToRadAdd(RCdata[0])/2;
	float Hpitch=PwmToRadAdd(RCdata[1])/2;
	float Hyaw=PwmToRadAdd(RCdata[3])/2;
	Qexp.q0=Mcos(Hroll)*Mcos(Hpitch)*Mcos(Hyaw)+Msin(Hroll)*Msin(Hpitch)*Msin(Hyaw);
	Qexp.q1=Msin(Hroll)*Mcos(Hpitch)*Mcos(Hyaw)-Mcos(Hroll)*Msin(Hpitch)*Msin(Hyaw);
	Qexp.q2=Mcos(Hroll)*Msin(Hpitch)*Mcos(Hyaw)+Msin(Hroll)*Mcos(Hpitch)*Msin(Hyaw);
	Qexp.q3=Mcos(Hroll)*Mcos(Hpitch)*Msin(Hyaw)-Msin(Hroll)*Msin(Hpitch)*Mcos(Hyaw);
	Quaternion Qerr=Quaternion_Error(Qexp,Qpos);
	adrcRoll.PosOut=adrcRoll.KpOut*RadToDeg(Masin(Qerr.q1));
	adrcPitch.PosOut=adrcPitch.KpOut*RadToDeg(Masin(Qerr.q2));
#endif
}
