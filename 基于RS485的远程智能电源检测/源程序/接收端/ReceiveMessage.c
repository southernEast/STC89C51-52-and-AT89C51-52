#include "ReceiveMessage.h"

unsigned char buf1[17];
unsigned char buf2[17];

void RS485Init()	//RS485����
{
	RE = 0;
	DE = 0;
}

void Init(void)		//��������
{
    TMOD=0x20;   	//��ʱ��1��������ʽ2              
    TH1=0xfd;		//������Ϊ9600
    TL1=0xfd;
    PCON=0x00;              
    TR1=1;     		//��ʱ��1��ʼ���� 
    SCON=0x50;		//0101 0000B��������ʽ1,8λUART�������ʿɱ䣬�������             
		RI = 0;
		TI = 0;   
} 


void receiveString1()
{
	char *pt = buf1;
	int num = 0;
	memset(buf1, 0, sizeof(buf1));
	while(num < 16){
		RI = 0;
		while(RI==0)
			;
		*pt = SBUF;
		num++;
		RI=0;
		pt++;
	}
}

void receiveString2()
{
	char *pt = buf2;
	int num = 0;
	memset(buf2, 0, sizeof(buf2));
	while(num < 16){
		RI = 0;
		while(RI==0)
			;
		*pt = SBUF;
		num++;
		RI=0;
		pt++;
	}
}

void show()
{
	display_1602(buf1, 0, 0);	// ��ʾ��һ��
	display_1602(buf2, 0, 1);	// ��ʾ�ڶ���
}