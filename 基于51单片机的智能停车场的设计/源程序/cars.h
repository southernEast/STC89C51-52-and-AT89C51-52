#ifndef __CARS_H__
#define __CARS_H__

#include <reg52.h>
#include <string.h>
#include <stdio.h>
#include "LCD1602.h"

typedef unsigned char uchar;
typedef unsigned int uint;

sbit parking0 = P1^0;			// ����ͣ������λ����
sbit parking1 = P1^1;
sbit parking2 = P1^2;
sbit parking3 = P1^3;
sbit inDetect = P2^4;			// ����ͣ����������
sbit outDetect= P2^3;			// �뿪ͣ����������


extern void showInfo();

#endif