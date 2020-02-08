#include <reg52.h> 
#include <intrins.h>
#include <stdio.h>
#include "i2c.h"
#include "delay.h"
#include "SendMessage.h"

unsigned char disFlag ;		//������ʾ��־
unsigned int times = 0;		//ʱ�����
unsigned long time_20ms=0;	//��ʱ����
float volt,current,power;	 //��ѹ ���� ���ʱ���

unsigned char count =0;//�ɼ�����
xdata float sumVolt,midV; //�����˲� �м����
xdata float sumAcur,midA;		//�����˲� �м����

unsigned char dis0[16];		 //Һ����ʾ��һ��
unsigned char dis1[16];		 //Һ����ʾ�ڶ���

void main (void)
{
	Init_RS485();						// 485��ʼ��
	Init_Timer0();        	// ��ʱ��0��ʼ��
	UART_Init();	   				// ���ڳ�ʼ�� 
	
	relay = 0;buzzer = 0;		// �ϵ��������һ��
	DelayMs(120);
	DelayMs(20);          	// ��ʱ�������ȶ�

	buzzer = 1; 						// �رշ�����
	relay = 1;							// �պϼ̵���
	sumAcur = 0;						// ���ԭ������
	sumVolt = 0;						// ����ۼ�
	while(1)         				// ��ѭ��
	{
		if(disFlag ==1)				// ������ʾ��־ һ��ʱ�䷢��һ������
		{
			disFlag = 0;
			midA=(float)ReadADC(0)*2.7*5.15/255;				//��ȡAD0
			if(midA < 0.08) midA = 0;
			DelayMs(2);          //��ʱ�������ȶ�

			midV=(float)ReadADC(1)*321*5.15/255;				//��ȡAD1
			DelayMs(2);          //��ʱ�������ȶ�

			sumAcur = sumAcur + midA;	//��β�����ƽ��
			sumVolt = sumVolt + midV;	//��β�����ƽ��

			count++;//�ɼ�����
			if(count >= 5)
			{	  
				count = 0;
				current = 	sumAcur/5;  //Q��ƽ��
				if(current < 0.08) current= 0;//�˲�΢С����
				sumAcur = 0;	//���ԭ������
				
				volt = sumVolt/5;	 //�����ѹֵ
				if(volt < 15) volt= 0;//�˲�΢С����
				sumVolt = 0;//����ۼ�
				power =current*  volt;  //���ʼ���
				
				sprintf(dis0,"v:%5.1fV a:%3.1fA ",volt,current);   //��ӡ��ѹ����ֵ
				uartSendStr(dis0, 16);											// 485���͵�һ��
				
				sprintf(dis0,"P:%5.1fW  no<600 ",power);   //��ӡ����ֵ
				uartSendStr(dis0, 16);											// 485���͵ڶ���

				if(power>=600)//ֵ�Ա�
				{
				 	buzzer =0;	 //�򿪷�����
					relay = 0;	 //�Ͽ��̵���
				}
			}									
		}
	}
}

