#include "Control.h"

uchar Run_Flag = 0;
char Left_Crazy = 0;	// �����ת
char Right_Crazy = 0;	// �����ת
int32 RC_Get = 1500;	// ң������ͨ��ֵ
int32 MOTOR_Duty_Left  = 0;
int32 MOTOR_Duty_Right = 0;
int32 MOTOR_Speed_Left = 0;
int32 MOTOR_Speed_Right = 0; 
int32 MOTOR_Speed_Left_Last = 0;
int32 MOTOR_Speed_Right_Last = 0;
int32 MOTOR_Left_Acc = 0;
int32 MOTOR_Right_Acc = 0;
char Mode_Set = 5;	// ģʽѡ�� 	very important !!!!!!

void Speed_Control(void)
{
	if (Run_Flag)
	{
		switch(Mode_Set)
		{
		case 1:		Speed_Set = 70;
				 	break;
		case 2:		Speed_Set = 80;
				 	break;
		case 3:		Speed_Set = 90;
				 	break;
		case 4:		Speed_Set = 100;
				 	break;
		case 5:		Speed_Set = 110;
				 	break;
		case 6:		Speed_Set_6();
				 	break;
		case 7:		Speed_Set_7();
					break;
		default:	Speed_Set = 0;
					break;
		}
	}
	else
	{
		Speed_Set = 0;
	}
}

void Speed_Set_6(void)
{
	if (Foresight >= 22)
	{
		Speed_Set = 110;
	}
	else if (Foresight >= 16)
	{
		Speed_Set = 140;
	}
	else if (Foresight >= 14)
	{
		Speed_Set = 150;
	}
	else if (Foresight >= 12)
	{
		Speed_Set = 160;
	}
	else if (Foresight >= 10)
	{
		Speed_Set = 170;
	}
	else if (Foresight >= 8)
	{
		Speed_Set = 180;
	}
	else if (Foresight >= 7)
	{
		Speed_Set = 200;
	}
	else if (Foresight >= 6)
	{
		Speed_Set = 200;
	}
	else if (Foresight >= 5)
	{
		Speed_Set = 200;
	}
	else
	{
		Speed_Set = 200;
	}
}

void Speed_Set_7(void)
{
	if (Foresight >= 18)
	{
		Speed_Set = 160;
	}
	else if (Foresight >= 16)
	{
		Speed_Set = 170;
	}
	else if (Foresight >= 14)
	{
		Speed_Set = 180;
	}
	else if (Foresight >= 12)
	{
		Speed_Set = 190;
	}
	else if (Foresight >= 10)
	{
		Speed_Set = 200;
	}
	else if (Foresight >= 8)
	{
		Speed_Set = 210;
	}
	else if (Foresight >= 7)
	{
		Speed_Set = 220;
	}
	else if (Foresight >= 6)
	{
		Speed_Set = 230;
	}
	else if (Foresight >= 5)
	{
		Speed_Set = 240;
	}
	else
	{
		Speed_Set = 260;
	}
}

void MOTOR_Control(int32 LDuty, int32 RDuty)
{
	if (LDuty >= 0)
	{
		LDuty = range_protect(LDuty, 0, MOTOR_MAX);	// �޷�����
		ftm_pwm_duty(MOTOR_FTM, MOTOR1_PWM,LDuty);	// ռ�ձ����990������
		ftm_pwm_duty(MOTOR_FTM, MOTOR2_PWM,0);	  	// ռ�ձ����990������
	}
	else
	{
		LDuty = range_protect(-LDuty, 0, MOTOR_MAX);// �޷�����
		ftm_pwm_duty(MOTOR_FTM, MOTOR1_PWM,0);		// ռ�ձ����990������
		ftm_pwm_duty(MOTOR_FTM, MOTOR2_PWM,LDuty);	// ռ�ձ����990������
	}
	
	if (RDuty >= 0)
	{
		RDuty = range_protect(RDuty, 0, MOTOR_MAX);	// �޷�����
		ftm_pwm_duty(MOTOR_FTM, MOTOR3_PWM,RDuty);	// ռ�ձ����990������
		ftm_pwm_duty(MOTOR_FTM, MOTOR4_PWM,0);	  	// ռ�ձ����990������
	}
	else
	{
		RDuty = range_protect(-RDuty, 0, MOTOR_MAX);// �޷�����
		ftm_pwm_duty(MOTOR_FTM, MOTOR3_PWM,0);		// ռ�ձ����990������
		ftm_pwm_duty(MOTOR_FTM, MOTOR4_PWM,RDuty);	// ռ�ձ����990������
	}
}

/******* ����ٶȲ��� ********/
void Speed_Measure(void)
{
	int32 Pulses;
	static int32 Speed_Last = 0;
//	static uchar Left_Count = 0, Right_Count = 0;
	
	/******* �ҵ���ٶ���ؿ��� ********/
	Pulses = ftm_quad_get(FTM1);	// ��ȡFTM �������� ��������(������ʾ������)
	ftm_quad_clean(FTM1);   		// ��������Ĵ�������
	MOTOR_Speed_Right = -Pulses;	// �õ�����ת��
	if (!Right_Crazy)
	{
		MOTOR_Speed_Right = 0.7*MOTOR_Speed_Right + 0.3*MOTOR_Speed_Right_Last;
	}
	MOTOR_Right_Acc = MOTOR_Speed_Right - MOTOR_Speed_Right_Last;			// ������ٶ�
	MOTOR_Speed_Right_Last = MOTOR_Speed_Right;	// ���������ٶ�
	if (MOTOR_Right_Acc > -30 && MOTOR_Right_Acc < 100)
	{
		if ((MOTOR_Speed_Right < Speed_Set + 100) && (MOTOR_Speed_Right > -20))
		{
			Right_Crazy = 0;
		}
	}
	else if (MOTOR_Right_Acc >= 100)
	{
		Right_Crazy = 1;
	}
	else if (MOTOR_Right_Acc <= -30)
	{
		Right_Crazy = -1;
	}
	if (MOTOR_Speed_Right > Speed_Set + 400 || MOTOR_Speed_Right < -60)
	{
		Right_Crazy = 1;
	}
	/******* �ҵ���ٶ���ؿ��ƽ��� ********/
	
	/******* �����ٶ���ؿ��� ********/
	Pulses = ftm_quad_get(FTM2);	// ��ȡFTM �������� ��������(������ʾ������)
	ftm_quad_clean(FTM2);			// ��������Ĵ�������
	MOTOR_Speed_Left = Pulses;		// �õ�����ת��
	if (!Left_Crazy)
	{
		MOTOR_Speed_Left = 0.7*MOTOR_Speed_Left + 0.3*MOTOR_Speed_Left_Last;	// ��ͨ�˲�
	}
	MOTOR_Left_Acc = MOTOR_Speed_Left - MOTOR_Speed_Left_Last;	// ������ٶ�
	MOTOR_Speed_Left_Last = MOTOR_Speed_Left;	// ���������ٶ�
	
	if (MOTOR_Left_Acc > -30 && MOTOR_Left_Acc < 100)
	{
		if ((MOTOR_Speed_Left < Speed_Set + 100) && (MOTOR_Speed_Left > -20))
		{
			Left_Crazy = 0;
		}
	}
	else if (MOTOR_Left_Acc >= 100)
	{
		Left_Crazy = 1;
	}
	else if (MOTOR_Left_Acc <= -30)
	{
		Left_Crazy = -1;
	}
	if (MOTOR_Speed_Left > Speed_Set + 400 || MOTOR_Speed_Left < -60)
	{
		Left_Crazy = 1;
	}
	/******* �����ٶ���ؿ��ƽ��� ********/
	
	if (MOTOR_Speed_Left <= MOTOR_Speed_Right)
	{
		Speed_Min = MOTOR_Speed_Left;
	}
	else
	{
		Speed_Min = MOTOR_Speed_Right;
	}
	
	/******* �����ת���⴦�� ********/
	if ((Left_Crazy == 1 || Left_Crazy == -1) && (Right_Crazy == 1 || Right_Crazy == -1))
	{
		Speed_Now = Speed_Last;			// ���߶���ת��ʹ���ϴ��ٶ���Ϊ��ǰʵ���ٶ�
	}
	else if (Left_Crazy == 1 || Left_Crazy == -1)
	{
		Speed_Now = Speed_Last;	// ������ת��ʹ���ϴ��ٶ���Ϊ��ǰʵ���ٶ�
	}
	else if (Right_Crazy == 1 || Right_Crazy == -1)
	{
		Speed_Now = Speed_Last;	// �ҵ����ת��ʹ���ϴ��ٶ���Ϊ��ǰʵ���ٶ�
	}
	else
	{
		Speed_Now = (MOTOR_Speed_Left + MOTOR_Speed_Right) / 2;	// ����ȡƽ�����㳵��ʵ���ٶ�
	}
	Speed_Now = Speed_Now *0.9 + Speed_Last * 0.1;
//	if (Speed_Now <= 1)
//	{
//		Speed_Now = 1;
//	}
	Speed_Last = Speed_Now;
}

void Start_Control(void)
{
	static uchar Start_OK = 0;
	static uchar Ready_Stop = 0;
	static uint16 No_Start_Line_Count = 0;
	
	if (Run_Flag)
	{									/** ���ܽ׶μ�� **/
		if (!Starting_Line_Flag && !Start_OK)	//û�м�⵽��������δ�ɹ�����
		{
			No_Start_Line_Count++;			//�൱����ʱԼ5s
			if (No_Start_Line_Count >= 200)	//����200��û�м�⵽�����ߣ���������ʱ�Ĺ����쳣����
			{
				No_Start_Line_Count = 0;
				Start_OK = 1;	//�����ɹ�
			}
		}
										/** �����׶μ�� **/
		if (Start_OK && Starting_Line_Flag)	//�ɹ����ܺ��⵽�յ���
		{
			Ready_Stop = 1;	//׼��ͣ��
		}
		
		if (Ready_Stop)
		{
			if (!Starting_Line_Flag)	/** ��⵽ͣ���ߺ���ʱһ��ʱ��ͣ�� **/
			{
				No_Start_Line_Count++;
				if (No_Start_Line_Count >= 7)	//����30��û�м�⵽�����ߣ�Լ��ʱ1s
				{
					No_Start_Line_Count = 0;
					Run_Flag = 0;			//ͣ��
					Start_OK = 0;			//��������ɹ���־λ
					Ready_Stop = 0;			//���׼��ͣ����־λ
//					led(LED_MAX,LED_OFF);	//�ر�LEDָʾ��
					led(LED2,LED_OFF);		//�ر�LEDָʾ��
					led(LED3,LED_OFF);		//�ر�LEDָʾ��
				}
			}
		}
	}
}

/******** �޷����� *********/
int32 range_protect(int32 duty, int32 min, int32 max)//�޷�����
{
	if (duty >= max)
	{
		return max;
	}
	if (duty <= min)
	{
		return min;
	}
	else
	{
		return duty;
	}
}
