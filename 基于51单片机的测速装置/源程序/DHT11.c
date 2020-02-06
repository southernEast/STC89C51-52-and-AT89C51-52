#include "DHT11.h"
#include "LCD1602.h"

uchar RH,RL,TH,TL;
uchar data_byte;

void DHT11_start() 										// dht11 ��ʼ�ź�
{ 
  io=1; 
  delay1(); 
  io=0; 
  delay2(25);
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
  DHT11_start();
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

 void delay2(uchar ms)							// ��ʱ
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

void ShowTemAndHum()
{
	receive();
	write_1602com(0x80 + 0x40);
	write_1602string("R:");
	write_1602dat(RH/10+0x30); 
	write_1602dat(RH%10+0x30);
	write_1602string("%   ");
	write_1602string("T:");
	write_1602dat(TH/10+0x30); 
	write_1602dat(TH%10+0x30); 
	write_1602string("C   ");

}