#include "SendMessage.h"

void Init_RS485()
{
	RE = 1;
	DE = 1;							// ����Ϊ����ģʽ
}


void Init_Timer0(void)
{
 TMOD |= 0x01;	  //ʹ��ģʽ1��16λ��ʱ����ʹ��"|"���ſ�����ʹ�ö����ʱ��ʱ����Ӱ��		     
 TH0=(65536-20000)/256;		  //���¸�ֵ 20ms
 TL0=(65536-20000)%256;
 EA=1;            //���жϴ�
 ET0=1;           //��ʱ���жϴ�
 TR0=1;           //��ʱ�����ش�
}

void Timer0_isr(void) interrupt 1 
{
	TH0=(65536-20000)/256;	//���¸�ֵ 20ms
	TL0=(65536-20000)%256;	
	time_20ms++;
	if(time_20ms%2==0)		 //��ʱ������ʾ
	{
		disFlag = 1;
	}

}
void UART_Init(void)
{
    SCON  = 0x50;		       		 	// SCON: ģʽ 1, 8-bit UART, ʹ�ܽ���  
    TMOD |= 0x20;               // TMOD: timer 1, mode 2, 8-bit ��װ
    TH1   = 0xFD;               // TH1:  ��װֵ 9600 ������ ���� 11.0592MHz  
    TR1   = 1;                  // TR1:  timer 1 ��                         
    EA    = 1;                  //�����ж�
    ES    = 1;                  //�򿪴����ж�
}

void uartSendByte(unsigned char dat)
{
	unsigned char time_out;
	time_out=0x00;
	SBUF = dat;			  //�����ݷ���SBUF��
	while((!TI)&&(time_out<100))  //����Ƿ��ͳ�ȥ
	{time_out++;DelayUs2x(10);}	//δ���ͳ�ȥ ���ж�����ʱ
	TI = 0;						//���ti��־
}

void uartSendStr(unsigned char *s,unsigned char length)
{
	unsigned char NUM;
	NUM=0x00;
	while(NUM<length)	//���ͳ��ȶԱ�
	{
		uartSendByte(*s);  //���ɵ��ֽ�����
		s++;		  //ָ��++
		NUM++;		  //��һ��++
  }
}
void UART_SER (void) interrupt 4 //�����жϷ������
{
	unsigned char u_buf;
  if(RI)                        //�ж��ǽ����жϲ���
	{
		RI=0;                      //��־λ����
		u_buf = SBUF;
	}
  if(TI)  //����Ƿ��ͱ�־λ������
		TI=0;
}

