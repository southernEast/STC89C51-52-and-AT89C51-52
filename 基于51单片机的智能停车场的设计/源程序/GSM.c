#include "GSM.h"

typedef unsigned char	u8;
typedef unsigned int	u16;
typedef unsigned long	u32;

/*************  ���ر�������	**************/
xdata u8 Uart1_Buf[Buf_Max];
u8 First_Int = 0;

u8 outInfo[50];						// ���
u16 count_20ms;
u8  receive_mode     = 0;
u8  find_string_flag = 0;
idata char num_message[4];//��ʱ�������ֻ�ܴ�999������
idata char AT_CMGR[15];

idata char  extract_phone_num[29];   // �����ݴ� 
idata char AT_CMGS_TEMP[29];
idata char phone_number[15];				// ���ͺ���
idata char end_char[2] = {0x1a, 0};

/*************	���غ�������	**************/
void delay_ms(u16 ms);   //��ʱms��20ms  ms*20ms
void CLR_Buf(void);      //�崮�ڽ��ջ���
u8 	 Find(u8 *a);        //�����ַ���
void UART1_SendData(u8 dat);   //����1���� 1�ֽ�
void UART1_SendString(char *s);//����1���� �ַ���
void UART1_Send_Command(char *s);
u8 UART1_Send_AT_Command(u8 *b,u8 *a,u8 wait_time,u16 interval_time);
u8 Wait_CREG(u8 query_times);


/**��ʱ����ʼ��**/
void Timer0Init(void)		//20����@115200
{
	TMOD &= 0xF0;					
	TMOD |= 0x01;					//		
	TL0 = (65536-20000)%256;		//
	TH0 = (65536-20000)/256;		//
	TF0 = 0;		//
	ET0 = 1;    	  //
	TR0 = 1;		//��ʼ��ʱ
}

/**���ڳ�ʼ��**/
void Uart1Init(void)		//9600bps@11.05926MHz
{
	PCON &= 0x7F;		// �����ʲ�����
	SCON = 0x50;		// ���ڷ�ʽ2
	TMOD &= 0x0F;		// ��ʱ��8λ�Զ���װ��
	TMOD |= 0x20;		//
	TL1 = 0xFD;		  //
	TH1 = 0xFD;		  //
	ET1 = 0;		  //
	TR1 = 1;		    //
	ES  = 1;					//
}

/**�ȴ�ģ��ע��**/
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

/**���״̬**/
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


/**���ý�����Ϣģʽ**/
int config_format(void)
{
	u8 ret;

	UART1_Send_Command("ATE0");					// �رջ���
	delay_ms(50);
	
	UART1_Send_Command("AT+CPMS=\"SM\",\"ME\",\"SM\""); 	// ���ô洢λ��
	delay_ms(50);

	ret = UART1_Send_AT_Command("AT+CNMI=2,1","OK",3,100);
	if(ret == 0)
	{
		return AT_CNMI_ERROR;
	}
	
	ret = UART1_Send_AT_Command("AT+CMGF=1","OK",3,50);		// ����ΪTEXTģʽ
	if(ret == 0)
	{
		return AT_CMGF_ERROR;
	}
	
	ret = UART1_Send_AT_Command("AT+CSCS=\"GSM\"","OK",3,50);// �����ַ���ʽ
	if(ret == 0)
	{
		return AT_CSCS_ERROR;
	}
	return 1;
}

/**��ȡ��Ϣλ�� �����Ͷ�ȡ������Ϣָ��**/
int read_message(void)
{
	char *buffer_ptr;
	u8 i,len;
	u8 ret;
	
	memset(num_message,'\0',4);			// �����ػ�����
	memset(AT_CMGR,'\0',15);
	buffer_ptr = strstr(Uart1_Buf,"\"SM\",");
	
	len = strlen("\"SM\",");

	i = len;
	if(buffer_ptr != NULL)					// ��ȡ��Ϣλ��
	{
		while(*(buffer_ptr + i) != 0x0D && i < (len+3))
		{
			num_message[i-len] = *(buffer_ptr + i);
			i++;
		}
	}
	else
	{
		return NO_SMS_LEN_ERROR;
	}
	
	memset(AT_CMGR,'\0',15);
	strcpy(AT_CMGR,"AT+CMGR=");				// ���ö�ȡ��Ϣָ��
	strcat(AT_CMGR,num_message);
	
	ret = UART1_Send_AT_Command(AT_CMGR,"OK",3,50*4);	// ����ָ��

	return ret;
}

/**���ռ���Ϣ�л�ȡ�������ֻ�����**/
void extract_phone_number(void)
{
	u8 pos;
	u8 i;

	memset(phone_number,'\0',12);		// ��ջ���
	
	for(i = 0;i < Buf_Max;i++)
	{
		if(Uart1_Buf[i] == '\"' && Uart1_Buf[i+1] == ',' && Uart1_Buf[i+2] == '\"')
		{
			pos = i + 3;
			break;
		}
	}
	i = 0;
	while(Uart1_Buf[pos] != '\"' && i < 14)
	{
		phone_number[i] = Uart1_Buf[pos];
		pos++;
		i++;
	}
	
	memset(AT_CMGS_TEMP,'\0',25);
	strcpy(AT_CMGS_TEMP,"AT+CMGS=\"");		// ���÷�����Ϣָ��
	strcat(AT_CMGS_TEMP,phone_number);
	strcat(AT_CMGS_TEMP,"\"\r");
	
	memset(extract_phone_num,'\0',30);
	strcpy(extract_phone_num,AT_CMGS_TEMP);
	
}

/**�����ı���Ϣ**/
int send_text_message(char *content)
{
	u8 ret;
	
	end_char[0] = 0x1A;//�����ַ�
	end_char[1] = '\0';
	
	
	ret = UART1_Send_AT_Command("AT+CMGF=1","OK",3,50);//����ΪTEXTģʽ
	if(ret == 0)
	{
		return AT_CMGF_ERROR;
	}
	
	
	ret = UART1_Send_AT_Command("AT+CSCS=\"GSM\"","OK",3,50);//�����ַ���ʽ
	if(ret == 0)
	{
		return AT_CSCS_ERROR;
	}
	
	ret = UART1_Send_AT_Command(extract_phone_num,">",3,50);//���������˵ĵ绰����
	if(ret == 0)
	{
        return AT_CMGS_ERROR;
	}    
	
	UART1_SendString(content); //�˺������Ͷ������� ���ӻس�����
	ret = UART1_Send_AT_Command(end_char,"OK",1,250);//���ͽ��������ȴ�����ok,�ȴ�5S��һ�Σ���Ϊ���ųɹ����͵�״̬ʱ��Ƚϳ�
	if(ret == 0)
	{
		return END_CHAR_ERROR;
	}
	
	return 1;
}


/**��ʱ��0�жϷ�����ں���,1ms�ж�һ��**/
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


/** UART1�жϺ���**/
void UART1_ISR (void) interrupt 4
{
	if (RI)
	{
		RI = 0;                           // ���RIλ
		Uart1_Buf[First_Int] = SBUF;  	  // �����յ����ַ����浽������
		First_Int++;                	  	// ����ָ������ƶ�
		if(First_Int >= Buf_Max)       	  // ���������,������ָ��ָ�򻺴���׵�ַ
		{
			First_Int = 0;
		}
		
		if(receive_mode == 1)
		{
			if(First_Int > 6)
			{
				find_string_flag = 1;
			}
	
		}
	}
	if (TI)
	{
		TI = 0;                          //���TIλ
	}
}


/**�������2��������**/
void CLR_Buf(void)
{
	u8 k;
	for(k=0;k<Buf_Max;k++)      //��������������
	{
		Uart1_Buf[k] = 0x00;
	}
    First_Int = 0;              //�����ַ�������ʼ�洢λ��
}


/**�жϻ������Ƿ���ָ�����ַ���**/
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

/**UART1 ���ʹ�������**/
void UART1_SendData(u8 dat)
{
	ES=0;			//�ش����ж�
	SBUF=dat;			
	while(TI!=1);	//�ȴ����ͳɹ�
	TI=0;			//��������жϱ�־
	ES=1;			//�������ж�
}

/**UART1 �����ַ���**/
void UART1_SendString(char *s)
{
	while(*s)//����ַ���������
	{
		UART1_SendData(*s++);//���͵�ǰ�ַ�
	}
}


/**��������**/
void UART1_Send_Command(char *s)
{
	CLR_Buf(); 
	while(*s)//����ַ���������
	{
		UART1_SendData(*s++);//���͵�ǰ�ַ�
	}
	UART1_SendLR();
}

/**����ATָ���
����: �������ݵ�ָ�롢ϣ���յ���Ӧ�𡢷��͵ȴ�ʱ��(��λ��interval_time*20 ms)
**/
u8 UART1_Send_AT_Command(u8 *b,u8 *a,u8 wait_time,u16 interval_time)         
{
	u8 i;

	CLR_Buf(); 							// ��ս��ջ���
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

/**��ʱ����**/
void delay_ms(u16 ms)
{
	 count_20ms = ms;
	 while(count_20ms);
}


/**GSM��ʼ��**/
void GSM_init()
{
	int i;
	Uart1Init(); 								// ���ڳ�ʼ��
	Timer0Init();								// TIM0��ʼ��
	EA = 1;      								// �������ж�
	
	for(i = 0;i < STABLE_TIMES;i++)	// �ȴ������ȶ�
	{
		delay_ms(50);
	}
	
	check_status();
	config_format();
	
	CLR_Buf();
	receive_mode = 1;
	
	LCD1602_write_com(0x80);
	LCD1602_write_word(" Initialized OK ");
	delay_ms(100);
}

/**ɨ�������Ϣ**/
void GSM_scan()
{
	int ret;
	if(find_string_flag == 1)	// ���յ��źű�־		
	{
		delay_ms(5);						// ��ʱһ�㣬�ô��ڰ����ݽ������
		if(Find("+CMTI"))				// ���յ��˶���
		{
			ret = read_message();	// �鿴���յ��Ķ���
			if(ret == 1)					// ���Ž��ճɹ�
			{
				if(Find("information"))		
				{
					LCD1602_write_com(0xc0);
					LCD1602_write_word(" Find  Successed");
					delay_ms(100);
					extract_phone_number();	// ��ȡ����
					
					sprintf(outInfo, "park1:%c\n park2:%c\n park3:%c\n park4:%c\n now Num:%d", 
									parkingFlag[0], parkingFlag[1], parkingFlag[2], parkingFlag[3], num);
					ret = send_text_message(outInfo);
					if(ret == 1)
					{
							LCD1602_write_com(0xc0);
							LCD1602_write_word(" Send  Successed");
							delay_ms(100);
					}
					else
					{
							LCD1602_write_com(0xc0);
							LCD1602_write_word(" Send  Failed   ");
							delay_ms(100);
					}
					CLR_Buf();
					find_string_flag = 0;
				}
				else
				{
					LCD1602_write_com(0xc0);
					LCD1602_write_word(" Find    Failed ");
					delay_ms(100);
					extract_phone_number();
					ret = send_text_message("This command is not supported!");
					if(ret == 1)
					{
							LCD1602_write_com(0xc0);
							LCD1602_write_word(" Send  Successed");
							delay_ms(100);
					}
					else
					{
							LCD1602_write_com(0xc0);
							LCD1602_write_word(" Send  Failed   ");
							delay_ms(100);
					}
					CLR_Buf();
					find_string_flag = 0;
				}
			}
		}			
	}	
}
