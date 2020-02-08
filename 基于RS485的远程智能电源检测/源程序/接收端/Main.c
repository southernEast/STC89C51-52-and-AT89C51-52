#include <reg52.h>
#include <stdio.h>
#include "LCD1602.h"
#include "ReceiveMessage.h"

#define uchar unsigned char
#define uint  unsigned int


void delay(uint k);

void main(void)
{
	InitLcd();      // lcd��ʼ��
	display_1602("Welcome to use!!", 0, 0);
	Init();					// ���ڳ�ʼ��
	RS485Init();		// RS485��ʼ��

	while(1)
	{
		receiveString1();					// ���յ�һ��
		receiveString2();					// ���յڶ���
		delay(100);

		show();										// ��ʾ����
	}
}

void delay(uint k)	
{						
	uint i,j;				
	for(i=0;i<k;i++)
	{			
		for(j=0;j<121;j++)			
		{;}
	}						
}