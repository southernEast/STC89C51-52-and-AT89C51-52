#include<reg52.h>	  //����ͷ�ļ�

#define uchar unsigned char
#define uint unsigned int	  //�궨��

#include "Data.h"			  //������С���жϺ���
#include "DS1302.h"			  //����1302����
#include "AT24C02.h"		  //����24c02����
#include "LCD1602.h"
#include "DHT11.h"

extern uchar code tab1[];
extern uchar code tab2[];	
extern uchar code tab3[];	
extern uchar code tab4[];	
extern uchar code tab5[];


sbit COUNT_IN=P3^2;			  //�����������ź�����
sbit voice = P2^3;

//���̶���
sbit K1=P3^4;	//����ʱ��
sbit K3=P3^6;	//������
sbit K2=P3^5;	//�Ӱ���
sbit K4=P3^7;	//���ð뾶��ȫ����
sbit BEEP=P2^0;	//������

uint count;					  //������������������
unsigned long Velocity,Mileage;	 //�ٶȱ�������̱���

			   			
uchar Mode=0;							//����ʱ���λ�ñ�����ֵΪ1ʱ�����ֵ꣬Ϊ2ʱ�����£��Դ�����...
uchar bike_set=0;						//���ð뾶����ȫ�ٶ�ʱ��λ�ñ���
uchar a;
char RADIUS,SAFE_SPEED;					//�뾶�Ͱ�ȫ�ٶȱ���
bit LED_SEC;							//�����˸����
uchar before_sec;						//��һ�룬������������˸

void READS();			//��ȡAT24C02������
void SETS();			//д��24c02����
void delay(uint x)		//��ʱ��������Լ��ʱ1ms������ȷ
{
	uint i,j;			//������ʱ����
	for(i=0;i<x;i++)	//ִ��x��
	for(j=0;j<110;j++);	//ִ��110��
}

void init()				//��ʱ����ʼ������
{
	IT0=1;	//INT0�����䴥��	
	TMOD=0x01;//��ʱ�������ڷ�ʽ1
	TH0=0x3c;	  //50ms
	TL0=0xb0;
	EA=1; //CPU���ж�������
	ET0=1;//����ʱ�ж�
	EX0=1;//���ⲿINTO�ж�
  TR0=1;//������ʱ
}


void display()
{
	//			1km/h=100m/0.1h	  	 360s
	//			10km/h=100m/0.01h 	 36s
	//			100km/h=100m/0.001h  3.6s
	if(Mode==0&&bike_set==0)			   //��������״̬ʱ
	{
		//��ʱ��
			Ds1302_Read_Time();			   //��ȡ1302�ڲ���ʱ������
			//��ʾʱ��
			write_1602com(0x80);				   //��һ�е�һ����ʾ��һ������������ʾ
			write_1602dat(0x30+time_buf1[1]/10);   //��ʾ���ʮλ
			write_1602dat(0x30+time_buf1[1]%10);   //��ʾ��ĸ�λ
			write_1602com(0x80+3);				   //��һ�е����У��ڶ�����ʾ����������Һ���Ǵӵ�0�п�ʼ
			write_1602dat(0x30+time_buf1[2]/10);   //��ʾ�µ�ʮλ
			write_1602dat(0x30+time_buf1[2]%10);   //��ʾ�µĸ�λ
			write_1602com(0x80+6);				   //��ͬ
			write_1602dat(0x30+time_buf1[3]/10);
			write_1602dat(0x30+time_buf1[3]%10);
			write_1602com(0x80+9);
			write_1602dat(0x30+time_buf1[4]/10);
			write_1602dat(0x30+time_buf1[4]%10);
			write_1602com(0x80+12);
			write_1602dat(0x30+time_buf1[5]/10);
			write_1602dat(0x30+time_buf1[5]%10);
			write_1602com(0x80+15);					//��һ�е�ʮ���У���ʾ��������
			write_1602dat(time_buf1[7]-1);			//��ʾ����

			if(before_sec!=time_buf1[6])			//�����˸
			{
				before_sec=time_buf1[6];			//����ĸ�λ
				write_1602com(0x80+11);				//ѡ�е�һ��11��
				write_1602dat(':');					//��ʾ��
				LED_SEC=1;							//��Ʊ�����1����ʱ����ʼ��ʱ500ms��Ϩ��
			}
			if(LED_SEC==0)							//��ʱ����ʱʱ�䵽
			{
				write_1602com(0x80+11);				//��һ�е�11��
				write_1602dat(' ');					//����ʾ
			}

			write_1602com(0x80+0x40);				//�ڶ���
			if(Mileage/1000000==0)					//��̵����λ�Ƿ�Ϊ0
			write_1602dat(' ');						//Ϊ0�Ͳ���ʾ
			else									//��Ϊ0
			write_1602dat(0x30+Mileage/1000000);//����+0x30�õ������ֵ�LCD1602��ʾ��
			if(Mileage%1000000/100000==0)			//ͬ��
			write_1602dat(' ');
			else	
			write_1602dat(0x30+Mileage%1000000/100000);//����+0x30�õ������ֵ�LCD1602��ʾ��
			write_1602dat(0x30+Mileage%1000000%100000/10000);//����+0x30�õ������ֵ�LCD1602��ʾ�� 
			write_1602com(0x80+0x40+4);				 //�ڶ��е�4��
			write_1602dat(0x30+Mileage%1000000%100000%10000/1000);//����+30�õ������ֵ�LCD1602��ʾ��
			write_1602dat(0x30+Mileage%1000000%100000%10000%1000/100);//����+30�õ������ֵ�LCD1602��ʾ��
			write_1602dat(0x30+Mileage%1000000%100000%10000%1000%100/10);//����+30�õ������ֵ�LCD1602��ʾ��
			SETS();							//ÿ��ˢ��������ݺ󶼱���һ��������ݣ�����洢
	
			write_1602com(0x80+0x40+9);							//�ڶ��е�9��
			if(Velocity/100==0)
			write_1602dat(' ');
			else
			write_1602dat(0x30+Velocity/100);
			if(Velocity/10==0)
			write_1602dat(' ');
			else
			write_1602dat(0x30+Velocity%100/10);						//��ʾ�ٶ�����
			write_1602dat(0x30+Velocity%100%10);//����+30�õ������ֵ�LCD1602��ʾ��
	}
	else if(Mode!=0)			 //������������ʱ��ģʽʱ
	{
		switch(Mode)			 //�ж�Mode����
		{
			case 1:												   //ֵΪ1ʱ
				write_1602com(0x80+0x40);//��ʾ�̶�����д��λ��	   //�ڶ���
				for(a=0;a<16;a++)
				{
					write_1602dat(tab5[a]);//д��ʾ�̶�����
				}
				write_1602com(0x80+0x40+14);				//�ڶ��е�14��
				write_1602dat(0x30+time_buf1[6]/10);		//��ʾ��
				write_1602dat(0x30+time_buf1[6]%10);	
				write_1602com(0x0F);	 //����˸
				write_1602com(0x80+1);	 //��һ�е�һ����˸�����λ����˸
				break;						  
			case 2:
				write_1602com(0x80+4);	 //�µ�λ����˸
				break;
			case 3:
				write_1602com(0x80+7);	 //�յ�λ����˸
				break;
			case 4:
				write_1602com(0x80+10);	 //Сʱ��λ����˸
				break;
			case 5:
				write_1602com(0x80+13);	 //���ӵ�λ����˸
				break;
			case 6:
				write_1602com(0x80+0x40+15); //���λ����˸
				break;
			case 7:
				write_1602com(0x80+15);		 //���ڵ�λ����˸
				break;
			case 8:
				write_1602com(0x0c); //�������ر���˸
				write_1602com(0x80);//��ʾ�̶����Ŵӵ�һ�е�1��λ��֮��ʼ��ʾ
				for(a=0;a<16;a++)
				{
					write_1602dat(tab1[a]);//��Һ����д�̶����Ų���
				}
				write_1602com(0x80+0x40);//��ʾ�̶�����д��λ�ã��ӵ�2��λ�ú�ʼ��ʾ
				for(a=0;a<16;a++)
				{
					write_1602dat(tab2[a]);//д��ʾ�̶�����
				}
				break;
		}
	}
	else if(bike_set!=0)				   //���ð뾶�Ͱ�ȫ�ٶ�
	{
		switch(bike_set)								 //�ж�bike_set
		{
			case 1:	
				write_1602com(0x80);//��ʾ�̶�����д��λ��
				for(a=0;a<16;a++)
				{
					write_1602dat(tab3[a]);//д��ʾ�̶�����
				}
				write_1602com(0x80+0x40);//��ʾ�̶�����д��λ��
				for(a=0;a<16;a++)
				{
					write_1602dat(tab4[a]);//д��ʾ�̶�����
				}
				write_1602com(0x80+12);
				write_1602dat(0x30+RADIUS/10);		   //���ְ뾶
				write_1602dat(0x30+RADIUS%10);
				write_1602com(0x80+0x40+10);
				write_1602dat(0x30+SAFE_SPEED/10);		   //��ȫ�ٶ�
				write_1602dat(0x30+SAFE_SPEED%10);	
				write_1602com(0x0F);	 //����˸
				write_1602com(0x80+13);
				break;						  
			case 2:
				write_1602com(0x80+0x40+11);
				break;
			case 3:
				write_1602com(0x0c);
				write_1602com(0x80);//��ʾ�̶����Ŵӵ�һ�е�1��λ��֮��ʼ��ʾ
				for(a=0;a<16;a++)
				{
					write_1602dat(tab1[a]);//��Һ����д�̶����Ų���
				}
				write_1602com(0x80+0x40);//��ʾ�̶�����д��λ�ã��ӵ�2��λ�ú�ʼ��ʾ
				for(a=0;a<16;a++)
				{
					write_1602dat(tab2[a]);//д��ʾ�̶�����
				}
				break;
		}
	}	
}

void KEY()					//��������
{	
	if(bike_set==0&&K1==0)	//�������ð뾶ģʽʱ�����԰���K1
	{
		delay(20);			//��ʱȥ��
		if(bike_set==0&&K1==0)//�ٴ��жϰ����Ƿ���
		{
			BEEP=0;			  //��������
			delay(50);		  //��ʱ
			BEEP=1;			  //�رշ��������ﵽ�����������ã���ͬ��ע����
			Mode++;			  //Mode++��
			display();		  //��ʾ����
			if(Mode>=8)		  //�������
			{
				Mode=0;			   //��������
				Ds1302_Write_Time();//ʱ����������д��ʱ��оƬ
			}
		}
		while(bike_set==0&&K1==0); //�����ͷţ�ֻҪ����û���ɿ����˴�һֱ��������������
	}
	if(K4==0&&Mode==0)			   //��������ʱ������ʱ�ſ��԰���K4
	{
		delay(20);				   //��ʱȥ������ͬ
		if(K4==0&&Mode==0)		   //�ٴ��жϰ����Ƿ���
		{
			BEEP=0;
			delay(50);
			BEEP=1;				   //������
			bike_set++;			   //������
			display();			   //������ʾ����
			if(bike_set>=3)		   //�������
			{
				bike_set=0;		   //��������
				SETS();			   //�����úõ����ݱ��浽AT24C02�У����籣��
			}
		}
		while(Mode==0&&K4==0);	   //�����ͷ�
	}

	//+
	if(K2==0&&(Mode!=0||bike_set!=0))  //������״̬ʱ������K2
	{
		delay(20);
		//��ʱ
		if(K2==0&&(Mode!=0||bike_set!=0))
		{
			BEEP=0;
			delay(50);
			BEEP=1;	
			switch(Mode)								//����ʱ��ʱ
			{
				case 1:									//������ʱ
					time_buf1[1]++;						//���
					if(time_buf1[1]>=100)				//�����ӵ�99����100������
						time_buf1[1]=0;
					write_1602com(0x80);
					write_1602dat(0x30+time_buf1[1]/10);//��ʾ������
					write_1602dat(0x30+time_buf1[1]%10);
					write_1602com(0x80+1);				//���ص����λ����˸
					break;								//����
				case 2:									//�����·�
					time_buf1[2]++;						//�·����ݼ�
					if(time_buf1[2]>=13)				//�����12
						time_buf1[2]=1;					//��ͬ��ע����
					write_1602com(0x80+3);
					write_1602dat(0x30+time_buf1[2]/10);
					write_1602dat(0x30+time_buf1[2]%10);
					write_1602com(0x80+4);
					break;
				case 3:
					time_buf1[3]++;								   //������
					if(time_buf1[3]>=YDay(time_buf1[1],time_buf1[2])+1)
						time_buf1[3]=1;
					write_1602com(0x80+6);
					write_1602dat(0x30+time_buf1[3]/10);
					write_1602dat(0x30+time_buf1[3]%10);
					write_1602com(0x80+7);
					break;
				case 4:
					time_buf1[4]++;								//ʱ
					if(time_buf1[4]>=24)
						time_buf1[4]=0;
					write_1602com(0x80+9);
					write_1602dat(0x30+time_buf1[4]/10);
					write_1602dat(0x30+time_buf1[4]%10);
					write_1602com(0x80+10);
					break;
				case 5:
					time_buf1[5]++;								 //��
					if(time_buf1[5]>=60)
						time_buf1[5]=0;
					write_1602com(0x80+12);
					write_1602dat(0x30+time_buf1[5]/10);
					write_1602dat(0x30+time_buf1[5]%10);
					write_1602com(0x80+13);
					break;
				case 6:
					time_buf1[6]++;
					if(time_buf1[6]>=60)					 //��
						time_buf1[6]=0;
					write_1602com(0x80+0x40+14);
					write_1602dat(0x30+time_buf1[6]/10);
					write_1602dat(0x30+time_buf1[6]%10);
					write_1602com(0x80+0x40+15);
					break;
				case 7:
					time_buf1[7]++;
					if(time_buf1[7]>=8)						  //����
						time_buf1[7]=1;
					write_1602com(0x80+15);
					write_1602dat(time_buf1[7]-1);
					write_1602com(0x80+15);
					break;
			}
			switch(bike_set)							 //���ð뾶��ȫ�ٶ�
			{
				case 1:
					RADIUS++;							 //�뾶����
					if(RADIUS>=71)
					RADIUS=0;
					write_1602com(0x80+12);
					write_1602dat(0x30+RADIUS/10);
					write_1602dat(0x30+RADIUS%10);
					write_1602com(0x80+13);
					break;
				case 2:
					SAFE_SPEED++;
					if(SAFE_SPEED>=100)					   //��ȫ�ٶ�
					SAFE_SPEED=0;
					write_1602com(0x80+0x40+10);
					write_1602dat(0x30+SAFE_SPEED/10);
					write_1602dat(0x30+SAFE_SPEED%10);
					write_1602com(0x80+0x40+11);
					break;
			}
		}
		while(K2==0);							  //�����ͷ�
	}

	//-
	if(K3==0&&(Mode!=0||bike_set!=0))			 //����ΪK3����������ע�Ͳο��Ӱ�������
	{
		delay(20);
		//��ʱ
		if(K3==0&&(Mode!=0||bike_set!=0))
		{
			BEEP=0;
			delay(50);
			BEEP=1;
			switch(Mode)
			{
				case 1:
					time_buf1[1]--;
					if(time_buf1[1]<0)
						time_buf1[1]=99;
					write_1602com(0x80);
					write_1602dat(0x30+time_buf1[1]/10);
					write_1602dat(0x30+time_buf1[1]%10);
					write_1602com(0x80+1);
					break;
				case 2:
					time_buf1[2]--;
					if(time_buf1[2]<=0)
						time_buf1[2]=12;
					write_1602com(0x80+3);
					write_1602dat(0x30+time_buf1[2]/10);
					write_1602dat(0x30+time_buf1[2]%10);
					write_1602com(0x80+4);
					break;
				case 3:
					time_buf1[3]--;
					if(time_buf1[3]<=0)
						time_buf1[3]=YDay(time_buf1[1],time_buf1[2]);
					write_1602com(0x80+6);
					write_1602dat(0x30+time_buf1[3]/10);
					write_1602dat(0x30+time_buf1[3]%10);
					write_1602com(0x80+7);
					break;
				case 4:
					time_buf1[4]--;
					if(time_buf1[4]<0)
						time_buf1[4]=23;
					write_1602com(0x80+9);
					write_1602dat(0x30+time_buf1[4]/10);
					write_1602dat(0x30+time_buf1[4]%10);
					write_1602com(0x80+10);
					break;
				case 5:
					time_buf1[5]--;
					if(time_buf1[5]<0)
						time_buf1[5]=59;
					write_1602com(0x80+12);
					write_1602dat(0x30+time_buf1[5]/10);
					write_1602dat(0x30+time_buf1[5]%10);
					write_1602com(0x80+13);
					break;
				case 6:
					time_buf1[6]--;
					if(time_buf1[6]<0)
						time_buf1[6]=59;
					write_1602com(0x80+0x40+14);
					write_1602dat(0x30+time_buf1[6]/10);
					write_1602dat(0x30+time_buf1[6]%10);
					write_1602com(0x80+0x40+15);
					break;
				case 7:
					time_buf1[7]--;
					if(time_buf1[7]<1)
						time_buf1[7]=7;
					write_1602com(0x80+15);
					write_1602dat(time_buf1[7]-1);
					write_1602com(0x80+15);
					break;
			}
			switch(bike_set)
			{
				case 1:
					RADIUS--;
					if(RADIUS<0)
					RADIUS=70;
					write_1602com(0x80+12);
					write_1602dat(0x30+RADIUS/10);
					write_1602dat(0x30+RADIUS%10);
					write_1602com(0x80+13);
					break;
				case 2:
					SAFE_SPEED--;
					if(SAFE_SPEED<0)
					SAFE_SPEED=99;
					write_1602com(0x80+0x40+10);
					write_1602dat(0x30+SAFE_SPEED/10);
					write_1602dat(0x30+SAFE_SPEED%10);
					write_1602com(0x80+0x40+11);
					break;
			}
		}
		while(K3==0);
	}	
	if(K2==0&&K3==0&&Mode==0&bike_set==0)	  //����������״̬ʱ����������ͬʱ����
	{
		BEEP=0;
		delay(100);
		BEEP=1;
		delay(100);
		BEEP=0;
		delay(100);
		BEEP=1;						   //��������������
		delay(100);
		Mileage=0;						//�����������
		SETS();							//�����������д��24c02
		while(K2==0&&K3==0);			//�����ͷ�
	}
	if(K2 == 0 && bike_set == 0 && Mode == 0){
		ShowTemAndHum();
		while(K2 == 0 && bike_set == 0 && Mode == 0);
		write_1602com(0x80);//��ʾ�̶����Ŵӵ�һ�е�1��λ��֮��ʼ��ʾ
		write_1602string(tab1);
		write_1602com(0x80+0x40);//��ʾ�̶�����д��λ�ã��ӵ�2��λ�ú�ʼ��ʾ
		write_1602string(tab2);
	}
}
void BJ_SAFE()						   //��ȫ�ٶȱ�������
{
	if(Velocity>SAFE_SPEED)			   //��ǰ�ٶȳ�����ȫ�ٶ�
	{
		BEEP=0;						  //��������
		voice = 0;
	}
	else							  //û����
	{
		BEEP=1;						 //�رշ�����
		voice = 1;
	}
}

void main()							 //������
{
	//��ʼ��
	Ds1302_Init();			 //ʱ��оƬ��ʼ��
	lcd_init();				 //Һ����ʼ��
	initeeprom();			 //24c02��ʼ��
	READS();					//��ȡ��ʼ����
	init();						//��ʱ����ʼ��
	before_sec=time_buf1[6];	//��ȡ��������
	while(1)					//����ѭ��
	{
		if(Mode==0&&bike_set==0)//��������״̬ʱ
		{
			display();		   //������ʾ����
			BJ_SAFE();			//�ж��Ƿ���
		}
		KEY();				   //ɨ�谴��
	}
}

void EXINT0() interrupt 0		 //�ⲿ�ж�0
{
	count++;					 //�������źţ��ñ�����1
}

void time0() interrupt 1		//��ʱ���ж�0
{
	uchar m,n;
	TH0=0x3c;
	TL0=0xb0;	 //50ms
	m++;
	if(LED_SEC==1)				//��ƿ���
	{
		n++;
		if(n>=10)				//��500msʱ
		{
			n=0;
			LED_SEC=0;			//Ϩ�����
		}
	}
	
	if(m>=10)					 //500msʱ����һ������ٶ���Ϣ
	{
		m=0;
		Mileage=Mileage+10*(Velocity/3.6)/2;		 //���m=���+�ٶ�km/h/3.6/2
		Velocity=count *2*3.14*RADIUS /100000*2*3600  /40;//��500ms�ľ��뾭������õ�km/h�����ٶ�/100��������ʾ
		count=0;									  //����
	}
}


//����ֵ
void READS()
{
	uchar Mileage_H,Mileage_M,Mileage_L;   //���彫��̲𿪵õ�����������
	delay(10);
	RADIUS=read_add(0x01);				   //��ȡ�뾶��Ϣ
	delay(10);							   //��ʱ�������Ƕ�д���ݸ��ȶ�
	SAFE_SPEED=read_add(0x02);			   //��ȡ��ȫ�ٶ���Ϣ

	delay(10);
	Mileage_H=read_add(0x03);			   //��ȡ������ݵĸ�λ
	delay(10);
	Mileage_M=read_add(0x04);			   //��ȡ������ݵ��м�λ
	delay(10);
	Mileage_L=read_add(0x05);			   //��ȡ������ݵĵ�λ

	Mileage=Mileage_H*100000+Mileage_M*1000+Mileage_L*10;//������������ϳ��������
}


//д��ֵ
void SETS()
{
	delay(10);
	write_add(0x01,RADIUS);				//���뾶����д��24c02
	delay(10);
	write_add(0x02,SAFE_SPEED);			//����ȫ�ٶ���Ϣд��

/*	Mileage_H=Mileage/10000;			 //123.4560
	Mileage_M=Mileage%10000/100;
	Mileage_L=Mileage%10000%100; */
	delay(10);
	write_add(0x03,Mileage/100000);		   //�������д��
	delay(10);
	write_add(0x04,Mileage%100000/1000);
	delay(10);
	write_add(0x05,Mileage%100000%1000/10);
}


