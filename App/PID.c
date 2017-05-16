#include "PID.h"

PID MOTOR_PID, Angle_PID, Ang_Vel_PID, Direct_PID, Turn_PID, Distance_PID;	//�������͵����PID�����ṹ��
float MOTOR[4]   = {-80, 0, -20, 1000};	// �ٶȻ�PID	���һ��Ϊ�����޷�
float Angle[4]   = {0.4, 0, 0.1, 1000};		// �ǶȻ�PID
float Ang_Vel[4] = {0.4, 0.03, 0, 1000};		// ���ٶȻ�PID
float Direct[4]  = {0.022, 0, 0.04, 1000};	// ת��PID
float Turn[4] = {100, 8, 100, 600};		// ת���⻷��̬PID	��Ȩ�㷨��
//float Turn[4] = {20, 20, 7, 500};		// ת���⻷��̬PID	����㷨��

// PID������ʼ��
void PID_Parameter_Init(PID *sptr)
{
	sptr->SumError  = 0;
	sptr->LastError = 0;	//Error[-1]
	sptr->PrevError = 0;	//Error[-2]	
	sptr->LastData  = 0;
}


// λ��ʽ��̬PID����
int32 PlacePID_Control(PID *sprt, float *PID, int32 NowPiont, int32 SetPoint)
{
	//����Ϊ�Ĵ���������ֻ���������ͺ��ַ��ͱ�������������ٶ�
	int32 iError,	//��ǰ���
		  Actual;	//���ó���ʵ�����ֵ
	float Kp;		//��̬P
	
	iError = SetPoint - NowPiont;	//���㵱ǰ���
	sprt->SumError += iError;
	if (sprt->SumError >= PID[KF])
	{
		sprt->SumError = PID[KF];
	}
	else if (sprt->SumError <= PID[KF])
	{
		sprt->SumError = -PID[KF];
	}
	Kp = 1.0 * (iError*iError) / PID[KP] + PID[KI];	//Pֵ���ֵ�ɶ��κ�����ϵ���˴�P��I����PID���������Ƕ�̬PID������Ҫע�⣡����
	
	Actual = Kp * iError
		   + PID[KD] * ((1*iError + 0*sprt->LastError) - sprt->LastError);//ֻ��PD
	sprt->LastError = 1*iError + 0*sprt->LastError;		//�����ϴ����
	
	Actual = range_protect(Actual, -600, 600);
	
//	if (Speed_Now > 130)
//	{
//		Actual += 2 * sprt->SumError;
//	}
	
	return Actual;
}

// λ��ʽPID����
int32 PID_Realize(PID *sptr, float *PID, int32 NowData, int32 Point)
{
	//��ǰ������Ϊ�Ĵ���������ֻ���������ͺ��ַ��ͱ�������������ٶ�
	int32 iError,	// ��ǰ���
		 Realize;	// ���ó���ʵ������
	
	iError = Point - NowData;	// ���㵱ǰ���
	sptr->SumError += iError;	// ������
	if (sptr->SumError >= PID[KF])
	{
		sptr->SumError = PID[KF];
	}
	else if (sptr->SumError <= -PID[KF])
	{
		sptr->SumError = -PID[KF];
	}
		
	Realize = PID[KP] * iError
			+ PID[KI] * sptr->SumError
			+ PID[KD] * (iError - sptr->LastError);
	sptr->PrevError = sptr->LastError;	// ����ǰ�����
	sptr->LastError = iError;		  	// �����ϴ����
	sptr->LastData  = NowData;			// �����ϴ�����
	
	return Realize;	// ����ʵ��ֵ
}

// ����ʽPID�������
int32 PID_Increase(PID *sptr, float *PID, int32 NowData, int32 Point)
{
	//��ǰ������Ϊ�Ĵ���������ֻ���������ͺ��ַ��ͱ�������������ٶ�
	int32 iError,	//��ǰ���
		Increase;	//���ó���ʵ������
	
	iError = Point - NowData;	// ���㵱ǰ���
		
	Increase =  PID[KP] * (iError - sptr->LastError)
			  + PID[KI] * iError
			  + PID[KD] * (iError - 2 * sptr->LastError + sptr->PrevError);
	
	sptr->PrevError = sptr->LastError;	// ����ǰ�����
	sptr->LastError = iError;		  	// �����ϴ����
	sptr->LastData  = NowData;			// �����ϴ�����
	
	return Increase;	// ��������
}
