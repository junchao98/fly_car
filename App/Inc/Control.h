#ifndef __CONTROL_H__
#define __CONTROL_H__
#include "common.h"
#include "include.h"

//定义电机端口
#define MOTOR_FTM   FTM3
#define MOTOR1_PWM  FTM_CH6	
#define MOTOR2_PWM  FTM_CH2	
#define MOTOR3_PWM  FTM_CH4	
#define MOTOR4_PWM  FTM_CH3     
#define MOTOR_HZ    (20*1000)	//滑行模式下，频率应该是 30~100。
				//常规模式下，频率应该是 20k 左右
#define MOTOR_MAX   950

extern int32 MOTOR_Duty_Left, MOTOR_Duty_Right;
extern int32 MOTOR_Speed_Left, MOTOR_Speed_Right;
extern int32 MOTOR_Left_Acc, MOTOR_Right_Acc;
extern int32 RC_Get;
extern uchar Run_Flag;
extern char Left_Crazy, Right_Crazy;
extern char Mode_Set;
extern uint8 Point_Mid;//目标点横坐标


/*********** 函数声明 ************/
void Speed_Measure(void);	//电机速度测量
void Start_Control(void);	//起跑线检测与停车控制
void MOTOR_Control(int32 LDuty, int32 RDuty);	// 电机控制
int32 range_protect(int32 duty, int32 min, int32 max); //限幅保护

void Speed_Set_6(void);
void Speed_Set_7(void);

#endif