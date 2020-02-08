#include "LCD1602.h"

//*********LCD1602��ʼ��**********************
void InitLcd()				
{			
	WriteCommandLCM(0x38,1);	
	WriteCommandLCM(0x08,1);	
	WriteCommandLCM(0x01,1);
	WriteCommandLCM(0x06,1);	
	WriteCommandLCM(0x0c,1);
}

//**********���æ�ź�************************
void WaitForEnable(void)	
{					
	DataPort=0xff;		
	LCM_RS=0;LCM_RW=1;_nop_();
	LCM_EN=1;_nop_();_nop_();
	while(DataPort&0x80);	
	LCM_EN=0;				
}
					
//**********д������LCD***********************
void WriteCommandLCM(uchar CMD,uchar Attribc)
{					
	if(Attribc)WaitForEnable();	
	LCM_RS=0;LCM_RW=0;_nop_();
	DataPort=CMD;_nop_();	
	LCM_EN=1;_nop_();_nop_();LCM_EN=0;
}	
				
//**********д������LCD************************
void WriteDataLCM(uchar dataW)
{					
	WaitForEnable();		
	LCM_RS=1;LCM_RW=0;_nop_();
	DataPort=dataW;_nop_();	
	LCM_EN=1;_nop_();_nop_();LCM_EN=0;
}
					
//*********дһ���ַ����ݵ�ָ����Ŀ��***********
void DisplayOneChar(uchar X,uchar Y,uchar DData)
{						
	Y&=1;						
	X&=15;						
	if(Y)X|=0x40;					
	X|=0x80;			
	WriteCommandLCM(X,0);		
	WriteDataLCM(DData);		
}

void display_1602(uchar *DData,X,Y)//��ʾ����  
{ 
	uchar ListLength=0;
	Y&=0x01;
	X&=0x0f;
	while(X<16)
	{
	    DisplayOneChar(X,Y,DData[ListLength]);
	    ListLength++;
	    X++;
	}
}

