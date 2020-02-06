#include "main.h"
#include "LCD1602.h"
#include "HX711.h"
#include "EEPROM.H"

//�������
unsigned char KEY_NUM = 0;   //������Ű������µļ�ֵ
unsigned long HX711_Buffer = 0;  //�������HX711��ȡ����������
unsigned long Weight_Maopi = 0; //�������ëƤ����
long Weight_Shiwu = 0;          //�������ʵ������
long Max_Value = 1;             //��������������ֵ
unsigned int maxValueTable[6] = {0};
unsigned char state = 0;    //�����������״̬
unsigned char Blink_Speed = 0;
#define Blink_Speed_Max 6          //��ֵ���Ըı�����ָ����˸Ƶ��

#define GapValue 24.5

//����ֵ����λ��g
#define AlarmValue 200000			



//****************************************************
//������
//****************************************************
void main()
{
	Init_LCD1602();								//��ʼ��LCD1602

	LCD1602_write_com(0x80);			//����LCD1602ָ�� ��ʼ����ʾ
	LCD1602_write_word("Welcome to use! ");
	LCD1602_write_com(0x80 + 0x40);	
	LCD1602_write_word("Designed by LiuS");
	play_record(3);

	
	Get_Maopi();
	Get_Maopi();
	Delay_ms(2000);		 						//��ʱ2s
	Get_Maopi();
	Get_Maopi();									//��ëƤ��  ��β���������HX711�ȶ�
  LCD1602_write_com(0x01);    	//����
    
	//��ȡEEPROM�б���ı���ֵ
	maxValueTable[0] = byte_read(0x2000);
	maxValueTable[1] = byte_read(0x2001);
	maxValueTable[2] = byte_read(0x2002);
	maxValueTable[3] = byte_read(0x2003);
	maxValueTable[4] = byte_read(0x2004);
	maxValueTable[5] = byte_read(0x2005);
	Max_Value = (long)maxValueTable[0]*100000+(long)maxValueTable[1]*10000+(long)maxValueTable[2]*1000+(long)maxValueTable[3]*100+(long)maxValueTable[4]*10+(long)maxValueTable[5]; 
    
	while(1)
	{	
    Get_Weight();		
		receive();									// ������ʪ�ȴ������ź�
		//��ʾ��ǰ����
		LCD1602_write_com(0x80);
    LCD1602_write_word("Weight=");
    LCD1602_write_data(Weight_Shiwu/100000 + 0x30);
		LCD1602_write_data(Weight_Shiwu%100000/10000 + 0x30);
		LCD1602_write_data(Weight_Shiwu%10000/1000 + 0x30);
		LCD1602_write_data('.');
		LCD1602_write_data(Weight_Shiwu%1000/100 + 0x30);
 		LCD1602_write_data(Weight_Shiwu%100/10 + 0x30);
		LCD1602_write_data(Weight_Shiwu%10 + 0x30);	       
		LCD1602_write_word("Kg");

		KEY_NUM = Scan_Key();		// ����ɨ��
		if(KEY_NUM == 1)        // ����1�л�����״̬
		{
				state++;
				if(state == 7)
				{
						state = 0;
						SectorErase(0x2000);
						byte_write(0x2000,maxValueTable[0]);				//����EEPROM����
						byte_write(0x2001,maxValueTable[1]);
						byte_write(0x2002,maxValueTable[2]);				//����EEPROM����
						byte_write(0x2003,maxValueTable[3]);
						byte_write(0x2004,maxValueTable[4]);
						byte_write(0x2005,maxValueTable[5]);
						Max_Value = (long)maxValueTable[0]*100000+(long)maxValueTable[1]*10000+(long)maxValueTable[2]*1000+(long)maxValueTable[3]*100+(long)maxValueTable[4]*10+(long)maxValueTable[5];     //���㳬�ޱ�������ֵ				
				}
		}
		if(KEY_NUM == 2)        //������
		{
				if(state != 0)
				{
						maxValueTable[state-1]++;
						if(maxValueTable[state-1] > 9)
						{
								maxValueTable[state-1] = 0;
						}
				}
				
		}
		if(KEY_NUM == 3)        //������
		{
				if(state != 0)
				{
						maxValueTable[state-1]--;
						if(maxValueTable[state-1] < 0)
						{
								maxValueTable[state-1] = 9;
						}
				}
		}
		if(KEY_NUM == 4 && Weight_Shiwu != 0) 						// �˿��³���������
		{
			play_record(1);	
		}
          
		if(state != 0)					// ����ģʽ
		{
			Blink_Speed++;
			if(Blink_Speed == Blink_Speed_Max)
			{
					Blink_Speed = 0;
			}
		}
		switch(state)
		{
				case 0:
						LCD1602_write_com(0x80+0x40);
						LCD1602_write_word("M");
						LCD1602_write_data('0' + maxValueTable[0]);
						LCD1602_write_data('0' + maxValueTable[1]);             
						LCD1602_write_data('0' + maxValueTable[2]);
						LCD1602_write_data('.');
						LCD1602_write_data('0' + maxValueTable[3]);
						LCD1602_write_data('0' + maxValueTable[4]);
						LCD1602_write_data('0' + maxValueTable[5]);
						display_tem();
						break;
				case 1:
						LCD1602_write_com(0x80+0x40);
						LCD1602_write_word("M");
						if(Blink_Speed < Blink_Speed_Max/2)
						{
								LCD1602_write_data('0' + maxValueTable[0]);
						}
						else
						{
								LCD1602_write_data(' ');
						}
						LCD1602_write_data('0' + maxValueTable[1]);             
						LCD1602_write_data('0' + maxValueTable[2]);
						LCD1602_write_data('.');
						LCD1602_write_data('0' + maxValueTable[3]);
						LCD1602_write_data('0' + maxValueTable[4]);
						LCD1602_write_data('0' + maxValueTable[5]);
						display_tem();
						break;
				case 2:
						LCD1602_write_com(0x80+0x40);
						LCD1602_write_word("M");               
						LCD1602_write_data('0' + maxValueTable[0]);
						if(Blink_Speed < Blink_Speed_Max/2)
						{
							LCD1602_write_data('0' + maxValueTable[1]);             
						}
						else
						{
								LCD1602_write_data(' ');
						}
						LCD1602_write_data('0' + maxValueTable[2]);
						LCD1602_write_data('.');               
						LCD1602_write_data('0' + maxValueTable[3]);
						LCD1602_write_data('0' + maxValueTable[4]);
						LCD1602_write_data('0' + maxValueTable[5]);
						display_tem();
						break;
				case 3:
						LCD1602_write_com(0x80+0x40);
						LCD1602_write_word("M");
						LCD1602_write_data('0' + maxValueTable[0]);
						LCD1602_write_data('0' + maxValueTable[1]);              
						if(Blink_Speed < Blink_Speed_Max/2)
						{
								LCD1602_write_data('0' + maxValueTable[2]);
						}
						else
						{
								LCD1602_write_data(' ');
						}
						LCD1602_write_data('.');
						LCD1602_write_data('0' + maxValueTable[3]);
						LCD1602_write_data('0' + maxValueTable[4]);
						LCD1602_write_data('0' + maxValueTable[5]);
						display_tem();
						break;
				case 4:
						LCD1602_write_com(0x80+0x40);
						LCD1602_write_word("M");
						LCD1602_write_data('0' + maxValueTable[0]);
						LCD1602_write_data('0' + maxValueTable[1]); 
						LCD1602_write_data('0' + maxValueTable[2]);
						LCD1602_write_data('.');
						if(Blink_Speed < Blink_Speed_Max/2)
						{
								LCD1602_write_data('0' + maxValueTable[3]);
						}
						else
						{
								LCD1602_write_data(' ');
						}                
						LCD1602_write_data('0' + maxValueTable[4]);
						LCD1602_write_data('0' + maxValueTable[5]);
						display_tem();
						break;
				case 5:
						LCD1602_write_com(0x80+0x40);
						LCD1602_write_word("M");
						LCD1602_write_data('0' + maxValueTable[0]);
						LCD1602_write_data('0' + maxValueTable[1]); 
						LCD1602_write_data('0' + maxValueTable[2]);
						LCD1602_write_data('.');
						LCD1602_write_data('0' + maxValueTable[3]);
						if(Blink_Speed < Blink_Speed_Max/2)
						{
								LCD1602_write_data('0' + maxValueTable[4]);
						}
						else
						{
								LCD1602_write_data(' ');
						} 
						LCD1602_write_data('0' + maxValueTable[5]);
						display_tem();
						break;
				case 6:
						LCD1602_write_com(0x80+0x40);
						LCD1602_write_word("M");
						LCD1602_write_data('0' + maxValueTable[0]);
						LCD1602_write_data('0' + maxValueTable[1]); 
						LCD1602_write_data('0' + maxValueTable[2]);
						LCD1602_write_data('.');
						LCD1602_write_data('0' + maxValueTable[3]);
						LCD1602_write_data('0' + maxValueTable[4]);
						if(Blink_Speed < Blink_Speed_Max/2)
						{
								LCD1602_write_data('0' + maxValueTable[5]);
						}
						else
						{
								LCD1602_write_data(' ');
						}                
						display_tem();
						break;
				default:
						break;
				
		}      
    //���ޱ���
		if(Weight_Shiwu >= Max_Value || Weight_Shiwu >= AlarmValue)	        //�����������ֵ���ߴ����������������ֵ����	
		{
			play_record(2);
		}
		else
		{
//			SendString("Noting~\n");
		}
	}
}

//****************************************************
//����
//****************************************************
void Get_Weight()
{
	Weight_Shiwu = HX711_Read();
	Weight_Shiwu = Weight_Shiwu - Weight_Maopi;		//��ȡ����
	if(Weight_Shiwu >= 0)			
	{	
		Weight_Shiwu = (unsigned long)((float)Weight_Shiwu/GapValue); 	//����ʵ���ʵ������
	}
	else
	{
		Weight_Shiwu = 0;
	}
	
}

//****************************************************
//��ȡëƤ����
//****************************************************
void Get_Maopi()
{
	Weight_Maopi = HX711_Read();	
} 


//****************************************************
//MS��ʱ����(12M�����²���)
//****************************************************
void Delay_ms(unsigned int n)
{
	unsigned int  i,j;
	for(i=0;i<n;i++)
		for(j=0;j<123;j++);
}



//****************************************************
//����ɨ�����
//****************************************************
unsigned char Scan_Key()
{	
	if( KEY1 == 0 )						//����ɨ��
	{
		Delay_ms(10);					//��ʱȥ��
		if( KEY1 == 0 )
		{
			while(KEY1 == 0);			//�ȴ�����
			return 1;
		}
	}
	if( KEY2 == 0 )						//����ɨ��
	{
		Delay_ms(10);					//��ʱȥ��
		if( KEY2 == 0 )
		{
			while(KEY2 == 0);			//�ȴ�����
			return 2;
		}
	}
	if( KEY3 == 0 )						//����ɨ��
	{
		Delay_ms(10);					//��ʱȥ��
		if( KEY3 == 0 )
		{
			while(KEY3 == 0);			//�ȴ�����
			return 3;
		}
	}
	if( KEY4 == 0 )						//����ɨ��
	{
		Delay_ms(10);					//��ʱȥ��
		if( KEY4 == 0 )
		{
			while(KEY4 == 0);			//�ȴ�����
			return 4;
		}
	}
	
  return 0;
}

void play_record(uchar index)
{
	switch(index)
	{
		case 1:
			play_01 = 0;
			break;
		case 2:
			play_02 = 0;
			break;
		case 3:
			play_03 = 0;
			break;
	}
	Delay_ms(1000);
	switch(index)
	{
		case 1:
			play_01 = 1;
			break;
		case 2:
			play_02 = 1;
			break;
		case 3:
			play_03 = 1;
			break;
	}
}

void display_tem()							//��ʾ�¶Ⱥ�ʪ��
{
	LCD1602_write_word("R:");
	LCD1602_write_data(RH/10+0x30); 
	LCD1602_write_data(RH%10+0x30); 	
	LCD1602_write_word("T:");
	LCD1602_write_data(TH/10+0x30); 
	LCD1602_write_data(TH%10+0x30); 
}

void start() 										// dht11 ��ʼ�ź�
{ 
  io=1; 
  delay1(); 
  io=0; 
  delay(25);
  io=1;   
  delay1();
  delay1(); 
  delay1(); 
} 

 

uchar receive_byte()						// ����һ���ֽ�
{ 
  uchar i,temp; 
  for(i=0;i<8;i++) 
  { 
		while(!io);
		delay1(); 
		delay1(); 
		delay1(); 
		temp=0;
		if(io==1) 
		 temp=1;  
		while(io);
		data_byte<<=1;
		data_byte|=temp; 						// ����8bit����
  } 

  return data_byte; 
} 

    

void receive()									// ��������
{ 
  uchar T_H,T_L,R_H,R_L,check,num_check,i; 
  start();
  io=1;   
  if(!io) 
  {  
		while(!io);
		while(io);
		R_H=receive_byte();						// ʪ�ȸ�λ
		R_L=receive_byte();						// ʪ�ȵ�λ
		T_H=receive_byte();						// �¶ȸ�λ
		T_L=receive_byte();						// �¶ȵ�λ
		check=receive_byte(); 				// У��λ
		io=0; 
		for(i=0;i<7;i++)							// ��ʱ50��m
			delay1(); 
		io=1;
		num_check=R_H+R_L+T_H+T_L; 
		if(num_check==check) 					// �ж��Ƿ���У��λ��ͬ
		{ 
		 RH=R_H; 
		 RL=R_L; 
		 TH=T_H; 
		 TL=T_L; 
		 check=num_check; 
		} 
  } 
} 

 void delay(uchar ms)							// ��ʱ
{ 
  uchar i; 
  while(ms--)                        
	for(i=0;i<100;i++); 
} 

void delay1()										//��ʱ8��m
{ 
  uchar i; 
  for(i=0;i<1;i++); 
} 