#include "task.h"
/**************�ļ�˵��**********************
���˷��������ƺ���֮��Ķ�ʱ�������ֱ�Ϊ��
Lock_And_Unlock();             ����������
RC_Prepare();                  �Խ��ջ����źŽ���Ԥ����
IMU_Processing();              ��̬������£�MPU6050����У׼
********************************************/

Quaternion Qpos={1,0,0,0};  //��̬��Ԫ����������Ԫ��(���ļ�ȫ�ֱ���)
AxisInt acc;  //������ٶ�
AxisInt gyro;  //������ٶ�(���ļ�ȫ�ֱ���)
AxisInt oacc;  //������ٶȼ�ԭʼ����
AxisInt ogyro;  //����������ԭʼ����
short RCdata[4];  //ң������������(���ļ�ȫ�ֱ���)
ADRC_Param adrR,adrP;  //�Կ��ſ���������(���ļ�ȫ�ֱ���)
u8 ReqMsg1=0;  //��λ��ָ��1
u8 ReqMsg2=0;  //��λ��ָ��2
u8 ErrCnt=0;  //δ�յ�ң�����źŵĴ���(���ļ�ȫ�ֱ���)
float gx,gy;

/***********************
��̬�������,MPU6050����У׼
*@period:2ms
**********************/
void IMU_Processing(void)
{
	static float IIRax[3],IIRay[3],IIRaz[3];
	static float IIRgx[3],IIRgy[3],IIRgz[3];
	MPU_Get_Accelerometer(&acc.x,&acc.y,&acc.z);
	MPU_Get_Gyroscope(&gyro.x,&gyro.y,&gyro.z);
	oacc=acc;ogyro=gyro;
	acc.x=IIR_LowPassFilter(oacc.x,IIRax);
	acc.y=IIR_LowPassFilter(oacc.y,IIRay);
	acc.z=IIR_LowPassFilter(oacc.z,IIRaz);
	gyro.x=IIR_LowPassFilter(ogyro.x,IIRgx);
	gyro.y=IIR_LowPassFilter(ogyro.y,IIRgy);
	gyro.z=IIR_LowPassFilter(ogyro.z,IIRgz);
	Acc_Correct(&acc);
	Gyro_Correct(&gyro);
	gx=GyroToDeg(gyro.x);
	gy=GyroToDeg(gyro.y);
	IMUupdate(acc,gyro,&Qpos);
	if(GlobalStat & ACC_CALI)
		if(!Acc_Calibrate(acc))
			GlobalStat &=~ ACC_CALI;
	if(GlobalStat &GYRO_CALI)
		if(!Gyro_Calibrate(gyro))
			GlobalStat &=~ GYRO_CALI;
}

/***********************
ʧ�ر���.��������:����1��������,����2��3������һ
1.����״̬
2.�෭�ӽ��򳬹�90��
3.����δ�յ�ң�����ź�
**********************/
void Fail_Safe(void)
{
	if(RCdata[2]<NORMALSPEED)
		return;
	RCdata[0]=500;
	RCdata[1]=500;
	RCdata[2]=NORMALSPEED;
	RCdata[3]=500;
}

/***********************
���յ���ң�����źŽ��д���.�յ�һ֡����ִ��һ��
*@period:100ms(Not strict)
**********************/
void RC_Processing(void)
{
	switch(FcnWord)
	{
	case P_STAT:
		if(RxTemp[1]!=WHO_AM_I)
			break;
		if(RxTemp[0] & MOTOR_LOCK)
		{
			if(RCdata[2]<=LOWSPEED)
				GlobalStat|=MOTOR_LOCK;
		}
		else
			GlobalStat&=~MOTOR_LOCK;
		break;
	case P_CTRL:
		RCdata[0]=(RxTemp[0]<<8) | RxTemp[1];
		RCdata[1]=(RxTemp[2]<<8) | RxTemp[3];
		RCdata[2]=(RxTemp[4]<<8) | RxTemp[5];
		RCdata[3]=(RxTemp[6]<<8) | RxTemp[7];
		break;
	case P_REQ_CTRL:
		ReqMsg1=RxTemp[0];
		ReqMsg2=RxTemp[1];
		if((ReqMsg1 & (REQ_MODE_SPEED+REQ_MODE_ATTI))==REQ_MODE_SPEED)
			GlobalStat|=SPEED_MODE;
		else if((ReqMsg1 & (REQ_MODE_SPEED+REQ_MODE_ATTI))==REQ_MODE_ATTI)
			GlobalStat&=~SPEED_MODE;
		if(ReqMsg2 & REQ_ACC_CALI)
			GlobalStat|=ACC_CALI;
		if(ReqMsg2 & REQ_GYRO_CALI)
			GlobalStat|=GYRO_CALI;
		break;
	case P_ROL_CTRL:
		adrR.KpIn=(RxTemp[0]*256.0f+RxTemp[1])/1000.0f;
		adrR.KdIn=(RxTemp[2]*256.0f+RxTemp[3])/1000.0f;
		adrR.A=(RxTemp[4]*256.0f+RxTemp[5])/1000.0f;
		adrR.B=(RxTemp[6]*256.0f+RxTemp[7])/1000.0f;
		ReqMsg2|=REQ_ROL_CTRL;
		break;
	case P_PIT_CTRL:
		adrP.KpIn=(RxTemp[0]*256.0f+RxTemp[1])/1000.0f;
		adrP.KdIn=(RxTemp[2]*256.0f+RxTemp[3])/1000.0f;
		adrP.A=(RxTemp[4]*256.0f+RxTemp[5])/1000.0f;
		adrP.B=(RxTemp[6]*256.0f+RxTemp[7])/1000.0f;
		ReqMsg2|=REQ_PIT_CTRL;
	default:break;
	}
	if((Qpos.q1>0.7)||(Qpos.q2>0.7))
	Fail_Safe();
}

/***********************
��ʱ�Ƿ��յ�ң�����ź�
*@period:100ms
**********************/
void RC_Monitor(void)
{
	ErrCnt++;
	if(ErrCnt>=ERR_TIME)
	{
		Fail_Safe();
		ErrCnt--;
	}
	if(STAT_PORT & STAT_Pin)
		LED3_PORT |= LED3_Pin;
	else
		LED3_PORT &=~ LED3_Pin;
}

void RC_Data_Send(void)
{
	ErrCnt=0;
	s16 sdata[6];
	//��λ������1
	if(ReqMsg1 & REQ_STAT)
	{
		u8 udata[2]={GlobalStat,(u8)Get_Battery_Voltage()};
		XDAA_Send_U8_Data(udata,2,1);
		ReqMsg1 &=~ REQ_STAT;
	}
	if(ReqMsg1 & REQ_ATTI)
	{
		float roll=Matan2(2*(Qpos.q0*Qpos.q1+Qpos.q2*Qpos.q3),1-2*(Qpos.q1*Qpos.q1+Qpos.q2*Qpos.q2))*57.3f;
		float pitch=Masin(2*(Qpos.q0*Qpos.q2-Qpos.q1*Qpos.q3))*57.3f;
		float yaw=Matan2(2*(Qpos.q1*Qpos.q2+Qpos.q0*Qpos.q3),1-2*(Qpos.q2*Qpos.q2+Qpos.q3*Qpos.q3))*57.3f;
		sdata[0]=(s16)(roll*100);
		sdata[1]=(s16)(pitch*100);
		sdata[2]=(s16)(yaw*100);
		XDAA_Send_S16_Data(sdata,3,P_ATTI);
		ReqMsg1 &=~ REQ_ATTI;
}
	if(ReqMsg1 & REQ_SENSOR)
	{
		sdata[0]=acc.x;sdata[1]=acc.y;sdata[2]=acc.z;
		sdata[3]=gyro.x;sdata[4]=gyro.y;sdata[5]=gyro.z;
		XDAA_Send_S16_Data(sdata,6,P_SENSOR);
		ReqMsg1 &=~ REQ_SENSOR;
	}
	if(ReqMsg1 & REQ_RC)
	{
		XDAA_Send_S16_Data(RCdata,4,P_CTRL);
		ReqMsg1 &=~ REQ_RC;
	}
	if(ReqMsg1 & REQ_MOTOR)
	{
		sdata[0]=MOTOR1;
		sdata[1]=MOTOR2;
		sdata[2]=MOTOR3;
		sdata[3]=MOTOR4;
		XDAA_Send_S16_Data(sdata,4,P_MOTOR);
		ReqMsg1 &=~ REQ_MOTOR;
	}
	//��λ������2
	if(ReqMsg2 & REQ_ROL_CTRL)
	{
		sdata[0]=(s16)(adrR.KpIn*1000);
		sdata[1]=(s16)(adrR.KdIn*1000);
		sdata[2]=(s16)(adrR.KpOut*1000);
		sdata[3]=(s16)(adrR.Kw*1000);
		XDAA_Send_S16_Data(sdata,4,P_ROL_CTRL);
		ReqMsg2 &=~ REQ_ROL_CTRL;
	}
	if(ReqMsg2 & REQ_PIT_CTRL)
	{
		sdata[0]=(s16)(adrP.KpIn*1000);
		sdata[1]=(s16)(adrP.KdIn*1000);
		sdata[2]=(s16)(adrP.KpOut*1000);
		sdata[3]=(s16)(adrP.Kw*1000);
		XDAA_Send_S16_Data(sdata,4,P_PIT_CTRL);
		ReqMsg2 &=~ REQ_PIT_CTRL;
	}
	if(ReqMsg2 & REQ_ROL_STAT)
	{
		sdata[0]=(s16)(adrR.SpeEst*100);
		sdata[1]=(s16)(adrR.u*100);
		sdata[2]=(s16)((gx-adrR.SpeEst)*100);
		sdata[3]=(s16)(adrR.AccEst*100);
		XDAA_Send_S16_Data(sdata,4,P_ROL_STAT);
		ReqMsg2 &=~ REQ_ROL_STAT;
	}
	if(ReqMsg2 & REQ_PIT_STAT)
	{
		sdata[0]=(s16)(adrP.SpeEst*100);
		sdata[1]=(s16)(adrP.u*100);
		sdata[2]=(s16)((gy-adrP.SpeEst)*1000);
		sdata[3]=(s16)(adrP.AccEst*100);
		XDAA_Send_S16_Data(sdata,4,P_PIT_STAT);
		ReqMsg2 &=~ REQ_PIT_STAT;
	}
	Total_Send();
}
