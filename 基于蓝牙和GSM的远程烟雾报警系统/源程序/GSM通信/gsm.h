#ifndef __GSM_H__
#define __GSM_H__

#include<reg52.h>
#include "string.h"

typedef 	unsigned char	u8;
typedef 	unsigned int	u16;
typedef 	unsigned long	u32;

#define COMMUNITE_ERROR       -1 
#define NO_SIM_CARD_ERROR     -2
#define SIM_CARD_NO_REG_ERROR -3
#define CALL_ERROR			  -4
#define AT_CPMS_ERROR		  -5
#define AT_CMGF_ERROR		  -6
#define AT_CSCS_ERROR		  -7
#define AT_CMGS_ERROR         -8
#define END_CHAR_ERROR		  -9
#define AT_CSMP_ERROR		  -10

#define Buf_Max 	 	  70 	//����1���泤��


//����1���ͻس�����
#define UART1_SendLR() UART1_SendData(0X0D)

extern void delay_ms(u16 ms);   //��ʱms��20ms  ms*20ms
extern void CLR_Buf(void);      //�崮�ڽ��ջ���
extern u8 Find(u8 *a);        //�����ַ���
extern void UART1_SendData(u8 dat);   //����1���� 1�ֽ�
extern void UART1_SendString(char *s);//����1���� �ַ���
extern void UART1_Send_Command(char *s);
extern void UART1_Send_Command_END(char *s);
extern u8 UART1_Send_AT_Command(u8 *b,u8 *a,u8 wait_time,u16 interval_time);//at�����
extern u8 UART1_Send_AT_Command_END(u8 *b,u8 *a,u8 wait_time,u16 interval_time);

extern int send_text_message(char *content);
extern void Timer0Init(void);
extern void Uart1Init(void);

#endif