#include "imu.h"
/**************�ļ�˵��**********************
����������У׼,�˲�,�����˲�������Ԫ��
********************************************/

#define T  0.002    //��������,2ms
#define hT 0.001   //��������/2
#define qT 0.0005  //��������/4
#define Kp 2.0f
#define Ki 0.1f
const float accA[3][3]={
	{0.984375f,-0.019751222473827f,0.005168265332243f},
	{-0.0390625f,1,0.046875f},
	{-0.0234375f,0.0390625f,1}};
short accB[3]={-4955,-3893,1866};
short gyroB[3]={447,45,-1};

/***********************
������ȷ����У׼����У�����ٶȼ�ԭʼ����
**********************/
void Acc_Correct(AxisInt *acc)
{
	float ax=acc->x,ay=acc->y,az=acc->z;
	acc->x=accA[0][0]*ax+accA[0][1]*ay+accA[0][2]*az+accB[0];
	acc->y=accA[1][0]*ax+accA[1][1]*ay+accA[1][2]*az+accB[1];
	acc->z=accA[2][0]*ax+accA[2][1]*ay+accA[2][2]*az+accB[2];
}
/***********************
������ȷ����У׼����У��������ԭʼ����
**********************/
void Gyro_Correct(AxisInt *gyro)
{
	gyro->x+=gyroB[0];
	gyro->y+=gyroB[1];
	gyro->z+=gyroB[2];
}

/***********************
���ݼ��ٶȼ�ԭʼ���ݼ���У׼����
**********************/
u8 Acc_Calibrate(AxisInt acc)
{
	static long sumx=0,sumy=0,sumz=0;
	static char cnt=0;
	acc.z-=16384;
	if(cnt<50)
	{
		sumx+=acc.x;
		sumy+=acc.y;
		sumz+=acc.z;
		cnt++;
		return 1;
	}
	else
	{
		accB[0]-=sumx/50;
		accB[1]-=sumy/50;
		accB[2]-=sumz/50;
		sumx=sumy=sumz=0;
		cnt=0;
		return 0;
	}
}
/***********************
����������ԭʼ���ݼ���У׼����
**********************/
u8 Gyro_Calibrate(AxisInt gyro)
{
	static long sumx=0,sumy=0,sumz=0;
	static char cnt=0;
	if(cnt<50)
	{
		sumx+=gyro.x;
		sumy+=gyro.y;
		sumz+=gyro.z;
		cnt++;
		return 1;
	}
	else
	{
		gyroB[0]-=sumx/50;
		gyroB[1]-=sumy/50;
		gyroB[2]-=sumz/50;
		sumx=sumy=sumz=0;
		cnt=0;
		return 0;
	}
}

/***********************
�����ںϻ����˲�
**********************/
void IMUupdate(AxisInt acc,AxisInt gyro,Quaternion *Q)
{
	float ax=acc.x,ay=acc.y,az=acc.z;  //��һ�����ٶȼ������ݴ�
	if(ax==0 && ay==0 && az==0)return;
	float q0=Q->q0,q1=Q->q1,q2=Q->q2,q3=Q->q3;  //��Ԫ���ݴ�
	if(q0==0 && q1==0 && q2==0 && q3==0)return;
	static float ogx=0,ogy=0,ogz=0;  //��һʱ�̵Ľ��ٶ�
	static float oq0=1,oq1=0,oq2=0,oq3=0;  //��һʱ�̵���Ԫ��
	static float exInt=0,eyInt=0;
	//�������ٶȹ�һ��
	float norm=Q_rsqrt(ax*ax+ay*ay+az*az);
	ax*=norm;ay*=norm;az*=norm;
	//��ȡ��Ԫ���ĵ�Ч���Ҿ����е���������
	float vx=2*(q1*q3-q0*q2);
	float vy=2*(q0*q1+q2*q3);
	float vz=1-2*(q1*q1+q2*q2);
	//��������ó���̬���
	float ex=ay*vz-az*vy; 
	float ey=az*vx-ax*vz;
	//�������л���
	exInt+=ex*Ki;
	eyInt+=ey*Ki;
	//��̬���������ٶ���,�������ٶȻ���Ư��
	float gx=GyroToRad(gyro.x)+Kp*ex+exInt;
	float gy=GyroToRad(gyro.y)+Kp*ey+eyInt;
	float gz=GyroToRad(gyro.z);
	//�Ľ�ŷ������ֵ�����Ԫ��΢�ַ���
	float K0=-oq1*ogx-oq2*ogy-oq3*ogz;
	float K1=oq0*ogx-oq3*ogy+oq2*ogz;
	float K2=oq3*ogx+oq0*ogy-oq1*ogz;
	float K3=-oq2*ogx+oq1*ogy+oq0*ogz;
	K0+=-q1*gx-q2*gy-q3*gz;
	K1+=q0*gx-q3*gy+q2*gz;
	K2+=q3*gx+q0*gy-q1*gz;
	K3+=-q2*gx+q1*gy+q0*gz;
	q0+=qT*K0;
	q1+=qT*K1;
	q2+=qT*K2;
	q3+=qT*K3;
	//��Ԫ����һ�������,��ֵ����
	norm=Q_rsqrt(q0*q0+q1*q1+q2*q2+q3*q3);
	Q->q0=q0*norm;Q->q1=q1*norm;Q->q2=q2*norm;Q->q3=q3*norm;
	oq0=Q->q0;oq1=Q->q1;oq2=Q->q2;oq3=Q->q3;
	ogx=gx;ogy=gy;ogz=gz;
}
