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
ADRC_Param adrcRoll,adrcPitch;  //�Կ��ſ���������(���ļ�ȫ�ֱ���)
u8 GlobalStat=0;  //ȫ��״̬(���ļ�ȫ�ֱ���)
float IIRroll[3],IIRpitch[3],IIRyaw[3];

/***********************
��̬�������,MPU6050����У׼
*@period:2ms
**********************/
void IMU_Processing(void)
{
	MPU_Get_Accelerometer(&acc.x,&acc.y,&acc.z);
	MPU_Get_Gyroscope(&gyro.x,&gyro.y,&gyro.z);
	oacc=acc;ogyro=gyro;
//	gyro.x=IIR_LowPassFilter(gyro.x,IIRroll);
//	gyro.y=IIR_LowPassFilter(gyro.y,IIRpitch);
//	gyro.z=IIR_LowPassFilter(gyro.z,IIRyaw);
	Acc_Calibrate(&acc);
//	GYRO_Calibrate(&gyro);
	IMUupdate(acc,&gyro,&Qpos);
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
		if(RCmsg & MOTOR_LOCK)
			GlobalStat|=MOTOR_LOCK;
		else
			GlobalStat&=~MOTOR_LOCK;
		break;
	case P_CTRL:
		RCdata[0]=RCchannel[0];
		RCdata[1]=RCchannel[1];
		RCdata[2]=RCchannel[2];
		RCdata[3]=RCchannel[3];
		break;
	default:break;
	}
	if((Qpos.q1>0.7)||(Qpos.q2>0.7))
		Fail_Safe();
}

/***********************
��ʱ����Ƿ��յ�ң�����ź�
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
}

void Send_Data(void)
{
	s16 data[6];
	static u8 count=0;
	u8 udata[2]={GlobalStat,(u8)AdcData};
	data[0]=acc.x;data[1]=acc.y;data[2]=acc.z;
	data[3]=gyro.x;data[4]=gyro.y;data[5]=gyro.z;
	XDAA_Send_S16_Data(data,6,P_SENSOR);
	XDAA_Send_S16_Data(RCdata,4,P_CTRL);	
	float roll=Matan2(2*(Qpos.q0*Qpos.q1+Qpos.q2*Qpos.q3),1-2*(Qpos.q1*Qpos.q1+Qpos.q2*Qpos.q2))*57.3f;
	float pitch=Masin(2*(Qpos.q0*Qpos.q2-Qpos.q1*Qpos.q3))*57.3f;
	float yaw=Matan2(2*(Qpos.q1*Qpos.q2+Qpos.q0*Qpos.q3),1-2*(Qpos.q2*Qpos.q2+Qpos.q3*Qpos.q3))*57.3f;
	data[0]=(s16)(roll*100);
	data[1]=(s16)(pitch*100);
	data[2]=(s16)(yaw*100);
	XDAA_Send_S16_Data(data,3,P_ATTI);
	count++;
	if(count>=10)
	{
		XDAA_Send_U8_Data(udata,2,1);
		count=0;
	}
	Total_Send();
}
