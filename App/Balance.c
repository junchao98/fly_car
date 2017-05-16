#include "Balance.h"

S_FLOAT_XYZ 
	GYRO_Real,		// ������ת���������
	ACC_Real,		// ���ٶȼ�ת���������
	Attitude_Angle,	// ��ǰ�Ƕ�
	Last_Angle,		// �ϴνǶ�
	Target_Angle,	// Ŀ��Ƕ�
	Now_Ang_Vel;	// ��ǰ���ٶ�
	

S_INT16_XYZ
	GYRO,			// ������ԭʼ����
	GYRO_Offset,	// ��������Ʈ
	GYRO_Last,		// �������ϴ�����
	ACC, 			// ���ٶȼ�����
	ACC_Offset,		// ���ٶȼ���Ʈ
	ACC_Last;		// ���ٶȼ��ϴ�����
S_INT32_XYZ
	Tar_Ang_Vel,	// Ŀ����ٶ�
	Tar_Ang_Vel_Last;	// �ϴ�Ŀ����ٶ�

int32 
	Speed_Now = 0,	// ��ǰʵ���ٶ�
	Speed_Min = 0,	// ������С�ٶ�
	Speed_Set = 0, 	// Ŀ���趨�ٶ�
	Theory_Duty = 0,// ����ֱ��ռ�ձ�
	Vel_Set = 0,	// Ŀ��ת����ٶ�
	Direct_Parameter = 0,// ת��ϵ��
	Direct_Last = 0,
	Radius = 0;		// Ŀ��ת��뾶

uchar Point = 40;
int32 Difference = 0;
	  
	  			/* ���ֱ�־λ���Ŷ�ʱ���н���ʱ����� */
char Speed_Flag, Angle_Flag, Ang_Velocity_Flag, Direct_Flag;
/********************************************************/

/********************* ����ƽ����� *********************/
// Ƶ�ʿ����ڶ�ʱ��������
void Balance_Control(void)
{
	if (Speed_Flag)		// �ٶȻ�	100ms
	{
		Speed_Flag = 0;
												/* �ٶȻ��ӵ��ǶȻ��ϴ������� */
		Target_Angle.Y = PID_Realize(&MOTOR_PID, MOTOR, Speed_Now, Speed_Set);	// ���Ϊ�Ŵ�100����Ŀ��Ƕ�
		Target_Angle.Y += Zero_Angle*100;	// Ŀ��Ƕȵ����������
		Target_Angle.Y = range_protect((int32)Target_Angle.Y, 500, 3000);	// -18 52
	}
	if (Direct_Flag)	// ת��	20ms
	{
		Direct_Flag = 0;
		
	}
	if (Angle_Flag)		// ֱ���ǶȻ�	10ms
	{
		Angle_Flag = 0;
		
		Get_Attitude();	// ��̬����
//		mpu_dmp_get_data(&Attitude_Angle.X, &Attitude_Angle.Y, &Attitude_Angle.Z);
		Speed_Measure();// ��ȡ��ǰ�ٶ�
											/* �ǶȻ��ӵ����ٶȻ��ϴ������� */
		Tar_Ang_Vel.Y = PID_Realize(&Angle_PID, Angle, (int32)(Attitude_Angle.Y*100), (int32)Target_Angle.Y);	// ���Ϊ�Ŵ�10����Ŀ����ٶ�
		Tar_Ang_Vel.Y = range_protect(Tar_Ang_Vel.Y, -800, 800);
	}
	if (Ang_Velocity_Flag)	// ֱ�����ٶȻ�	2ms
	{
		Ang_Velocity_Flag = 0;
		
		MPU6050_GetData(&GYRO, &ACC);	// ��ȡ����������
		Data_Filter();					// ��ԭʼ���ݻ����˲�	
		Now_Ang_Vel.Y = GYRO_Real.Y;	// ���㵱ǰ���ٶ�													
												/* ���ٶȻ���Ϊ���ڻ�����ֱ�� */
		Theory_Duty += PID_Increase(&Ang_Vel_PID, Ang_Vel, (int32)(Now_Ang_Vel.Y*10), (int32)(Tar_Ang_Vel.Y));	// ����ֱ��PWM
											
		if (Speed_Now > 20)
		{
			if (MOTOR_Speed_Left < MOTOR_Speed_Right*0.7)	// ����ת�ٽϵ�ʱҪ��ֹ��ת	���ڿ�������
			{
				Direct_Parameter = range_protect(Direct_Parameter, -100, Theory_Duty);	// �޷�����ת
			}
			if (MOTOR_Speed_Right < MOTOR_Speed_Left*0.7)
			{
				Direct_Parameter = range_protect(Direct_Parameter, -Theory_Duty, 100);	// �޷�����ת
			}
		}
		Direct_Last = Direct_Last*0 + Direct_Parameter*1;	// �����ϴν��ٶȻ����
		
		MOTOR_Duty_Left  = Theory_Duty - Direct_Last;	// ���ҵ������ת��ϵ����������
		MOTOR_Duty_Right = Theory_Duty + Direct_Last;
		
		if (Left_Crazy == 1 && Right_Crazy == 1)	// ��������ת�������˶�
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
			MOTOR_Control(MOTOR_Duty_Left, MOTOR_Duty_Right);	// �������ҵ��
		}
		else
		{
			MOTOR_Control(0, 0);
		}
	}
}
/*
//�������˲������뺯��
float dt=0.01;//ע�⣺dt��ȡֵΪkalman�˲�������ʱ��
float angle, angle_dot;//�ǶȺͽ��ٶ�
float P[2][2] = {{ 1, 0 },
                 { 0, 1 }};
float Pdot[4] ={ 0,0,0,0};
float Q_angle=0.001, Q_gyro=0.005; //�Ƕ��������Ŷ�,���ٶ��������Ŷ�
float R_angle=0.5 ,C_0 = 1;
float q_bias, angle_err, PCt_0, PCt_1, E, K_0, K_1, t_0, t_1;

//�������˲�
float Kalman_Filter(float angle_m, float gyro_m)//angleAx �� gyroGy
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
        angle += K_0 * angle_err; //���ŽǶ�
        q_bias += K_1 * angle_err;
        angle_dot = gyro_m-q_bias;//���Ž��ٶ�

        return angle��
}*/
