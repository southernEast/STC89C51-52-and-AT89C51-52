#include "SendMessage.h"


void main(void)
{
    Init();				// ��ʱ���ʹ��ڳ�ʼ��
 
    while(1)
    {
        Gas();		// ����Ƿ�������
        Delay(1000);
    }
}