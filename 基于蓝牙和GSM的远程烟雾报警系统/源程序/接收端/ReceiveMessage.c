#include "ReceiveMessage.h"

unsigned char flag;

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

void delay(unsigned int i)
{
	while(i--);
}

void receive()			// ������Ϣ
{
	while(RI==0)
		;
	flag = SBUF;
	RI = 0;
//	send(flag);
}

void send(uchar ShowData)
{
	SBUF=ShowData;	 		//д���ͼĴ���
	while(TI==0)		 	//�ȴ����з������
	{
	;
	}
	TI=0;  					//�Ա�־λ����
}

int select()
{
	if(flag == 1)		// ��������
		return 1;
	else						// δ��������
		return 0;
}


