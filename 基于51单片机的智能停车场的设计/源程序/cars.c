#include "cars.h"


/**ͳ�Ƴ���**/
uchar outBuf[16];				// ����ַ�������
uchar parkingFlag[4];		// �ĸ�ͣ��λ��־
uint num;								// ������Ŀ����

void cars_delay(uint i)
{
	while(i--);
}

/**ͳ�Ƶ�ǰͣ����������Ŀ**/
void countNum()
{
	if(inDetect == 0){		// ���복��
		while(!inDetect)
			;
		num++;
	}
	if(outDetect == 0){		// �뿪����
		while(!outDetect)
			;
		if(num > 0)
			num--;
	}
}

/**ɨ�賵λ**/
void parkingPlaceScan()
{
	if(parking0 == 0){			// ��⵽�����ź�
			cars_delay(100);
			if(parking0 == 0){
				parkingFlag[0] = 'Y';
			}
		}
		else{									// �޳����ź�
			parkingFlag[0] = 'N';
		}
		if(parking1 == 0){
			cars_delay(100);
			if(parking1 == 0){
				parkingFlag[1] = 'Y';
			}
		}
		else{
			parkingFlag[1] = 'N';
		}
		if(parking2 == 0){
			cars_delay(100);
			if(parking2 == 0){
				parkingFlag[2] = 'Y';
			}
		}
		else{
			parkingFlag[2] = 'N';
		}
		if(parking3 == 0){
			cars_delay(100);
			if(parking3 == 0){
				parkingFlag[3] = 'Y';
			}
		}
		else{
			parkingFlag[3] = 'N';
		}
}

void showInfo()
{
	sprintf(outBuf, "%c %c %c %c  %4d   ", 
					parkingFlag[0], parkingFlag[1], parkingFlag[2], parkingFlag[3], num);			// ����Ҫ��ʾ����Ϣ�����ַ�����
	parkingPlaceScan();							// ɨ�賵λ��Ϣ
	LCD1602_write_com(0x80);
	LCD1602_write_word("1 2 3 4 NOW NUM:");	// LCD��ʾ��һ��
	LCD1602_write_com(0xc0);
	LCD1602_write_word(outBuf);			// LCD��ʾ�ڶ���
	countNum();

}