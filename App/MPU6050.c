#include "MPU6050.h"

//�궨����õײ��I2C�ӿ�
#define MPU6050_OPEN()      	IIC_init()
#define MPU6050_WR(reg,value)   simiic_write_reg(MPU6050_ADRESS, reg, value)	//mpu6050 д�Ĵ���
#define MPU6050_RD(reg)         simiic_read_reg(MPU6050_ADRESS, reg, IIC)		//mpu6050 ���Ĵ���

char Offset_OK = 0;
 
/*
 * ��������MPU6050_Init
 * ����  �������ǳ�ʼ��
 * ����  ����
 * ���  ��0�ɹ�  1ʧ��
 * ����  ���ⲿ����
 */
uchar MPU6050_Init(void)
{ 
	uchar res;
	
	MPU6050_OPEN();           				// ��ʼ�� mpu6050 �ӿ�
	DELAY_MS(100);
	MPU6050_WR(MPU_PWR_MGMT1_REG,0X80);		// ��λMPU6050
  	DELAY_MS(10);
	MPU6050_WR(MPU_PWR_MGMT1_REG,0X00);		// ����MPU6050 
	DELAY_MS(10);
	MPU6050_WR(MPU_PWR_MGMT1_REG,0X01);		// ����CLKSEL,PLL X��Ϊ�ο�
	DELAY_MS(10);
	MPU6050_WR(MPU_GYRO_CFG_REG,3<<3);		// �����Ǵ�����,��2000dps	// 0,��250dps;1,��500dps;2,��1000dps;3,��2000dps
	DELAY_MS(10);
	MPU6050_WR(MPU_ACCEL_CFG_REG,0<<3);		// ���ٶȴ�����,��2g		// 0,��2g;1,��4g;2,��8g;3,��16g
	DELAY_MS(10);
	MPU6050_WR(MPU_SAMPLE_RATE_REG,0X07);	// ����MPU6050�Ĳ�����,8KHz
	DELAY_MS(10);
	MPU6050_WR(MPU_CFG_REG,0X00);			// �������ֵ�ͨ�˲���,256Hz
	DELAY_MS(100);
	
	res=MPU6050_RD(MPU_DEVICE_ID_REG);
	if(res==MPU6050_ADRESS)// ����ID��ȷ
	{
		MPU6050_Offset();
		
		return 0;
 	}
	else 
	{
		return 1;
	}
}

/*
 * ��������MPU6050_Offset
 * ����  ���������ɼ���ƫ
 * ����  ����
 * ���  ����
 * ����  ���ڲ�����
 */
void MPU6050_Offset(void)
{
	uint8 i, Count = 100;
	int64 temp[6] = {0};
	
	GYRO_Offset.X = 0;
	GYRO_Offset.Y = 0;
	GYRO_Offset.Z = 0;
	
	for (i = 0; i < Count; i++)
	{
		MPU6050_GetData(&GYRO, &ACC);	// ��ȡ����������
		DELAY_MS(2);
		
		temp[0] += ACC.X;
		temp[1] += ACC.Y;
		temp[2] += ACC.Z;
		
		temp[3] += GYRO.X;
		temp[4] += GYRO.Y;
		temp[5] += GYRO.Z;
	}
	ACC_Offset.X = temp[0] / Count;
	ACC_Offset.Y = temp[1] / Count;
	ACC_Offset.Z = temp[2] / Count;
	
	GYRO_Offset.X = temp[3] / Count;
	GYRO_Offset.Y = temp[4] / Count;
	GYRO_Offset.Z = temp[5] / Count;
	
	Offset_OK = 1;
}

/*
 * ��������MPU6050_GetData
 * ����  ����ô�������������
 * ����  ��*GYRO ������		*ACC ���ٶȼ�
 * ���  ����
 * ����  ���ⲿ����
 */
void MPU6050_GetData(S_INT16_XYZ *GYRO, S_INT16_XYZ *ACC)
{
	if (Offset_OK)
	{
		ACC->X = GetData(MPU_ACCEL_XOUTH_REG);	// ��ȡ���ٶȼ�ԭʼ����
		ACC->Y = GetData(MPU_ACCEL_YOUTH_REG);
		ACC->Z = GetData(MPU_ACCEL_ZOUTH_REG);
		
		GYRO->X = GetData(MPU_GYRO_XOUTH_REG) - GYRO_Offset.X;	// ��ȡ������ԭʼ����
		GYRO->Y = GetData(MPU_GYRO_YOUTH_REG) - GYRO_Offset.Y;
		GYRO->Z = GetData(MPU_GYRO_ZOUTH_REG) - GYRO_Offset.Z;
	}
	else
	{
		ACC->X = GetData(MPU_ACCEL_XOUTH_REG);	// ��ȡ���ٶȼ�ԭʼ���ݲ���һ��
		ACC->Y = GetData(MPU_ACCEL_YOUTH_REG);
		ACC->Z = GetData(MPU_ACCEL_ZOUTH_REG);
		
		GYRO->X = GetData(MPU_GYRO_XOUTH_REG);	// ��ȡ������ԭʼ���ݲ���һ��
		GYRO->Y = GetData(MPU_GYRO_YOUTH_REG);
		GYRO->Z = GetData(MPU_GYRO_ZOUTH_REG);
	}
}

/*
 * ��������GetData
 * ����  �����16λ����
 * ����  ��REG_Address �Ĵ�����ַ
 * ���  �����ؼĴ�������
 * ����  ���ⲿ����
 */
int16 GetData(uchar REG_Address)
{
	uchar H, L;

	H = MPU6050_RD(REG_Address);
	L = MPU6050_RD(REG_Address+1);
	
	return ((H<<8)|L);   //�ϳ�����
}


#define AcceRatio 	16384.0f
#define GyroRatio 	16.4f
#define Gyro_Gr		0.0010653	// ���ٶȱ�ɻ���	�˲�����Ӧ����2000��ÿ��
#define ACC_FILTER_NUM 5		// ���ٶȼ��˲����
#define GYRO_FILTER_NUM 4		// �������˲����
int32 ACC_X_BUF[ACC_FILTER_NUM], ACC_Y_BUF[ACC_FILTER_NUM], ACC_Z_BUF[ACC_FILTER_NUM];	// �˲���������
int32 GYRO_X_BUF[GYRO_FILTER_NUM], GYRO_Y_BUF[GYRO_FILTER_NUM], GYRO_Z_BUF[GYRO_FILTER_NUM];
/*
 * ��������Data_Filter
 * ����  �����ݻ����˲�
 * ����  ����
 * ���  ����
 * ����  ���ⲿ����
 */
void Data_Filter(void)	// �����˲�
{
	uchar i;
	int64 temp1 = 0, temp2 = 0, temp3 = 0, temp4 = 0, temp5 = 0, temp6 = 0;
	
	ACC_X_BUF[0] = ACC.X;	// ���»�����������
	ACC_Y_BUF[0] = ACC.Y;
	ACC_Z_BUF[0] = ACC.Z;
	GYRO_X_BUF[0] = GYRO.X;
	GYRO_Y_BUF[0] = GYRO.Y;
	GYRO_Z_BUF[0] = GYRO.Z;
	
	for(i=0;i<ACC_FILTER_NUM;i++)
	{
		temp1 += ACC_X_BUF[i];
		temp2 += ACC_Y_BUF[i];
		temp3 += ACC_Z_BUF[i];
		
	}
	for(i=0;i<GYRO_FILTER_NUM;i++)
	{
		temp4 += GYRO_X_BUF[i];
		temp5 += GYRO_Y_BUF[i];
		temp6 += GYRO_Z_BUF[i];
	}
	
	ACC_Real.X = temp1 / ACC_FILTER_NUM / AcceRatio * 9.8;
	ACC_Real.Y = temp2 / ACC_FILTER_NUM / AcceRatio * 9.8;
	ACC_Real.Z = temp3 / ACC_FILTER_NUM / AcceRatio * 9.8;
	GYRO_Real.X = temp4 / GYRO_FILTER_NUM / GyroRatio;
	GYRO_Real.Y = temp5 / GYRO_FILTER_NUM / GyroRatio;
	GYRO_Real.Z = temp6 / GYRO_FILTER_NUM / GyroRatio;
	
	for(i = 0; i < ACC_FILTER_NUM - 1; i++)
	{
		ACC_X_BUF[ACC_FILTER_NUM-1-i] = ACC_X_BUF[ACC_FILTER_NUM-2-i];
		ACC_Y_BUF[ACC_FILTER_NUM-1-i] = ACC_Y_BUF[ACC_FILTER_NUM-2-i];
		ACC_Z_BUF[ACC_FILTER_NUM-1-i] = ACC_Z_BUF[ACC_FILTER_NUM-2-i];
		
	}
	for(i = 0; i < GYRO_FILTER_NUM - 1; i++)
	{
		GYRO_X_BUF[GYRO_FILTER_NUM-1-i] = GYRO_X_BUF[GYRO_FILTER_NUM-2-i];
		GYRO_Y_BUF[GYRO_FILTER_NUM-1-i] = GYRO_Y_BUF[GYRO_FILTER_NUM-2-i];
		GYRO_Z_BUF[GYRO_FILTER_NUM-1-i] = GYRO_Z_BUF[GYRO_FILTER_NUM-2-i];
	}
}

/*
 * ��������Get_Attitude
 * ����  ����̬����
 * ����  ����
 * ���  ����
 * ����  ���ⲿ����
 */
void Get_Attitude(void)	// ��̬����
{
	IMUupdate(GYRO_Real.X*Gyro_Gr*GyroRatio, 
			  GYRO_Real.Y*Gyro_Gr*GyroRatio, 
			  GYRO_Real.Z*Gyro_Gr*GyroRatio, 
			  ACC_Real.X * AcceRatio / 9.8, 
			  ACC_Real.Y * AcceRatio / 9.8, 
			  ACC_Real.Z * AcceRatio / 9.8);	// ��̬�����ŷ����
}


//===============================��Ԫ��============================================
#define Kp 1.6f //10.0f             	// proportional gain governs rate of convergence to accelerometer/magnetometer
#define Ki 0.001f//1.2f // //0.008f  	// integral gain governs rate of convergence of gyroscope biases
#define halfT 0.005f                   	// half the sample period�������ڵ�һ��
float q0 = 1, q1 = 0, q2 = 0, q3 = 0; 	// quaternion elements representing the estimated orientation
float exInt = 0, eyInt = 0, ezInt = 0; 	// scaled integral error
/*
 * ��������IMUupdate
 * ����  ����Ԫ�ؽ���ŷ����
 * ����  �������� ���ٶȼ�
 * ���  ����
 * ����  ���ڲ�����
 */
void IMUupdate(float gx, float gy, float gz, float ax, float ay, float az)
{
	float norm;
	float vx, vy, vz;
	float ex, ey, ez;

	// �Ȱ���Щ�õõ���ֵ���
	float q0q0 = q0*q0;
	float q0q1 = q0*q1;
	float q0q2 = q0*q2;
	float q1q1 = q1*q1;
	float q1q3 = q1*q3;
	float q2q2 = q2*q2;
	float q2q3 = q2*q3;
	float q3q3 = q3*q3;

	if (ax*ay*az == 0)
	{
		return;
	}
		
	norm = sqrt(ax*ax + ay*ay + az*az);	// acc���ݹ�һ��
	ax = ax / norm;
	ay = ay / norm;
	az = az / norm;

	// estimated direction of gravity and flux (v and w)	�����������������/��Ǩ
	vx = 2*(q1q3 - q0q2);									// ��Ԫ����xyz�ı�ʾ
	vy = 2*(q0q1 + q2q3);
	vz = q0q0 - q1q1 - q2q2 + q3q3 ;

	// error is sum of cross product between reference direction of fields and direction measured by sensors
	ex = (ay*vz - az*vy) ;		// �������������õ���־������
	ey = (az*vx - ax*vz) ;
	ez = (ax*vy - ay*vx) ;

	exInt = exInt + ex * Ki;	// �������л���
	eyInt = eyInt + ey * Ki;
	ezInt = ezInt + ez * Ki;

	// adjusted gyroscope measurements
	gx = gx + Kp*ex + exInt;	// �����PI�󲹳��������ǣ����������Ư��
	gy = gy + Kp*ey + eyInt;
	gz = gz + Kp*ez + ezInt;	// �����gz����û�й۲��߽��н��������Ư�ƣ����ֳ����ľ��ǻ����������Լ�

	// integrate quaternion rate and normalise	// ��Ԫ�ص�΢�ַ���
	q0 = q0 + (-q1*gx - q2*gy - q3*gz)*halfT;
	q1 = q1 + (q0*gx + q2*gz - q3*gy)*halfT;
	q2 = q2 + (q0*gy - q1*gz + q3*gx)*halfT;
	q3 = q3 + (q0*gz + q1*gy - q2*gx)*halfT;

	// normalise quaternion
	norm = sqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);
	q0 = q0 / norm;
	q1 = q1 / norm;
	q2 = q2 / norm;
	q3 = q3 / norm;
	
	Attitude_Angle.Y = asin(-2*q1*q3 + 2*q0*q2) * 57.3; // pitch
//	Attitude_Angle.X = atan2(2*q2*q3 + 2*q0*q1, -2*q1*q1 - 2*q2*q2 + 1)*57.3; // roll
//	Attitude_Angle.Z = atan2(2*q1*q2 + 2*q0*q3, -2*q2*q2 - 2*q3*q3 + 1)*57.3; // yaw
	Attitude_Angle.Z = 0;
}

