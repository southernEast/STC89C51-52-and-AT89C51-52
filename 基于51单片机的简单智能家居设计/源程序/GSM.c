#include "GSM.h"

xdata u8 Uart1_Buf[Buf_Max];
u8 First_Int = 0;
u16 count_20ms;
u8  *phone_num = "AT+CMGS=\"13394308020\""; //����绰���޸���������޸Ĳ���ĵ绰��

void Timer0Init(void)		//20����@115200
{
	TMOD &= 0xF0;		
	TMOD |= 0x01;		//
	TL0 = (65536-20000)%256;		//
	TH0 = (65536-20000)/256;		//
	TF0 = 0;		//
	ET0 = 1;    	  //
	TR0 = 1;		//��ʼ��ʱ
}

void Uart1Init(void)		//9600bps@11.05926MHz
{
	PCON &= 0x7F;		// �����ʲ�����
	SCON = 0x50;		// ��ʽ1 ����REN
	TMOD &= 0x0F;		// 
	TMOD |= 0x20;		// ��ʽ2 8λ�Զ���װ��
	TL1 = 0xFD;		  // ����ֵ
	TH1 = 0xFD;		  //
	ET1 = 0;		    // �رն�ʱ���ж�
	TR1 = 1;		    // ������ʱ��
	ES  = 1;				// ���������ж�
}

int check_status(void)
{
	int ret;
	
	ret = UART1_Send_AT_Command("AT","OK",3,50);//����ͨ���Ƿ�ɹ�
	if(ret == 0)
	{
		return COMMUNITE_ERROR;
	}
	
	ret = UART1_Send_AT_Command("AT+CPIN?","READY",2,50);//��ѯ���Ƿ����
	if(ret == 0)
	{
		return NO_SIM_CARD_ERROR;
	}
	
	ret = Wait_CREG(3);//��ѯ���Ƿ�ע�ᵽ����
	if(ret == 0)
	{
		return SIM_CARD_NO_REG_ERROR;
	}
	return 1;
}

/***************************************************************
ע����Ȼ����Է�������ֵ����ȷ����������һ������ָ�����ʧ���ˡ�
****************************************************************/
int send_text_message(char *content)
{
	u8 ret;
	char end_char[2];
	
	end_char[0] = 0x1A;//�����ַ�
	end_char[1] = '\0';
	
	//���ô洢λ��

	UART1_Send_Command("AT+CSCS=\"GSM\"");
	delay_ms(50);
	UART1_Send_Command("AT+CMGF=1");
	delay_ms(80);
	UART1_Send_Command(phone_num);
	delay_ms(80);
	UART1_SendString(content); 
	UART1_Send_Command(end_char);
//	UART1_Send_Command("ATD15543777824;");
	
	return 1;
}

/*******************************************************************************
* ������ : CLR_Buf
* ����   : �������2��������
* ����   : 
* ���   : 
* ����   : 
* ע��   : 
*******************************************************************************/
void CLR_Buf(void)
{
	u8 k;
	for(k=0;k<Buf_Max;k++)      //��������������
	{
		Uart1_Buf[k] = 0x00;
	}
    First_Int = 0;              //�����ַ�������ʼ�洢λ��
}


/*******************************************************************************
* ������ : Find
* ����   : �жϻ������Ƿ���ָ�����ַ���
* ����   : 
* ���   : 
* ����   : unsigned char:1 �ҵ�ָ���ַ���0 δ�ҵ�ָ���ַ� 
* ע��   : 
*******************************************************************************/

u8 Find(u8 *a)
{ 
	ES = 0;  //�Ľ�����
	if(strstr(Uart1_Buf,a)!=NULL)
	{
		ES = 1;
		return 1;
	}	
	else
	{
		ES = 1;
		return 0;
	}
		
}



/*******************************************************************************
* ������ : Wait_CREG
* ����   : �ȴ�ģ��ע��ɹ�
* ����   : 
* ���   : 
* ����   : 
* ע��   : 
*******************************************************************************/
u8 Wait_CREG(u8 query_times)
{
	u8 i;
	u8 k;
	u8 j;
	i = 0;
	CLR_Buf();
	while(i == 0)        			
	{

		UART1_Send_Command("AT+CREG?");
		delay_ms(100); 
		
		for(k=0;k<Buf_Max;k++)      			
		{
			if((Uart1_Buf[k] == '+')&&(Uart1_Buf[k+1] == 'C')&&(Uart1_Buf[k+2] == 'R')&&(Uart1_Buf[k+3] == 'E')&&(Uart1_Buf[k+4] == 'G')&&(Uart1_Buf[k+5] == ':'))
			{
					 
				if((Uart1_Buf[k+7] == '1')&&((Uart1_Buf[k+9] == '1')||(Uart1_Buf[k+9] == '5')))
				{
					i = 1;
					return 1;
				}
				
			}
		}
		j++;
		if(j > query_times)
		{
			return 0;
		}
		
	}
	return 0;
}



/*----------------------------
UART1 ���ʹ�������
-----------------------------*/
void UART1_SendData(u8 dat)
{
	ES=0;			//�ش����ж�
	SBUF=dat;			
	while(TI!=1);	//�ȴ����ͳɹ�
	TI=0;			//��������жϱ�־
	ES=1;			//�������ж�
}
/*----------------------------
UART1 �����ַ���
-----------------------------*/
void UART1_SendString(char *s)
{
	while(*s)//����ַ���������
	{
		UART1_SendData(*s++);//���͵�ǰ�ַ�
	}
}

/**********************************
��������
**********************************/
void UART1_Send_Command(char *s)
{
	CLR_Buf(); 
	while(*s)//����ַ���������
	{
		UART1_SendData(*s++);//���͵�ǰ�ַ�
	}
	UART1_SendLR();
}

void UART1_Send_Command_END(char *s)
{
	CLR_Buf(); 
	while(*s)//����ַ���������
	{
		UART1_SendData(*s++);//���͵�ǰ�ַ�
	}
}

/*******************************************************************************
* ������ : Second_AT_Command
* ����   : ����ATָ���
* ����   : �������ݵ�ָ�롢ϣ���յ���Ӧ�𡢷��͵ȴ�ʱ��(��λ��interval_time*20 ms)
* ���   : 
* ����   : 
* ע��   : 
*******************************************************************************/

u8 UART1_Send_AT_Command(u8 *b,u8 *a,u8 wait_time,u16 interval_time)         
{
	u8 i;

	CLR_Buf(); 
	i = 0;
	while(i < wait_time)                    
	{
		UART1_Send_Command(b);
		delay_ms(interval_time);
		if(Find(a))            //������ҪӦ����ַ�
		{
			return 1;
		}
		i++;
	}
	
	return 0;
}

void delay_ms(u16 ms)
{
	 count_20ms = ms;
	 while(count_20ms);
}


u8 UART1_Send_AT_Command_END(u8 *b,u8 *a,u8 wait_time,u16 interval_time)         
{
	u8 i;

	CLR_Buf(); 
	i = 0;
	while(i < wait_time)                    
	{
		UART1_Send_Command_END(b);
		delay_ms(interval_time);
		if(Find(a))            //������ҪӦ����ַ�
		{
			return 1;
		}
		i++;
	}
	
	return 0;
}

/*******************************************************************************
* ������ : Timer0_ISR
* ����   : ��ʱ��0�жϷ�����ں���,1ms�ж�һ��
* ����   : 
* ���   : 
* ����   : 
* ע��   : 
*******************************************************************************/
void Timer0_ISR() interrupt 1
{
	TR0=0;//�ض�ʱ��
	TL0 = (65536-20000)%256;		//���趨ʱ����ֵ
	TH0 = (65536-20000)/256;		//
	
	if(count_20ms > 0) //20ms��ʱ������
	{
		count_20ms--;
	}	
	
	TR0=1;//����ʱ��
}


/********************* UART1�жϺ���************************/
void UART1_ISR (void) interrupt 4
{
	if (RI)
	{
		RI = 0;                           //���RIλ
		Uart1_Buf[First_Int] = SBUF;  	  //�����յ����ַ����浽������
		First_Int++;                	  //����ָ������ƶ�
		if(First_Int >= Buf_Max)       	  //���������,������ָ��ָ�򻺴���׵�ַ
		{
			First_Int = 0;
		}		
	}
	if (TI)
	{
		TI = 0;                          //���TIλ
	}
}
