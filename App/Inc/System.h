#ifndef __SYSTEM_H__
#define __SYSTEM_H__
#include "common.h"
#include "include.h"

extern uint8 ImageBuff[];
extern uint8 *imgbuff;
extern uint8 flag_run;
extern uint8 nrf_rx_buff[];         //Ԥ��
extern uint8 nrf_tx_buff[];         //Ԥ��
extern uint8 threshold_value, var_speed;

extern uchar Image_Send_Flag;
extern uchar Send_OK;
extern uchar System_OK;

void System_Init();		 //����ģ���ʼ��
void data_send();		 //���߷��ͺͽ�������
void HMI_Send_End(void);//����HMIָ�������
void HMI_Data_Handle(void);	//HMI���������շ�

#endif