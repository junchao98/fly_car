#ifndef __CONTROL_H__
#define __CONTROL_H__
#include "common.h"
#include "include.h"

//�������˿�
#define MOTOR_FTM   FTM3
#define MOTOR1_PWM  FTM_CH6	
#define MOTOR2_PWM  FTM_CH2	
#define MOTOR3_PWM  FTM_CH4	
#define MOTOR4_PWM  FTM_CH3     
#define MOTOR_HZ    (20*1000)	//����ģʽ�£�Ƶ��Ӧ���� 30~100��
				//����ģʽ�£�Ƶ��Ӧ���� 20k ����
#define MOTOR_MAX   950

extern int32 MOTOR_Duty_Left, MOTOR_Duty_Right;
extern int32 MOTOR_Speed_Left, MOTOR_Speed_Right;
extern int32 MOTOR_Left_Acc, MOTOR_Right_Acc;
extern int32 RC_Get;
extern uchar Run_Flag;
extern char Left_Crazy, Right_Crazy;
extern char Mode_Set;
extern uint8 Point_Mid;//Ŀ��������


/*********** �������� ************/
void Speed_Measure(void);	//����ٶȲ���
void Start_Control(void);	//�����߼����ͣ������
void MOTOR_Control(int32 LDuty, int32 RDuty);	// �������
int32 range_protect(int32 duty, int32 min, int32 max); //�޷�����

void Speed_Set_6(void);
void Speed_Set_7(void);

#endif