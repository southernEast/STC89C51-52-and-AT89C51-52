#include "cars.h"
#include "GSM.h"

extern uint num;

void main()
{
	Init_LCD1602();		// ��ʼ��LCD1602
	GSM_init();				// ��ʼ��GSM
	num = 0;					// ��ʼ������������
	
	while(1){
		showInfo();			// ��ʾ��Ϣ
		GSM_scan();			// ɨ��GSM�ӿ� �Ƿ���յ���Ϣ
	}
}