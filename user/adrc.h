#ifndef __ADRC_H
#define __ADRC_H

#include "mymath.h"

float ADRC_fhan(float x1,float x2);
float ADRC_TD(float r,float *derivative);
float ADRC_fal(float x);
float ADRC_ESO(float u,float y,float b);
float ADRC_LESO(float u,float y,float b,float *z1,float *z2);
Quaternion Quaternion_Error(Quaternion E,Quaternion P);

//�Կ��ſ������Ŀ��Ʋ��������в���
//�Կ��ſ���������ƽ�Ϊ���,ʹ�õĲ������ܻ�Ƶ���Ķ�
typedef struct
{
	//���Ʋ���
	float KpOut;  //�⻷λ�ñ�������
	float KpIn;   //�ڻ��ٶȱ�������
	float KdIn;   //�ڻ����ٶȱ�������
	//���в���
	float PosOut;    //λ��������
	float SpeOut;    //�ٶ�������
	float gEst;      //���ٶȵ�״̬����
	float AccEst;    //�Ǽ��ٶȵ�״̬����
}ADRC_Param;

#endif
