#include "Control.h"

uchar Run_Flag = 0;
char Left_Crazy = 0;	// 电机疯转
char Right_Crazy = 0;	// 电机疯转
int32 RC_Get = 1500;	// 遥控器的通道值
int32 MOTOR_Duty_Left  = 0;
int32 MOTOR_Duty_Right = 0;
int32 MOTOR_Speed_Left = 0;
int32 MOTOR_Speed_Right = 0; 
int32 MOTOR_Speed_Left_Last = 0;
int32 MOTOR_Speed_Right_Last = 0;
int32 MOTOR_Left_Acc = 0;
int32 MOTOR_Right_Acc = 0;
char Mode_Set = 5;	// 模式选择 	very important !!!!!!

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
		LDuty = range_protect(LDuty, 0, MOTOR_MAX);	// 限幅保护
		ftm_pwm_duty(MOTOR_FTM, MOTOR1_PWM,LDuty);	// 占空比最大990！！！
		ftm_pwm_duty(MOTOR_FTM, MOTOR2_PWM,0);	  	// 占空比最大990！！！
	}
	else
	{
		LDuty = range_protect(-LDuty, 0, MOTOR_MAX);// 限幅保护
		ftm_pwm_duty(MOTOR_FTM, MOTOR1_PWM,0);		// 占空比最大990！！！
		ftm_pwm_duty(MOTOR_FTM, MOTOR2_PWM,LDuty);	// 占空比最大990！！！
	}
	
	if (RDuty >= 0)
	{
		RDuty = range_protect(RDuty, 0, MOTOR_MAX);	// 限幅保护
		ftm_pwm_duty(MOTOR_FTM, MOTOR3_PWM,RDuty);	// 占空比最大990！！！
		ftm_pwm_duty(MOTOR_FTM, MOTOR4_PWM,0);	  	// 占空比最大990！！！
	}
	else
	{
		RDuty = range_protect(-RDuty, 0, MOTOR_MAX);// 限幅保护
		ftm_pwm_duty(MOTOR_FTM, MOTOR3_PWM,0);		// 占空比最大990！！！
		ftm_pwm_duty(MOTOR_FTM, MOTOR4_PWM,RDuty);	// 占空比最大990！！！
	}
}

/******* 电机速度测量 ********/
void Speed_Measure(void)
{
	int32 Pulses;
	static int32 Speed_Last = 0;
//	static uchar Left_Count = 0, Right_Count = 0;
	
	/******* 右电机速度相关控制 ********/
	Pulses = ftm_quad_get(FTM1);	// 获取FTM 正交解码 的脉冲数(负数表示反方向)
	ftm_quad_clean(FTM1);   		// 正交解码寄存器清零
	MOTOR_Speed_Right = -Pulses;	// 得到右轮转速
	if (!Right_Crazy)
	{
		MOTOR_Speed_Right = 0.7*MOTOR_Speed_Right + 0.3*MOTOR_Speed_Right_Last;
	}
	MOTOR_Right_Acc = MOTOR_Speed_Right - MOTOR_Speed_Right_Last;			// 计算加速度
	MOTOR_Speed_Right_Last = MOTOR_Speed_Right;	// 更新右轮速度
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
	/******* 右电机速度相关控制结束 ********/
	
	/******* 左电机速度相关控制 ********/
	Pulses = ftm_quad_get(FTM2);	// 获取FTM 正交解码 的脉冲数(负数表示反方向)
	ftm_quad_clean(FTM2);			// 正交解码寄存器清零
	MOTOR_Speed_Left = Pulses;		// 得到左轮转速
	if (!Left_Crazy)
	{
		MOTOR_Speed_Left = 0.7*MOTOR_Speed_Left + 0.3*MOTOR_Speed_Left_Last;	// 低通滤波
	}
	MOTOR_Left_Acc = MOTOR_Speed_Left - MOTOR_Speed_Left_Last;	// 计算加速度
	MOTOR_Speed_Left_Last = MOTOR_Speed_Left;	// 更新左轮速度
	
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
	/******* 左电机速度相关控制结束 ********/
	
	if (MOTOR_Speed_Left <= MOTOR_Speed_Right)
	{
		Speed_Min = MOTOR_Speed_Left;
	}
	else
	{
		Speed_Min = MOTOR_Speed_Right;
	}
	
	/******* 电机疯转特殊处理 ********/
	if ((Left_Crazy == 1 || Left_Crazy == -1) && (Right_Crazy == 1 || Right_Crazy == -1))
	{
		Speed_Now = Speed_Last;			// 两边都疯转，使用上次速度作为当前实际速度
	}
	else if (Left_Crazy == 1 || Left_Crazy == -1)
	{
		Speed_Now = Speed_Last;	// 左电机疯转，使用上次速度作为当前实际速度
	}
	else if (Right_Crazy == 1 || Right_Crazy == -1)
	{
		Speed_Now = Speed_Last;	// 右电机疯转，使用上次速度作为当前实际速度
	}
	else
	{
		Speed_Now = (MOTOR_Speed_Left + MOTOR_Speed_Right) / 2;	// 左右取平均计算车子实际速度
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
	{									/** 起跑阶段检测 **/
		if (!Starting_Line_Flag && !Start_OK)	//没有检测到起跑线且未成功出发
		{
			No_Start_Line_Count++;			//相当于延时约5s
			if (No_Start_Line_Count >= 200)	//连续200次没有检测到起跑线，避免起跑时的光线异常干扰
			{
				No_Start_Line_Count = 0;
				Start_OK = 1;	//出发成功
			}
		}
										/** 结束阶段检测 **/
		if (Start_OK && Starting_Line_Flag)	//成功起跑后检测到终点线
		{
			Ready_Stop = 1;	//准备停车
		}
		
		if (Ready_Stop)
		{
			if (!Starting_Line_Flag)	/** 检测到停车线后延时一段时间停车 **/
			{
				No_Start_Line_Count++;
				if (No_Start_Line_Count >= 7)	//连续30次没有检测到起跑线，约延时1s
				{
					No_Start_Line_Count = 0;
					Run_Flag = 0;			//停车
					Start_OK = 0;			//清除出发成功标志位
					Ready_Stop = 0;			//清除准备停车标志位
//					led(LED_MAX,LED_OFF);	//关闭LED指示灯
					led(LED2,LED_OFF);		//关闭LED指示灯
					led(LED3,LED_OFF);		//关闭LED指示灯
				}
			}
		}
	}
}

/******** 限幅保护 *********/
int32 range_protect(int32 duty, int32 min, int32 max)//限幅保护
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
