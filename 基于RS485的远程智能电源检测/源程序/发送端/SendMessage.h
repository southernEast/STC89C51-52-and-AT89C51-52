#ifndef __SENDMESSAGE_H__
#define __SENDMESSAGE_H__
#include <reg52.h>
#include "delay.h"

sbit relay =P1^0;						// �̵���
sbit buzzer =P1^3;					// ������
sbit RE = P0^7;							// 485����
sbit DE = P0^6;

extern unsigned char disFlag;		//������ʾ��־
extern unsigned int times;			//ʱ�����
extern unsigned long time_20ms;	//��ʱ����
extern float volt,current,power;	//��ѹ ���� ���ʱ���

extern unsigned char count;				//�ɼ�����
extern xdata float sumVolt,midV;	//�����˲� �м����
extern xdata float sumAcur,midA;	//�����˲� �м����

extern unsigned char dis0[16];		//Һ����ʾ��һ��
extern unsigned char dis1[16];		//Һ����ʾ�ڶ���


extern void Init_RS485();
extern void Init_Timer0(void);		//��������
extern void UART_Init(void);
extern void uartSendByte(unsigned char dat);
extern void uartSendStr(unsigned char *s,unsigned char length);

#endif