#include "SendMessage.h"

uchar flag;

void Gas()
{
	uchar i = 0;
	if(gasDet == 0){					
		Delay(100);
		if(gasDet == 0){				// ��⵽����
			led1 = 0;							// ����Ӧ����
			led2 = 1;
			for(i = 0; i < 10; i++)
				Send(1);						// ������Ϣ
		}
	}
	else{											// δ��⵽����
		led1 = 1;								// �ر�Ӧ����
		led2 = 0;
		Send(0);								// ������Ϣ
	}
}

void Delay(uint k)
{
	uchar j;
	while((k--)!=0)           
	{
		for(j=0;j<125;j++)
			{;}
	}
}


void Init(void)
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
                        
void Send(uchar ShowData)
{
	SBUF=ShowData;	 		//д���ͼĴ���
	while(TI==0)		 	//�ȴ����з������
		;
	TI=0;  					//�Ա�־λ����
}