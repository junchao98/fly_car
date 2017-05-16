#include "PID.h"

PID MOTOR_PID, Angle_PID, Ang_Vel_PID, Direct_PID, Turn_PID, Distance_PID;	//定义舵机和电机的PID参数结构体
float MOTOR[4]   = {-80, 0, -20, 1000};	// 速度环PID	最后一项为积分限幅
float Angle[4]   = {0.4, 0, 0.1, 1000};		// 角度环PID
float Ang_Vel[4] = {0.4, 0.03, 0, 1000};		// 角速度环PID
float Direct[4]  = {0.022, 0, 0.04, 1000};	// 转向环PID
float Turn[4] = {100, 8, 100, 600};		// 转向外环动态PID	加权算法用
//float Turn[4] = {20, 20, 7, 500};		// 转向外环动态PID	面积算法用

// PID参数初始化
void PID_Parameter_Init(PID *sptr)
{
	sptr->SumError  = 0;
	sptr->LastError = 0;	//Error[-1]
	sptr->PrevError = 0;	//Error[-2]	
	sptr->LastData  = 0;
}


// 位置式动态PID控制
int32 PlacePID_Control(PID *sprt, float *PID, int32 NowPiont, int32 SetPoint)
{
	//定义为寄存器变量，只能用于整型和字符型变量，提高运算速度
	int32 iError,	//当前误差
		  Actual;	//最后得出的实际输出值
	float Kp;		//动态P
	
	iError = SetPoint - NowPiont;	//计算当前误差
	sprt->SumError += iError;
	if (sprt->SumError >= PID[KF])
	{
		sprt->SumError = PID[KF];
	}
	else if (sprt->SumError <= PID[KF])
	{
		sprt->SumError = -PID[KF];
	}
	Kp = 1.0 * (iError*iError) / PID[KP] + PID[KI];	//P值与差值成二次函数关系，此处P和I不是PID参数，而是动态PID参数，要注意！！！
	
	Actual = Kp * iError
		   + PID[KD] * ((1*iError + 0*sprt->LastError) - sprt->LastError);//只用PD
	sprt->LastError = 1*iError + 0*sprt->LastError;		//更新上次误差
	
	Actual = range_protect(Actual, -600, 600);
	
//	if (Speed_Now > 130)
//	{
//		Actual += 2 * sprt->SumError;
//	}
	
	return Actual;
}

// 位置式PID控制
int32 PID_Realize(PID *sptr, float *PID, int32 NowData, int32 Point)
{
	//当前误差，定义为寄存器变量，只能用于整型和字符型变量，提高运算速度
	int32 iError,	// 当前误差
		 Realize;	// 最后得出的实际增量
	
	iError = Point - NowData;	// 计算当前误差
	sptr->SumError += iError;	// 误差积分
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
	sptr->PrevError = sptr->LastError;	// 更新前次误差
	sptr->LastError = iError;		  	// 更新上次误差
	sptr->LastData  = NowData;			// 更新上次数据
	
	return Realize;	// 返回实际值
}

// 增量式PID电机控制
int32 PID_Increase(PID *sptr, float *PID, int32 NowData, int32 Point)
{
	//当前误差，定义为寄存器变量，只能用于整型和字符型变量，提高运算速度
	int32 iError,	//当前误差
		Increase;	//最后得出的实际增量
	
	iError = Point - NowData;	// 计算当前误差
		
	Increase =  PID[KP] * (iError - sptr->LastError)
			  + PID[KI] * iError
			  + PID[KD] * (iError - 2 * sptr->LastError + sptr->PrevError);
	
	sptr->PrevError = sptr->LastError;	// 更新前次误差
	sptr->LastError = iError;		  	// 更新上次误差
	sptr->LastData  = NowData;			// 更新上次数据
	
	return Increase;	// 返回增量
}
