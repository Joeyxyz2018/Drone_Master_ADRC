#include "protocol.h"
/**************�ļ�˵��**********************
����վ�ͷɿ�ͨ�����ڽ�������ͨ��
********************************************/
//���ݲ�ֺ궨�壬�ڷ��ʹ���1�ֽڵ���������ʱ������int16��float�ȣ���Ҫ�����ݲ�ֳɵ����ֽڽ��з���
#define BYTE0(dwTemp)       ( *( (char *)(&dwTemp)		) )
#define BYTE1(dwTemp)       ( *( (char *)(&dwTemp) + 1) )
#define BYTE2(dwTemp)       ( *( (char *)(&dwTemp) + 2) )
#define BYTE3(dwTemp)       ( *( (char *)(&dwTemp) + 3) )

/*�봮��Ӳ����صĳ���*/
u8 RxData;
u8 RxTemp[14];  //��ʱ���洮�ڽ��յ��Ĵ�������
short CtrlCmd[4];
u8 RCcmd=0;
/**
����DMA����ͨ��
*/
void Protocol_Init(void)
{
	HAL_UART_Receive_DMA(&huart2,&RxData,1);
}
/**
�����ֽڴ���
*/
u8 XDAA_Data_Receive_Precess(void)
{
	static u8 RxState=0;
	static u8 sum=0,i=2;
	switch(RxState)
	{
	case 0:  //֡ͷУ��
		if(RxData=='<')
		{
			sum=RxData;
			RxState=1;
		}
		break;
	case 1:  //������У���뱣��
		if(RxData<=4)
		{
			sum+=RxData;
			RxTemp[0]=RxData;
			RxState=2;
		}
		else if(RxData=='<')  //��֡ͷ���¿�ʼ
			sum=RxData;
		else
		{
			sum=0;
			RxState=0;
		}
		break;
	case 2:  //���ݳ���У���뱣��
		if(RxState<=12)
		{
			sum+=RxData;
			RxTemp[1]=RxData;
			RxState=3;
		}
		else if(RxData=='<')  //��֡ͷ���¿�ʼ
		{
			sum=RxData;
			RxState=1;
		}
		else
		{
			sum=0;
			RxState=0;
		}
		break;
	case 3:  //��ʱ�����������
		sum+=RxData;
		RxTemp[i++]=RxData;
		if(i>=RxTemp[1]+2)
			RxState=4;
		break;
	case 4:  //ƥ��У���
		RxState=0;
		i=2;
		if(sum==RxData)
			return 0;  //����������Ч
		break;
	default:break;
	}
	return 1;  //����������Ч
}
/**
����ͨ��DMA��ʽ���յ�һ���ֽ�
*/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(XDAA_Data_Receive_Precess())
		return;
	u8 len=RxTemp[1]/2+1;
	switch(RxTemp[0])
	{
	case FLY_CTRL:
		for(u8 i=1;i<len;i++)
			CtrlCmd[i-1]=(RxTemp[2*i]<<8) | RxTemp[2*i+1];
		break;
	default:break;
	}
}

/*Э����ʽ��ʼ*/
u8 DataToSend[16];
/**
�ɿ�->����վ �����û��Զ���֡
*@UserFrame:�û���������
*@len:���鳤��(30����)
*@fun:������(0xF1~0xFA)
*/
void XDAA_Send_User_Data(s16 *UserFrame,u8 len)
{
	u8 i,cnt=0,checksum=0;
	s16 temp;
	DataToSend[cnt++]='>';
	DataToSend[cnt++]=0xF1;
	DataToSend[cnt++]=len*2;
	for(i=0;i<len;i++)
	{
		temp = UserFrame[i];
		DataToSend[cnt++]=BYTE1(temp);
		DataToSend[cnt++]=BYTE0(temp);
	}
	for(i=0;i<DataToSend[2];i++)
		checksum+=DataToSend[i+2];
	DataToSend[cnt++]=checksum;
	XDAA_Send_Data(DataToSend,cnt);
}
