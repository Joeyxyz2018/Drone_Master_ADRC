#include "adrc.h"
/**************�ļ�˵��**********************
�Կ��ſ�������غ���,��һ�����еĺ�����������
����һ�����Կ��ſ����޹ص������Ԫ�����㺯��
********************************************/
#define adrcR   4
#define adrcH   0.025
#define adrcD   0.0025
#define T       0.02

/**********************
��ɢϵͳ���ٿ����ۺϺ���(bug)
*@x1:���׻����������
*@x2:���׻����������΢��
*@return:���������
**********************/
float ADRC_fhan(float x1, float x2)
{
	float y1 = x1 + adrcH*x2;
	float y2 = x1 + 2 * adrcH*x2;
	if ((y1 >= -adrcD) && (y1 <= adrcD) && (y2 >= -adrcD) && (y2 <= adrcD))
		return -y2 / adrcH / adrcH;
	float u = (1 + Msqrt(1 + 8 * ABS(y1) / adrcD)) / 2 + 0.0001f;
	short k = (short)u;
	if (k > 2)
		u = (1 - k / 2.0f)*adrcR*SIGN(y1) - (x1 + k*adrcH*x2) / ((k - 1)*adrcH*adrcH);
	else if (k == 2)
		u = -adrcR / 2.0f*SIGN(y1) - (x1 + 3 * adrcH*x2) / 2 * adrcH*adrcH;
	else if (k == 1)
		u = -adrcR*SIGN(x2);
	return LIMIT(u, -adrcR, adrcR);
}

/**********************
����΢����(bug)
*@u:����
*@derivative:ԭ�źŵ�΢�����
*@return:ԭ�źŵ��˲����
**********************/
float ADRC_TD(float r,float *derivative)
{
	static float x1=0,x2=0;
	float u=ADRC_fhan(x1-r,x2);
	x1+=x2;
	x2+=u;
	*derivative=x2;
	return x1;
}

/**********************
����������(bug)
**********************/
float ADRC_fal(float x)
{
	float y;
	if(ABS(x)<=0.02)
		y=x*0.1414213562;
	else
		y=Msqrt(ABS(x))*SIGN(x);
	return y;
}

/**********************
����״̬�۲���(bug)
**********************/
float ADRC_ESO(float u,float y,float b)
{
	static float z1=0,z2=0;
	float e=z1-y;
	z1+=b*u+z2-50*e;
	z2-=220*ADRC_fal(e);
	float w=z2/b;
	return w;
}

/**********************
��������״̬�۲���
x1'=x2;x2'=-x2+bu+w;
*@u:�����ź�
*@y:������ٶ�
*@b:�����ź�����
*@z1:���ٶ�x1�Ĺ���ֵ(����ʱӦ�г�ʼֵ)
*@z2:�Ǽ��ٶ�x2�Ĺ���ֵ(����ʱӦ�г�ʼֵ)
*@return:���Ŷ�w
**********************/
float ADRC_LESO(float u,float y,float b,float *z1,float *z2)
{
	static float w=0;
	float e=y-*z1;
	*z1+=(*z2+6*e)*T;
	*z2+=(-1**z2+b*u+w+12*e)*T;
	w+=8*e*T;
	return w;
}

/**********************
�����̬��Ԫ��pos��������Ԫ��exp֮��������Ԫ��
**********************/
Quaternion Quaternion_Error(Quaternion E,Quaternion P)
{
	Quaternion ans;
	ans.q0=E.q0*P.q0+E.q1*P.q1+E.q2*P.q2+E.q3*P.q3;
	ans.q1=P.q0*E.q1-E.q0*P.q1+E.q3*P.q2-E.q2*P.q3;
	ans.q2=P.q0*E.q2-E.q0*P.q2+E.q1*P.q3-E.q3*P.q1;
	ans.q3=P.q0*E.q3-E.q0*P.q3+E.q2*P.q1-E.q1*P.q2;
	return ans;
}
