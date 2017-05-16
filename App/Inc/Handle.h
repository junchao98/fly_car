#ifndef __HANDLE_H__
#define __HANDLE_H__
#include "common.h"
#include "include.h"

//外部变量声明
extern uchar Foresight;
extern uchar Annulus_Flag;	// 环路标志位
extern uchar Annulus_Mode;	// 环路状态标志位
extern int16 Annulus_Delay;	// 环路延时
extern uchar Out_Side;
extern uchar Line_Count;
extern uchar Starting_Line_Flag;
extern uchar Left_Max, Right_Min;
extern uchar Left_Line[62];
extern uchar Right_Line[62];
extern uchar Mid_Line[62];
extern uchar Left_Add_Line[62];
extern uchar Right_Add_Line[62];
extern int32 Area_Left, Area_Right;

//函数声明
void Image_Para_Init(void);
void Annulus_Control(void);
void Annulus_Handle(uchar *data);
void Image_Handle(uchar *data);
int32 Area_Calculate(void);
uchar Point_Average(void);
float Slope_Weight(uchar *Mid);
uchar First_Line_Handle(uchar *data);
uchar Traversal_Left(uchar i, uchar *data, uchar *Mid, uchar Left_Min, uchar Right_Max);
uchar Traversal_Right(uchar i, uchar *data, uchar *Mid, uchar Left_Min, uchar Right_Max);
uchar Traversal_Left_Line(uchar i, uchar *data, uchar *Left_Line, uchar *Right_Line);
uchar Traversal_Right_Line(uchar i, uchar *data, uchar *Left_Line, uchar *Right_Line);
void Traversal_Mid_Line(uchar i, uchar *data, uchar Mid, uchar Left_Min, uchar Right_Max,
		uchar *Left_Line, uchar *Right_Line, uchar *Left_Add_Line, uchar *Right_Add_Line);
void Line_Repair(uchar Start, uchar Stop, uchar *data, uchar *Line, uchar *Line_Add, uchar *Add_Flag);
void Mid_Line_Repair(uchar count, uchar *data);

float Calculate_Angle(uchar Point_1, uchar Point_2, uchar Point_3);
void Curve_Fitting(float *Ka, float *Kb, uchar *Start, uchar *Line, uchar *Add_Flag, uchar Mode);
uchar Calculate_Add(uchar i, float Ka, float Kb);
uchar Limit_Scan(uchar i, uchar *data, uchar Point);
uchar Point_Weight(void);
char Error_Transform(uchar Data, uchar Set);


void img_extract(uint8 *dst, uint8 *src, uint32 srclen);        //解压 √√√
void img_recontract(uint8 *dst, uint8 *src, uint32 srclen);     //压缩√√√
					//图像来源  //保存目标地址  //图像规格
void img_getline(uint8 *dst, uint8 *src, uint32 srclen);        //提取边界线√√√

#endif
