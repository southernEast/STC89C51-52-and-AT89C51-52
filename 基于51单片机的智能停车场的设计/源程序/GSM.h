#ifndef __GSM_H__
#define __GSM_H__

#include <reg52.h>
#include <string.h>
#include <stdio.h>
#include "LCD1602.h"
#include "cars.h"


/*************	���س�������	**************/

#define COMMUNITE_ERROR       -1 
#define NO_SIM_CARD_ERROR     -2
#define SIM_CARD_NO_REG_ERROR -3
#define CALL_ERROR			  -4
#define AT_CPMS_ERROR		  -5
#define AT_CMGF_ERROR		  -6
#define AT_CSCS_ERROR		  -7
#define AT_CMGS_ERROR         -8
#define END_CHAR_ERROR		  -9
#define NO_SMS_LEN_ERROR      -22
#define AT_CNMI_ERROR     -33
#define ATE_ERROR         -34

#define Buf_Max 	 	  85 	//����1���泤��
#define STABLE_TIMES	  20	//�ȴ��ϵ���ȶ�

/*************	���ر�������	**************/

//����1���ͻس�����
#define UART1_SendLR() UART1_SendData(0X0D)

extern uchar parkingFlag[4];
extern uint num;

extern void GSM_init();		// GSM��ʼ��
extern void GSM_scan();		// GSM�ź�ɨ��

#endif