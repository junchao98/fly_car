#include "Balance.h"

S_FLOAT_XYZ 
	GYRO_Real,		// 陀螺仪转化后的数据
	ACC_Real,		// 加速度计转化后的数据
	Attitude_Angle,	// 当前角度
	Last_Angle,		// 上次角度
	Target_Angle,	// 目标角度
	Now_Ang_Vel;	// 当前角速度
	

S_INT16_XYZ
	GYRO,			// 陀螺仪原始数据
	GYRO_Offset,	// 陀螺仪温飘
	GYRO_Last,		// 陀螺仪上次数据
	ACC, 			// 加速度计数据
	ACC_Offset,		// 加速度计温飘
	ACC_Last;		// 加速度计上次数据
S_INT32_XYZ
	Tar_Ang_Vel,	// 目标角速度
	Tar_Ang_Vel_Last;	// 上次目标角速度

int32 
	Speed_Now = 0,	// 当前实际速度
	Speed_Min = 0,	// 左右最小速度
	Speed_Set = 0, 	// 目标设定速度
	Theory_Duty = 0,// 理论直立占空比
	Vel_Set = 0,	// 目标转向角速度
	Direct_Parameter = 0,// 转向系数
	Direct_Last = 0,
	Radius = 0;		// 目标转向半径

uchar Point = 40;
int32 Difference = 0;
	  
	  			/* 各种标志位，放定时器中进行时序控制 */
char Speed_Flag, Angle_Flag, Ang_Velocity_Flag, Direct_Flag;
/********************************************************/

/********************* 串级平衡控制 *********************/
// 频率控制在定时器中设置
void Balance_Control(void)
{
	if (Speed_Flag)		// 速度环	100ms
	{
		Speed_Flag = 0;
												/* 速度环加到角度环上串级控制 */
		Target_Angle.Y = PID_Realize(&MOTOR_PID, MOTOR, Speed_Now, Speed_Set);	// 结果为放大100倍的目标角度
		Target_Angle.Y += Zero_Angle*100;	// 目标角度叠加在零点上
		Target_Angle.Y = range_protect((int32)Target_Angle.Y, 500, 3000);	// -18 52
	}
	if (Direct_Flag)	// 转向环	20ms
	{
		Direct_Flag = 0;
		
	}
	if (Angle_Flag)		// 直立角度环	10ms
	{
		Angle_Flag = 0;
		
		Get_Attitude();	// 姿态解算
//		mpu_dmp_get_data(&Attitude_Angle.X, &Attitude_Angle.Y, &Attitude_Angle.Z);
		Speed_Measure();// 获取当前速度
											/* 角度环加到角速度环上串级控制 */
		Tar_Ang_Vel.Y = PID_Realize(&Angle_PID, Angle, (int32)(Attitude_Angle.Y*100), (int32)Target_Angle.Y);	// 结果为放大10倍的目标角速度
		Tar_Ang_Vel.Y = range_protect(Tar_Ang_Vel.Y, -800, 800);
	}
	if (Ang_Velocity_Flag)	// 直立角速度环	2ms
	{
		Ang_Velocity_Flag = 0;
		
		MPU6050_GetData(&GYRO, &ACC);	// 读取陀螺仪数据
		Data_Filter();					// 对原始数据滑动滤波	
		Now_Ang_Vel.Y = GYRO_Real.Y;	// 计算当前角速度													
												/* 角速度环作为最内环控制直立 */
		Theory_Duty += PID_Increase(&Ang_Vel_PID, Ang_Vel, (int32)(Now_Ang_Vel.Y*10), (int32)(Tar_Ang_Vel.Y));	// 计算直立PWM
											
		if (Speed_Now > 20)
		{
			if (MOTOR_Speed_Left < MOTOR_Speed_Right*0.7)	// 车轮转速较低时要防止倒转	后期可能有用
			{
				Direct_Parameter = range_protect(Direct_Parameter, -100, Theory_Duty);	// 限幅防倒转
			}
			if (MOTOR_Speed_Right < MOTOR_Speed_Left*0.7)
			{
				Direct_Parameter = range_protect(Direct_Parameter, -Theory_Duty, 100);	// 限幅防倒转
			}
		}
		Direct_Last = Direct_Last*0 + Direct_Parameter*1;	// 更新上次角速度环结果
		
		MOTOR_Duty_Left  = Theory_Duty - Direct_Last;	// 左右电机根据转向系数调整差速
		MOTOR_Duty_Right = Theory_Duty + Direct_Last;
		
		if (Left_Crazy == 1 && Right_Crazy == 1)	// 两个都疯转，自由运动
		{
			Theory_Duty = 0;
			MOTOR_Duty_Left = 0;
			MOTOR_Duty_Right = 0;
			Direct_Parameter = 0;
			Ang_Vel_PID.SumError = 0;
			Run_Flag = 0;
		}
		
		if (Run_Flag)
		{
			MOTOR_Control(MOTOR_Duty_Left, MOTOR_Duty_Right);	// 控制左右电机
		}
		else
		{
			MOTOR_Control(0, 0);
		}
	}
}
/*
//卡尔曼滤波参数与函数
float dt=0.01;//注意：dt的取值为kalman滤波器采样时间
float angle, angle_dot;//角度和角速度
float P[2][2] = {{ 1, 0 },
                 { 0, 1 }};
float Pdot[4] ={ 0,0,0,0};
float Q_angle=0.001, Q_gyro=0.005; //角度数据置信度,角速度数据置信度
float R_angle=0.5 ,C_0 = 1;
float q_bias, angle_err, PCt_0, PCt_1, E, K_0, K_1, t_0, t_1;

//卡尔曼滤波
float Kalman_Filter(float angle_m, float gyro_m)//angleAx 和 gyroGy
{
        angle += (gyro_m-q_bias) * dt;
        angle_err = angle_m - angle;
        Pdot[0] = Q_angle - P[0][1] - P[1][0];
        Pdot[1] = -P[1][1];
        Pdot[2] = -P[1][1];
        Pdot[3] = Q_gyro;
        P[0][0] += Pdot[0] * dt;
        P[0][1] += Pdot[1] * dt;
        P[1][0] += Pdot[2] * dt;
        P[1][1] += Pdot[3] * dt;
        PCt_0 = C_0 * P[0][0];
        PCt_1 = C_0 * P[1][0];
        E = R_angle + C_0 * PCt_0;
        K_0 = PCt_0 / E;
        K_1 = PCt_1 / E;
        t_0 = PCt_0;
        t_1 = C_0 * P[0][1];
        P[0][0] -= K_0 * t_0;
        P[0][1] -= K_0 * t_1;
        P[1][0] -= K_1 * t_0;
        P[1][1] -= K_1 * t_1;
        angle += K_0 * angle_err; //最优角度
        q_bias += K_1 * angle_err;
        angle_dot = gyro_m-q_bias;//最优角速度

        return angle；
}*/
