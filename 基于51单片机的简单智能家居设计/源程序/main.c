#include "GSM.h"
#include "LCD1602.h"
#include "BUZZ.h"
#include "beam.h"
#include "TemAndHum.h"
#include "Gas.h"

int sendFlag = 0;

void main(void)
{
	u8 i;
	int ret;
	Int0Init();			// ��ʼ���ⲿ�ж�0
	Int1Init();			// ��ʼ���ⲿ�ж�1
	Init_BH1750();
	Init_LCD1602();
	Uart1Init();   	// ���ڳ�ʼ��
	Timer0Init();		// TIM0��ʼ��
	
	for(i = 0;i < STABLE_TIMES;i++){ 				//�ȴ������ȶ�
		delay_ms(50);
	}
	LCD1602_write_com(0x80);
	LCD1602_write_word("Welcome to use! ");
	delay_ms(50);
	while(1){
		Show_Beam();												// ��ʾ���ն�
		ShowTemAndHum();										// ��ʾ��ʪ��
		delay_ms(10);	
		if(sendFlag == 1){
			LCD1602_write_com(0x80);
			LCD1602_write_word("Sending message!");
			LCD1602_write_com(0x80 + 0x40);
			LCD1602_write_word("                ");
			ret = send_text_message("Attention the fire!");// ����TEXT����
			if(ret == 1){
				LCD1602_write_com(0x80);
				LCD1602_write_word("Send Successful!");
				LCD1602_write_com(0x80 + 0x40);
				LCD1602_write_word("                ");
				delay_ms(50);
			}
			else{
				LCD1602_write_com(0x80 + 0x40);
				LCD1602_write_word("      Fail      ");
			}	
			sendFlag = 0;
		}
	}
}



