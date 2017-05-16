#ifndef __BALANCE_H__
#define __BALANCE_H__
#include "common.h"
#include "include.h"

#define Zero_Angle 30.5f	// ��ɫ���
//#define Zero_Angle 22.0f	// ��ɫ���

extern S_FLOAT_XYZ 
	GYRO_Real,		// ������ת���������
	ACC_Real,		// ���ٶȼ�ת���������
	Attitude_Angle,	// ��ǰ�Ƕ� 
	Last_Angle,		// �ϴνǶ�
	Target_Angle,	// Ŀ��Ƕ�
	Now_Ang_Vel;	// ��ǰ���ٶ�
	

extern S_INT16_XYZ
	GYRO,			// ������ԭʼ����
	GYRO_Offset,	// ��������Ʈ
	GYRO_Last,		// �������ϴ�����
	ACC, 			// ���ٶȼ�����
	ACC_Offset,		// ���ٶȼ���Ʈ
	ACC_Last;		// ���ٶȼ��ϴ�����
extern S_INT32_XYZ
	Tar_Ang_Vel,	// Ŀ����ٶ�
	Tar_Ang_Vel_Last;	// �ϴ�Ŀ����ٶ�

extern int32 
	Speed_Now,		// ��ǰʵ���ٶ�
	Speed_Min,		// ������С�ٶ�
	Speed_Set, 		// Ŀ���趨�ٶ�
	Vel_Set,		// Ŀ��ת����ٶ�
	Direct_Parameter,
	Radius;

extern uchar Point;
extern int32 Difference;

extern char Speed_Flag, Angle_Flag, Ang_Velocity_Flag, Direct_Flag;

void Balance_Control(void);

#endif
