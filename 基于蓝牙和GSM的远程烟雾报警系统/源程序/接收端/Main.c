#include "ReceiveMessage.h"

void main(void)
{

	Init();					// ���ڳ�ʼ��
	
	while(1)
	{
		receive();		// ������Ϣ
		delay(100);
		if(select()){	// �ж��Ƿ�������
			EA = 0;			// �ر����ж�
			beep = 0;		// ����������
			led = 0;		// ����Ӧ����
			delay(5000);
			EA = 1;			// �������ж�
		}
		else{
			beep = 1;		// �رշ�����
			led = 1;		// �ر�Ӧ����
		}
	}
}
