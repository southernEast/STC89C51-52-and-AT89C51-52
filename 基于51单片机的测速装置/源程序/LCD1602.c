#include "LCD1602.h"

//�Զ����ַ�
uchar code num[]={
						0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x00,//1	  //�˴��ַ��Ƿ��ε����֣����ں������������ֿ���������ʾ����
						0x1f,0x01,0x01,0x1f,0x10,0x10,0x1f,0x00,//2
						0x1f,0x01,0x01,0x1f,0x01,0x01,0x1f,0x00,//3	 
						0x11,0x11,0x11,0x1f,0x01,0x01,0x01,0x00,//4
						0x1f,0x10,0x10,0x1f,0x01,0x01,0x1f,0x00,//5	 
						0x1f,0x10,0x10,0x1f,0x11,0x11,0x1f,0x00,//6
						0x1f,0x01,0x01,0x01,0x01,0x01,0x01,0x00,//7 	
};

uchar code tab1[]={"  /  /     :    "}; //14/09/10 16:34 3   	
uchar code tab2[]={"  0.000km 00km/h"};	//000.000km 00km/h
uchar code tab3[]={"Wheel Radius  cm"};	//��ʾ���ְ뾶���ý���
uchar code tab4[]={"Safe Speed  km/h"};	//��ʾ��ȫ�ٶ����ý���
uchar code tab5[]={"Sec :           "};	//ʱ������ʱ�������ʾ����


void LCD1602_delay(uint x)		//��ʱ��������Լ��ʱ1ms������ȷ
{
	uint i,j;			//������ʱ����
	for(i=0;i<x;i++)	//ִ��x��
	for(j=0;j<110;j++);	//ִ��110��
}

void write_1602com(uchar com)//****Һ��д��ָ���****
{
	rs=0;//����/ָ��ѡ����Ϊָ��
	P0=com;//��������
	LCD1602_delay(1);
	en=1;//����ʹ�ܶˣ�Ϊ������Ч���½�����׼��
	LCD1602_delay(1);
	en=0;//en�ɸ߱�ͣ������½��أ�Һ��ִ������

}


void write_1602dat(uchar dat)//***Һ��д�����ݺ���****
{
	rs=1;//����/ָ��ѡ����Ϊ����
	P0=dat;//��������
	LCD1602_delay(1);
	en=1; //en�øߵ�ƽ��Ϊ�����½�����׼��
	LCD1602_delay(1);
	en=0; //en�ɸ߱�ͣ������½��أ�Һ��ִ������
}

void write_1602string(uchar *str)
{
	uchar *now = str;
	while(*now){
		write_1602dat(*now);
		now++;
	}
}

//�Զ����ַ���
void Lcd_ram()      
{ 
	uint i,j,k=0,temp=0x40; 
	for(i=0;i<7;i++)
	{
	   for(j=0;j<8;j++)
	   {
	    write_1602com(temp+j);
	    write_1602dat(num[k]);
	    k++;
	   }
	   temp=temp+8;
	}
}

void lcd_init()//***Һ����ʼ������****
{
	Lcd_ram();
	write_1602com(0x38);//����Һ������ģʽ����˼��16*2����ʾ��5*7����8λ����
	write_1602com(0x0c);//����ʾ����ʾ���
	write_1602com(0x06);//�������ƶ�������Զ�����
	write_1602com(0x01);//����ʾ

	write_1602com(0x80);//��ʾ�̶����Ŵӵ�һ�е�1��λ��֮��ʼ��ʾ
	write_1602string(tab1);
	write_1602com(0x80+0x40);//��ʾ�̶�����д��λ�ã��ӵ�2��λ�ú�ʼ��ʾ
	write_1602string(tab2);

}