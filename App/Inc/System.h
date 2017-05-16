#ifndef __SYSTEM_H__
#define __SYSTEM_H__
#include "common.h"
#include "include.h"

extern uint8 ImageBuff[];
extern uint8 *imgbuff;
extern uint8 flag_run;
extern uint8 nrf_rx_buff[];         //预多
extern uint8 nrf_tx_buff[];         //预多
extern uint8 threshold_value, var_speed;

extern uchar Image_Send_Flag;
extern uchar Send_OK;
extern uchar System_OK;

void System_Init();		 //所有模块初始化
void data_send();		 //无线发送和接收数据
void HMI_Send_End(void);//发送HMI指令结束符
void HMI_Data_Handle(void);	//HMI串口数据收发

#endif