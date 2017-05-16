#include "Handle.h"

uint8 colour[2] = {255, 0},black=1,white=0; //0 �� 1 �ֱ��Ӧ����ɫ
//ע��ɽ�������ͷ 0 ��ʾ ��ɫ��1��ʾ ��ɫ
uchar Weight[60] = { 5,  6,  8,  9, 11, 12, 14, 15, 17, 18,	// 50
					20, 21, 22, 23, 24, 25, 26, 27, 28, 29,	// 40
				    27, 27, 26, 25, 24, 23, 22, 21, 20, 19,	// 30
				    4,  3,  3,  2,  1,    1,  1,  1,  1,  1,	// 20
					1,  1,  1,  1,  1,    1,  1,  1,  1,  1,
					1,  1,  1,  1,  1,    1,  1,  1,  1,  1};	//��Ȩƽ������

uchar Left_Line[62], Right_Line[62], Mid_Line[62];	// ԭʼ���ұ߽�����
uchar Left_Add_Line[62], Right_Add_Line[62];		// ���ұ߽粹������
uchar Left_Add_Flag[62], Right_Add_Flag[62];		// ���ұ߽粹�߱�־λ
uchar Width_Real[62];	// ʵ���������
uchar Width_Add[62];	// �����������
uchar Width_Min;		// ��С�������

uchar Foresight;	// ǰհ�������ٶȿ���
uchar Out_Side = 0;	// ���߿���
uchar Line_Count;	// ��¼�ɹ�ʶ�𵽵���������

uchar Left_Add_Start, Right_Add_Start;	// ���Ҳ�����ʼ������
uchar Left_Add_Stop, Right_Add_Stop;	// ���Ҳ��߽���������
float Left_Ka = 0, Right_Ka = 0;
float Left_Kb = 0, Right_Kb = 0;	// ��С���˷�����

uchar Left_Hazard_Flag, Right_Hazard_Flag;	// �����ϰ����־
uchar Left_Max, Right_Min;
int32 Area_Left = 0, Area_Right = 0;	// ���Ҳ������

uchar Starting_Line_Flag = 0;	// �����߱�־λ

/************** ��·��ر��� *************/
uchar Annulus_Count[10] = {0, 1, 0, 1, 1, 1, 0, 0, 1, 1};	// 1��·��0�һ�·
uchar Annulus_Times = 0;	// ��·����
uchar Annulus_Flag = 0;
uchar Annulus_Left = 0;
uchar Annulus_Right = 0;
int16 Annulus_Delay = 0;
uchar Annulus_Mode = 0;		// ׼������·״̬
/************** ��·��ر��� *************/

void Annulus_Control(void)
{
	Annulus_Left = Annulus_Count[Annulus_Times++];
	Annulus_Right = !Annulus_Left;
	
	Annulus_Mode = 1;	// ��ʼ���뻷·
	Annulus_Flag = 1;	// ��·��־λ��λ
	Annulus_Delay = 500;	// ������ʱ1s
	
	if (Annulus_Times >= 4)
	{
		Annulus_Times = 0;
	}
}

/****************** ���㷨 ******************/

/*
*	ͼ���㷨������ʼ��
*
*	˵������Ӱ���һ�����⴦��
*/
void Image_Para_Init(void)
{	
	Mid_Line[61] = 40;
	Left_Line[61] = 1;
	Right_Line[61] = 79;
	Left_Add_Line[61] = 1;
	Right_Add_Line[61] = 79;
	Width_Real[61] = 78;
	Width_Add[61] = 78;
	
	Annulus_Left = 1;	// ��λ��·
	Annulus_Right = 0; 
}

/*
*	��·ͼ�����㷨
*
*	˵������·ר�ã���������Ԫ��
*/
void Annulus_Handle(uchar *data)
{
	uchar i;	// ������
	uchar res;
	
	Line_Count = 0;	// ����������λ
	
	Left_Add_Start = 0;		// ��λ������ʼ������
	Right_Add_Start = 0;
	Left_Add_Stop = 0;
	Right_Add_Stop = 0;
	
	/***************************** ��һ�����⴦�� *****************************/
	res = First_Line_Handle(data);
	if (res == 0)
	{
		Out_Side++;	// ����
		return;
	}
	/*************************** ��һ�����⴦����� ***************************/
	
	
	/**************************** ��·����״̬�ж� ****************************/
	if (Annulus_Left)	// ��·
	{
		if (Right_Add_Flag[59])		// �������Ҫ����
		{
			if (Annulus_Mode == 1)	// �ϴ�״̬Ϊ��ʼ���뻷·
			{
				Annulus_Mode = 2;	// ��ǰ״̬Ϊ���뻷·��
			}
			if (Annulus_Mode == 3)	// �ϴ�״̬Ϊ�ѳɹ����뻷·
			{
				Annulus_Mode = 4;	// ��ǰ״̬Ϊ����·��
			}
		}
		else						// ����в���Ҫ����
		{
			if (Annulus_Mode == 2)	// �ϴ�״̬Ϊ���뻷·��
			{
				Annulus_Mode = 3;	// ��ǰ״̬Ϊ�ѳɹ����뻷·��׼������·
			}
			if (Annulus_Mode == 4)	// �ϴ�״̬Ϊ����·��
			{
				if (!Annulus_Delay)	// ���ٱ���1s��·ר���㷨����ֹ����
				{
					Annulus_Mode = 0;	// ����״̬Ϊ�ɹ�����·��׼���´ν��뻷·
					Annulus_Flag = 0;	// �����·״̬��־λ
				}
			}
		}
	}
	else	// �һ�·
	{
		if (Left_Add_Flag[59])		// �������Ҫ����
		{
			if (Annulus_Mode == 1)	// �ϴ�״̬Ϊ��ʼ���뻷·
			{
				Annulus_Mode = 2;	// ��ǰ״̬Ϊ���뻷·��
			}
			if (Annulus_Mode == 3)	// �ϴ�״̬Ϊ�ѳɹ����뻷·
			{
				Annulus_Mode = 4;	// ��ǰ״̬Ϊ����·��
			}
		}
		else						// ����в���Ҫ����
		{
			if (Annulus_Mode == 2)	// �ϴ�״̬Ϊ���뻷·��
			{
				Annulus_Mode = 3;	// ��ǰ״̬Ϊ�ѳɹ����뻷·��׼������·
			}
			if (Annulus_Mode == 4)	// �ϴ�״̬Ϊ����·��
			{
				if (!Annulus_Delay)	// ����״̬Ϊ�ɹ�����·��׼���´ν��뻷·
				{
					Annulus_Mode = 0;	// ����״̬Ϊ�ɹ�����·��׼���´ν��뻷·
					Annulus_Flag = 0;	// �����·״̬��־λ
				}
			}
		}
	}
	/************************** ��·����״̬�жϽ��� **************************/
	
	for (i = 59; i >= 21;)
	{
		i -= 2;
		
		if (!data[i*80 + Mid_Line[i+2]])//ǰ2���е��ڱ���Ϊ�ڵ㣬��������
		{ 
			i += 2;
			break;
		}
		else
		{
			Traversal_Mid_Line(i, data, Mid_Line[i+2], 1, 79, Left_Line, Right_Line, Left_Add_Line, Right_Add_Line);
			/************************** ���߼�⿪ʼ **************************/
			if (Width_Real[i] >= Width_Min)	// ������ȱ��,���뻷·
			{	
				if (Annulus_Left)	// ��·
				{
					Left_Add_Line[i] = Left_Line[i];
					Right_Add_Line[i] = Left_Line[i] + (Width_Min-3);
					
				}
				else				// �һ�·
				{
					Right_Add_Line[i] = Right_Line[i];
					Left_Add_Line[i] = Right_Line[i] - (Width_Min-3);
				}
			}
			/************************** ���߼����� **************************/
			Mid_Line[i] = (Right_Add_Line[i] + Left_Add_Line[i]) / 2;
			Width_Add[i] = Right_Add_Line[i] - Left_Add_Line[i];	
			if (Width_Add[i] < Width_Min)
			{
				Width_Min = Width_Add[i];
			}
		}
	}
	Line_Count = i;
	
	for (i = 61; i >= Line_Count;)
	{
		i -= 2;
		/**//************************* ��λ����ʾ�߽� *************************/
		/**/data[i*80 + Left_Add_Line[i] + 2] = 0;	// ��λ����ʾ���ߺ����߽磬����ʱ����
		/**/data[i*80 + Right_Add_Line[i] - 2] = 0;	// ��λ����ʾ���ߺ���ұ߽磬����ʱ����
		/**/data[i*80 + Mid_Line[i]] = 0;			// ��λ����ʾ���ߣ�����ʱ����
//		/**/data[i*80 + Left_Line[i] + 2] = 0;		// ��λ����ʾԭʼ��߽磬����ʱ����
//		/**/data[i*80 + Right_Line[i] - 2] = 0;		// ��λ����ʾԭʼ�ұ߽磬����ʱ����
		/**//************************* ��λ����ʾ�߽� *************************/
	}
}

/*
*	ͼ�����㷨
*
*	˵����������ͨͼ�񣬰���ʮ�֡��ϰ�
*/
void Image_Handle(uchar *data)
{
	uchar i;	// ������
	uchar res;	// ���ڽ��״̬�ж�
	float Result;	// ���ڽ��״̬�ж�

	uchar Mid_Left, Mid_Right;

	
	Line_Count = 0;	// ����������λ
	
	Annulus_Flag = 0;		// ��λ��·��־λ
	
	Left_Hazard_Flag = 0;	// ��λ�����ϰ����־λ
	Right_Hazard_Flag = 0;
	
	Left_Add_Start = 0;		// ��λ������ʼ������
	Right_Add_Start = 0;
	Left_Add_Stop = 0;
	Right_Add_Stop = 0;
	
	/***************************** ��һ�����⴦�� *****************************/
	
	res = First_Line_Handle(data);
	if (res == 0)
	{
		Out_Side = 1;	// ����
		return;
	}
	Out_Side = 0;
	Line_Count = 59;
	
	/*************************** ��һ�����⴦����� ***************************/
	
	for (i = 59; i >= 21;)	// ������ǰ40��ͼ�񣬸��к������20������
	{
		i -= 2;	// ���д�����С��Ƭ������
		
		if (!data[i*80 + Mid_Line[i+2]])//ǰ2���е��ڱ���Ϊ�ڵ㣬����������������Ҳ�����ǻ�·
		{
			if (Left_Add_Start && !Left_Add_Stop && Right_Add_Start && !Right_Add_Stop)	//  ���߶��в��ߣ���û�в��߽������������˻�·
			{ 
				if ((Left_Add_Start >= i+6) && (Right_Add_Start >= i+6) && (Width_Add[i+2]>=18))	// ��ֹ����
				{
					if (Left_Ka < 0 && Right_Ka > 0)
					{
						Annulus_Control();	// ������·ר���㷨
					}
				}
			}
			break;
		}
		else	// ʹ��ǰ2���е�������ɨ��߽�
		{
			Traversal_Mid_Line(i, data, Mid_Line[i+2], 1, 79, Left_Line, Right_Line, Left_Add_Line, Right_Add_Line);
		}
			
		/**************************** ���߼�⿪ʼ ****************************/
		
		if (Width_Real[i] > Width_Min+1)	// ������ȱ��������ʮ�ֻ�·
		{
			// ʹ�������㷨����ͻ����⣬�ž�ʮ��©��
			Result = Calculate_Angle(Left_Add_Line[i+4], Left_Add_Line[i+2], Left_Add_Line[i]);	// ͻ�����
			if (Result <= -1.4)	// -1��135��
			{
				Left_Add_Flag[i] = 2;
			}
			Result = Calculate_Angle(Right_Add_Line[i+4], Right_Add_Line[i+2], Right_Add_Line[i]);	// ͻ�����
			if (Result >= 1.4)	// 1��135��
			{
				Right_Add_Flag[i] = 2;
			}
			if (i >= 55 || (Width_Real[i] > 1+Width_Real[i+2]))	// ����Ͻ�
			{
				if (Left_Add_Line[i] <  Left_Add_Line[i+2])
				{
					if (!Left_Add_Flag[i])
					{
						Left_Add_Flag[i] = 1;	// ǿ���϶�Ϊ��Ҫ����
					}
				}
				if (Right_Add_Line[i] > Right_Add_Line[i+2])
				{
					if (!Right_Add_Flag[i])
					{
						Right_Add_Flag[i] = 1;	// ǿ���϶�Ϊ��Ҫ����
					}
				}
			}
			
			if (Left_Add_Flag[i] || Right_Add_Flag[i])
			{
				if (Left_Add_Stop || Right_Add_Stop)
				{
					break;
				}
			}
		}
		
		/**************************** ���߼����� ****************************/
		
		
		/**************************** �ϰ���⿪ʼ ****************************/
		
		else
		{
			if (i != 59 && i > 30)
			{
				if (Left_Add_Line[i] > Left_Add_Line[i+2] + 5 && !Left_Add_Start)	// ������ǰһ����߽��нϴ�ͻ����û�в���
				{
					res = Traversal_Left(i, data, &Mid_Left, Left_Add_Line[i+2], Right_Add_Line[i+2]);	// ������࿪ʼ�����߽磬�����������
					if (res < Width_Add[i] && res > 10)	// ���������ȱ����߷�С��������̫С��˵���������ϰ����������µ�����
					{
						Left_Hazard_Flag = i;	// �ϰ����־λ��λ
					}
				}
				if (Right_Add_Line[i] > Right_Add_Line[i+2] + 5 && !Right_Add_Start)	// ������ǰһ���ұ߽��нϴ�ͻ����û�в���
				{
					res = Traversal_Right(i, data, &Mid_Right, Left_Add_Line[i+2], Right_Add_Line[i+2]);	// �����Ҳ࿪ʼ�����߽磬�����������
					if (res < Width_Add[i] && res > 10)	// �Ҳ�������ȱ����߷�С��������̫С��˵���������ϰ����������µ�����
					{
						Right_Hazard_Flag = i;	// �ϰ����־λ��λ
					}
				}
			}
		}
		
		/**************************** �ϰ������� ****************************/
		
		
		/*************************** ��һ�ֲ��߿�ʼ ***************************/
		
		if (Left_Add_Flag[i])	// �����Ҫ����
		{
			if (i >= 55)	// ǰ���в��߲���
			{
				Left_Add_Line[i] = Left_Add_Line[i+2];	// ʹ��ǰһ����߽�ֱ�����ϲ���
				if (!Left_Add_Start)
				{
					Left_Add_Start = i;	// ��¼���߿�ʼ��
					Left_Ka = 0;
					Left_Kb = Left_Add_Line[i];
				}
				
			}
			else
			{
				if (!Left_Add_Start)	// ֮ǰû�в���
				{
					Left_Add_Start = i;	// ��¼��ಹ�߿�ʼ��
					Curve_Fitting(&Left_Ka, &Left_Kb, &Left_Add_Start, Left_Add_Line, Left_Add_Flag, 1);	// ʹ�����㷨���ֱ��
				}
				Left_Add_Line[i] = Calculate_Add(i, Left_Ka, Left_Kb);	// �������
			}
		}
		else
		{
			if (Left_Add_Start)	// �Ѿ���ʼ����
			{
				Result = Calculate_Angle(Left_Line[i+4], Left_Line[i+2], Left_Line[i]);	// ͻ�����
				if (Result <= -1.6)	// -1��135
				{
					Left_Add_Stop = i;	// ��¼��ಹ�߽�����
				}
			}
		}
		
		if (Right_Add_Flag[i])	// �Ҳ���Ҫ����
		{
			if (i >= 55)	// ǰ���в��߲���
			{
				Right_Add_Line[i] = Right_Add_Line[i+2];	// ʹ��ǰһ���ұ߽�ֱ�����ϲ���
				if (!Right_Add_Start)
				{
					Right_Add_Start = i;	// ��¼���߿�ʼ��
					Right_Ka = 0;
					Right_Kb = Right_Add_Line[i];
				}
			}
			else
			{
				if (!Right_Add_Start)	// ֮ǰû�в���
				{
					{
						Right_Add_Start = i;	// ��¼�Ҳಹ�߿�ʼ��
						Curve_Fitting(&Right_Ka, &Right_Kb, &Right_Add_Start, Right_Add_Line, Right_Add_Flag, 2);	// ʹ�����㷨���ֱ��
					}
				}
				Right_Add_Line[i] = Calculate_Add(i, Right_Ka, Right_Kb);	// �������
			}
		}
		else
		{
			if (Right_Add_Start)	// �Ѿ���ʼ����
			{
				Result = Calculate_Angle(Right_Line[i+4], Right_Line[i+2], Right_Line[i]);	// ͻ�����
				if (Result >= 1.6)	// -1��135
				{
					Right_Add_Stop = i;	// ��¼�Ҳಹ�߽�����
				}
			}
		}
		
		/*************************** ��һ�ֲ��߽��� ***************************/
		
		Width_Add[i] = Right_Add_Line[i] - Left_Add_Line[i];	// ���¼����������
		if ((Left_Add_Flag[59] && Left_Add_Start && !Left_Add_Stop) && (Right_Add_Flag[59] && Right_Add_Start && !Right_Add_Stop))
		{
			Mid_Line[i] = Mid_Line[i+2];
		}
		else
		{
			Mid_Line[i] = (Right_Add_Line[i] + Left_Add_Line[i]) / 2;	// ��������
		}
		if (Width_Add[i] < Width_Min)
		{
			Width_Min = Width_Add[i];	// ������С�������
		}
		Line_Count = i;	// ��¼�ɹ�ʶ�𵽵���������
	}
	/*************************** �ڶ��ֲ����޸���ʼ ***************************/
	if (!Annulus_Flag)	// ���ǻ�·
	{
		if (Left_Add_Start)		// ��߽���Ҫ����
		{
			Line_Repair(Left_Add_Start, Left_Add_Stop, data, Left_Line, Left_Add_Line, Left_Add_Flag);
		}
		if (Right_Add_Start)	// �ұ߽���Ҫ����
		{
			Line_Repair(Right_Add_Start, Right_Add_Stop, data, Right_Line, Right_Add_Line, Right_Add_Flag);
		}
	}
	
	/*************************** �ڶ��ֲ����޸����� ***************************/
	
	
	/****************************** �����޸���ʼ ******************************/
	
	Mid_Line_Repair(Line_Count, data);
	
	/****************************** �����޸����� ******************************/
}

/*
*	����㷨
*
*	˵���������������
*/
int32 Area_Calculate(void)
{
	char i;
	int32 Result;
	static int32 Result_Last = 0;
	
	Area_Left = 0;	// ����������
	Area_Right = 0;	// �Ҳ��������
	
		if (Line_Count <= 27)
		{
			Line_Count = 27;
		}	
	for (i = 61; i >= Line_Count;)
	{
		i -= 2;
		
		if (Mid_Line[i] == 41)
		{
			Area_Left += (41 - Left_Add_Line[i]);
			Area_Right += (Right_Add_Line[i] - 41);
		}
		else if (Right_Add_Line[i] <= 41)	// �����������
		{
			Area_Left += Width_Add[i];
		}
		else if (Left_Add_Line[i] >= 41)	// ���Ҳ�������
		{
			Area_Right += Width_Add[i];
		}
		else
		{
			Area_Left += (41 - Left_Add_Line[i]);
			Area_Right += (Right_Add_Line[i] - 41);
		}
	}
	
	Result = 80*(Area_Right - Area_Left)/(Area_Right + Area_Left);
	Result = range_protect(Result, -50, 50);
	Result = Result * 0.8 + Result_Last * 0.2;
	Result_Last = Result;
	
	return Result;
}

/*
*	����ƽ��
*
*	˵�����������⴦��ֱ��ȡ��ֵ
*/
uchar Point_Average(void)
{
	uchar i;
	int32 Sum = 0;
	static uchar Last_Point = 40;
	
	if (Out_Side || Line_Count >= 53)	// �����������ͷͼ���쳣
	{
		if (Last_Point <= 40)
		{
			Point  = 1;			// ʹ���ϴ�Ŀ���
		}
		else
		{
			Point = 79;
		}
	}
	else
	{
		if (Left_Max + 10 < Right_Min)
		{
			Point = (Left_Max + Right_Min) / 2;
		}
		else
		{
			if (Line_Count <= 31)
			{
				Line_Count = 31;
			}
			else
			{
				for (i = 61; i >= Line_Count;)
				{
					i -= 2;
					Sum += Mid_Line[i];
				}
				Point = Sum / ((61-Line_Count)/2);	// ��������ƽ��
			}
		}
		
		Point = Point*0.95 + Last_Point*0.05;	// ��ͨ�˲�
		Point = range_protect(Point, 1, 79);		// �޷�����ֹ�������
		
		/*** �ϰ�������������� ***/
//		if (Left_Hazard_Flag)			//������ϰ�������Ҫ���ߣ���ʹ����Ҳ�������Ӱ��
//		{
//			Point = Mid_Line[Left_Hazard_Flag]+6;	//ʹ���ϰ�����ֵ���һ���е���ΪĿ���
//			if (Left_Hazard_Flag < 40)
//			{
//				Point += 3;
//			}
//		}
//		else if (Right_Hazard_Flag)	//�Ҳ����ϰ�������Ҫ���ߣ���ʹ����Ҳ�������Ӱ��
//		{
//			Point = Mid_Line[Right_Hazard_Flag]-6;//ʹ���ϰ�����ֵ���һ���е���ΪĿ���
//			if (Right_Hazard_Flag < 40)
//			{
//				Point -= 5;
//			}
//		}
		Last_Point = Point;	// �����ϴ�Ŀ���
	}
	
	return Point;
}

/*
*	��С���˷�����б��
*
*	˵��������б����Ϊת�������,����Ŵ�100��
*/
int32 Least_Squares(void)
{
	uchar i;

	for (i = 61; i >= Line_Count; )
	{
		i -= 2;
	}

	return 0;
}

/*
*	б�ʼ���
*
*	˵����ʹ��б����Ϊת�������������б�ʵĵ���
*/
float Slope_Weight(uchar *Mid)
{
	float Slope;
	
	Slope = 1.0 * (Mid[Line_Count] - 40) / (60-Line_Count);
	
	return Slope;
}

/*
*	�����Ƕȼ���
*
*	˵�������ؽ��Ϊ �ң�0��180�㣬1��135�㣬2��90�㣬>= 1����Ϊֱ��ͻ��
*					 ��0��180�㣬-1��135�㣬-2��90�㣬<= -1����Ϊֱ��ͻ��
*/
float Calculate_Angle(uchar Point_1, uchar Point_2, uchar Point_3)
{
	char K1, K2;
	float Result;
	
	K1 = Point_2 - Point_1;
	K2 = Point_3 - Point_2;
	
	Result = (K2 - K1) * 0.5;
	
	return Result;
}

/*
*	���㷨��ֱ��
*
*	˵�������ֱ�� y = Ka * x + Kb   Mode == 1������߽磬Mode == 2�����ұ߽�
*/
void Curve_Fitting(float *Ka, float *Kb, uchar *Start, uchar *Line, uchar *Add_Flag, uchar Mode)
{
	*Start += 4;
	if (Add_Flag[*Start] == 2)
	{
		if (*Start <= 51)
		{
			*Start += 2;
		}
		*Ka = 1.0*(Line[*Start+4] - Line[*Start]) / 4;
		if (Mode == 2)
		{
			if (*Ka < 0)
			{
				*Ka = 0;
			}
		}
		if (Mode == 1)
		{
			if (*Ka > 0)
			{
				*Ka = 0;
			}
		}
	}
	else
	{
		*Ka = 1.0*(Line[*Start+4] - Line[*Start]) / 4;
	}
	*Kb = 1.0*Line[*Start] - (*Ka * (*Start));
}

/*
*	���㲹������
*
*	˵����ʹ�����㷨������ϳ��Ĳ�������
*/
uchar Calculate_Add(uchar i, float Ka, float Kb)	// ���㲹������
{
	float res;
	int32 Result;
	
	res = i * Ka + Kb;
	Result = range_protect((int32)res, 1, 79);
	
	return (uchar)Result;
}

/*
*	��ͷ����
*
*	˵������ĳһ�㿪ʼ��ֱ����������������Զ������
*/
uchar Limit_Scan(uchar i, uchar *data, uchar Point)
{
	for ( ; i >= 20; i--)
	{
		if (!data[80*i + Point])	// �������ڵ�
		{
			break;
		}
	}
	
	return i;	// ������Զ������
}

/*
*	��һ�����⴦��
*
*	˵������ʹ�õ�60���е���Ϊ��59��(��һ��)������ʼλ�ã��ɹ����������ұ߽��
 	����59���е㸳ֵ����60�б�����һ֡ͼ��ʹ�á������60���е��ڱ���Ϊ�ڵ㣬��
 	�ֱ�ʹ����������ұ����ķ��������߽磬��������Ƚϴ�Ľ����Ϊ��59�б߽磬
 	����Ȼ���������߽�������쳣��Ϊ���磬���緵��0
*/
uchar First_Line_Handle(uchar *data)
{
	uchar i;	// ������
	uchar Weight_Left, Weight_Right;	// �����������
	uchar Mid_Left, Mid_Right;
	
	i = 59;
	
	if (!data[i*80 + Mid_Line[61]])	// ��61���е��ڵ�59��Ϊ�ڵ�
	{
		Weight_Left = Traversal_Left(i, data, &Mid_Left, 1, 79);	// ����������߽�
		Weight_Right = Traversal_Right(i, data, &Mid_Right, 1, 79);// ���Ҳ������߽�
		if (Weight_Left >= Weight_Right && Weight_Left)	// ��������ȴ�������������Ҳ�Ϊ0
		{
			Traversal_Left_Line(i, data, Left_Line, Right_Line);	// ʹ���������ȡ�����߽�
		}
		else if (Weight_Left < Weight_Right && Weight_Right)
		{
			Traversal_Right_Line(i, data, Left_Line, Right_Line);	// ʹ���ұ�����ȡ�����߽�
		}
		else	// ˵��û�鵽
		{
			return 0;
		}
	}
	else
	{
		Traversal_Mid_Line(i, data, Mid_Line[i+2], 1, 79, Left_Line, Right_Line, Left_Add_Line, Right_Add_Line);	// ��ǰһ���е�������ɨ��
	}
	
	Left_Line[61] = Left_Line[59];
	Right_Line[61] = Right_Line[59];
	Left_Add_Line[61] = Left_Add_Line[59];
	Right_Add_Line[61] = Right_Add_Line[59];
	if (Left_Add_Flag[59] && Right_Add_Flag[59])
	{
		Mid_Line[59] = Mid_Line[61];
	}
	else
	{
		Mid_Line[59] = (Right_Line[59] + Left_Line[59]) / 2;
		Mid_Line[61] = Mid_Line[59];	// ���µ�60�������е㣬������һ֡ͼ��ʹ��
	}
	Width_Real[61] = Width_Real[59];
	Width_Add[61] = Width_Add[59];
	Width_Min = Width_Add[59];
	
	return 1;
}

/*
*	����࿪ʼ�����߽磬�����������
*
*	˵����������������Ϊ̽������ʹ�ã�������������ȣ�������߽�����
*/
uchar Traversal_Left(uchar i, uchar *data, uchar *Mid, uchar Left_Min, uchar Right_Max)
{
	uchar j, White_Flag = 0;
	uchar Left_Line = Left_Min, Right_Line = Right_Max;
	
	for (j = Left_Min; j <= Right_Max; j++)	// �߽����� 1��79
	{
		if (!White_Flag)	// �Ȳ�����߽�
		{
			if (data[i*80 + j])	// ��⵽�׵�
			{
				Left_Line = j;	// ��¼��ǰjֵΪ������߽�
				White_Flag = 1;	// ��߽����ҵ��������ұ߽�
				
				continue;	// ��������ѭ���������´�ѭ��
			}
		}
		else
		{
			if (!data[i*80 + j])//���ڵ�
			{
				Right_Line = j-1;//��¼��ǰjֵΪ�����ұ߽�
				
				break;	// ���ұ߽綼�ҵ�������ѭ��
			}
		}
	}
	
	if (!White_Flag)	// δ�ҵ����ұ߽�
	{
		return 0;
	}
	else
	{
		*Mid = (Right_Line + Left_Line) / 2;
		
		return (Right_Line - Left_Line);
	}
}

/*
*	���Ҳ࿪ʼ�����߽磬�����������
*
*	˵����������������Ϊ̽������ʹ�ã�������������ȣ�������߽�����
*/
uchar Traversal_Right(uchar i, uchar *data, uchar *Mid, uchar Left_Min, uchar Right_Max)
{
	uchar j, White_Flag = 0;
	uchar Left_Line = Left_Min, Right_Line = Right_Max;
	
	for (j = Right_Max; j >= Left_Min; j--)	// �߽����� 1��79
	{
		if (!White_Flag)	// �Ȳ����ұ߽�
		{
			if (data[i*80 + j])	// ��⵽�׵�
			{
				Right_Line = j;	// ��¼��ǰjֵΪ�����ұ߽�
				White_Flag = 1;	// �ұ߽����ҵ���������߽�
				
				continue;	// ��������ѭ���������´�ѭ��
			}
		}
		else
		{
			if (!data[i*80 + j])	//���ڵ�
			{
				Left_Line = j+1;	//��¼��ǰjֵΪ������߽�
				
				break;	// ���ұ߽綼�ҵ�������ѭ��
			}
		}
	}
	
	if (!White_Flag)	// δ�ҵ����ұ߽�
	{
		return 0;
	}
	else
	{
		*Mid = (Right_Line + Left_Line) / 2;
			
		return (Right_Line - Left_Line);
	}
}

/*
*	����࿪ʼ�����߽磬���������߽磬����1�ɹ� 0ʧ��
*
*	˵����������ʹ�ú󽫱���߽�����
*/
uchar Traversal_Left_Line(uchar i, uchar *data, uchar *Left_Line, uchar *Right_Line)
{
	uchar j, White_Flag = 0;
	
	Left_Line[i] = 1;
	Right_Line[i] = 79;
	
	for (j = 1; j < 80; j++)	// �߽����� 1��79
	{
		if (!White_Flag)	// �Ȳ�����߽�
		{
			if (data[i*80 + j])	// ��⵽�׵�
			{
				Left_Line[i] = j;	// ��¼��ǰjֵΪ������߽�
				White_Flag = 1;		// ��߽����ҵ��������ұ߽�
				
				continue;	// ��������ѭ���������´�ѭ��
			}
		}
		else
		{
			if (!data[i*80 + j])	//���ڵ�
			{
				Right_Line[i] = j-1;//��¼��ǰjֵΪ�����ұ߽�
				
				break;	// ���ұ߽綼�ҵ�������ѭ��
			}
		}
	}
	if (White_Flag)
	{
		Left_Add_Line[i] = Left_Line[i];
		Right_Add_Line[i] = Right_Line[i];
		Width_Real[i] = Right_Line[i] - Left_Line[i];
		Width_Add[i] = Width_Real[i];
	}
	
	return White_Flag;	// �����������
}

/*
*	���Ҳ࿪ʼ�����߽磬���������߽磬����1�ɹ� 0ʧ��
*
*	˵����������ʹ�ú󽫱���߽�����
*/
uchar Traversal_Right_Line(uchar i, uchar *data, uchar *Left_Line, uchar *Right_Line)
{
	uchar j, White_Flag = 0;
	
	Left_Line[i] = 1;
	Right_Line[i] = 79;
	
	for (j = 79; j > 0; j--)	// �߽����� 1��79
	{
		if (!White_Flag)	// �Ȳ����ұ߽�
		{
			if (data[i*80 + j])	// ��⵽�׵�
			{
				Right_Line[i] = j;	// ��¼��ǰjֵΪ�����ұ߽�
				White_Flag = 1;		// �ұ߽����ҵ���������߽�
				
				continue;	// ��������ѭ���������´�ѭ��
			}
		}
		else
		{
			if (!data[i*80 + j])	//���ڵ�
			{
				Left_Line[i] = j+1;//��¼��ǰjֵΪ������߽�
				
				break;	// ���ұ߽綼�ҵ�������ѭ��
			}
		}
	}
	if (White_Flag)
	{
		Left_Add_Line[i] = Left_Line[i];
		Right_Add_Line[i] = Right_Line[i];
		Width_Real[i] = Right_Line[i] - Left_Line[i];
		Width_Add[i] = Width_Real[i];
	}
	
	return White_Flag;	// �����������
}

/*
*	���м������������߽�
*
*	˵����������ʹ�ú󽫱���߽�����
*/
void Traversal_Mid_Line(uchar i, uchar *data, uchar Mid, uchar Left_Min, uchar Right_Max, uchar *Left_Line, uchar *Right_Line, uchar *Left_Add_Line, uchar *Right_Add_Line)
{
	uchar j;
	
	Left_Add_Flag[i] = 1;	// ��ʼ�����߱�־λ
	Right_Add_Flag[i] = 1;
	
	Left_Min = range_protect(Left_Min, 1, 79);	// �޷�����ֹ����
	if (Left_Add_Flag[i+2])
	{
		Left_Min = range_protect(Left_Min, Left_Add_Line[i+2]-10, 79);
	}
	Right_Max = range_protect(Right_Max, 1, 79);
	if (Right_Add_Flag[i+2])
	{
		Right_Max = range_protect(Right_Max, 1, Right_Add_Line[i+2]+10);
	}
	
	Right_Line[i] = Right_Max;
	Left_Line[i] = Left_Min;	// �����߽��ʼֵ
	
	for (j = Mid; j >= Left_Min; j--)	// ��ǰһ���е�Ϊ���������ұ߽�
	{
		if (!data[i*80 + j])	// ��⵽�ڵ�
		{
			Left_Add_Flag[i] = 0;	//��߽粻��Ҫ���ߣ������־λ
			Left_Line[i] = j+1;	//��¼��ǰjֵΪ����ʵ����߽�
			Left_Add_Line[i] = j+1;	// ��¼ʵ����߽�Ϊ������߽�
			
			break;
		}
	}
	for (j = Mid; j <= Right_Max; j++)	// ��ǰһ���е�Ϊ������Ҳ����ұ߽�
	{
		if (!data[i*80 + j])	//��⵽�ڵ�
		{
			Right_Add_Flag[i] = 0;		//�ұ߽粻��Ҫ���ߣ������־λ
			Right_Line[i] = j-1;	//��¼��ǰjֵΪ�����ұ߽�
			Right_Add_Line[i] = j-1;	// ��¼ʵ���ұ߽�Ϊ������߽�
			
			break;
		}
	}
	if (Left_Add_Flag[i])	// ��߽���Ҫ����
	{
		if (i >= 55)	// ǰ6��
		{
			Left_Add_Line[i] = Left_Line[59];	// ʹ�õ�������
		}
		else
		{
			Left_Add_Line[i] = Left_Add_Line[i+2];	// ʹ��ǰ2����߽���Ϊ������߽�
		}
	}
	if (Right_Add_Flag[i])	// �ұ߽���Ҫ����
	{
		if (i >= 55)	// ǰ6��
		{
			Right_Add_Line[i] = Right_Line[59];	// ʹ�õ�������
		}
		else
		{
			Right_Add_Line[i] = Right_Add_Line[i+2];	// ʹ��ǰ2���ұ߽���Ϊ�����ұ߽�
		}
	}
	Width_Real[i] = Right_Line[i] - Left_Line[i];			// ����ʵ���������
	Width_Add[i] = Right_Add_Line[i] - Left_Add_Line[i];	// ���㲹���������
	
//	data[80 * i + Right_Line[i] + 2] = 0;//����2λ��ʾ��߽磬������Թ۲�
//	data[80 * i + Left_Line[i] - 2] = 0;//����2λ��ʾ�ұ߽磬������Թ۲�
}

/*
*	�����޸�
*
*	˵������ʼ���ղ�ʹ�ã�ֱ��ʹ������б�ʽ��в���
*/
void Line_Repair(uchar Start, uchar Stop, uchar *data, uchar *Line, uchar *Line_Add, uchar *Add_Flag)
{
	float res;
	uchar i;	// ������
	float Ka, Kb;
	
	if (Start == 57)
	{
		Start = 59;
	}
	else
	{
		Start += 4;
	}
	
	if (Stop)	// ��ʼ����
	{
		Ka = 1.0*(Line_Add[Start] - Line_Add[Stop]) / (Start - Stop);
		Kb = 1.0*Line_Add[Start] - (Ka * Start);
		
		for (i = Stop; i <= Start; )
		{
			i += 2;
			res = i * Ka + Kb;
			Line_Add[i] = range_protect((int32)res, 1, 79);
		}
	}
//	else if (!Left_Hazard_Flag && !Right_Hazard_Flag)	// ��ʼ������û���ϰ���
//	{
//		if (((Left_Add_Start==0) || (Right_Add_Start==0)) && (Start - 16 >= Line_Count))
//		{
//			for (i = Start; i >= Line_Count;)
//			{
//				i -= 2;
//				Line_Add[i] = Line[i];	// ʹ����ʵ�߽粻�ò���
//			}
//		}
//	}
}

/*
*	�����޸�
*
*	˵������ͨ�������ʹ��ƽ��������ʽ���е㵽��߽����
*/
void Mid_Line_Repair(uchar count, uchar *data)
{
	uchar i;	// ������
	
	Left_Max = Left_Add_Line[59];
	Right_Min = Right_Add_Line[59];
	
	for (i = 61; i >= count; )
	{
		i -= 2;
		Mid_Line[i] = (Right_Add_Line[i] + Left_Add_Line[i]) / 2;	// ���������е�
		Width_Add[i] = Right_Add_Line[i] - Left_Add_Line[i];		// �����������
		
		if (Left_Add_Line[i] > Left_Max)		// ������ұ߽缫ֵ����
		{
			Left_Max = Left_Add_Line[i];
		}
		if (Right_Add_Line[i] < Right_Min)
		{
			Right_Min = Right_Add_Line[i];
		}
			
	/**//*************************** ��λ����ʾ�߽� ***************************/
	/**/data[i*80 + Left_Add_Line[i] + 2] = 0;	// ��λ����ʾ���ߺ����߽磬����ʱ����
	/**/data[i*80 + Right_Add_Line[i] - 2] = 0;	// ��λ����ʾ���ߺ���ұ߽磬����ʱ����
	/**/data[i*80 + Mid_Line[i]] = 0;			// ��λ����ʾ���ߣ�����ʱ����
//	/**/data[i*80 + Left_Line[i] + 2] = 0;		// ��λ����ʾԭʼ��߽磬����ʱ����
//	/**/data[i*80 + Right_Line[i] - 2] = 0;		// ��λ����ʾԭʼ�ұ߽磬����ʱ����
	/**//*************************** ��λ����ʾ�߽� ***************************/
	}
	Mid_Line[61] = Mid_Line[59];
}

/****************** ���㷨 ******************/

/*
*	��Ȩƽ��
*
*	˵����Ȩ�����Ҵյģ�Ч������
*/
uchar Point_Weight(void)
{
	uchar i ,Point, Point_Mid;
	static char Last_Point = 40;
	int32 Sum = 0, Weight_Count = 0;
	
	if (Line_Count <= 20)
	{
		Line_Count = 20;
	}
	
	if (Out_Side || Line_Count >= 53)	//�����������ͷͼ���쳣
	{
		if (Last_Point == 40)
		{
			Point = Last_Point;
		}
		else if (Last_Point < 40)
		{
			Point = 1;
		}
		else if (Last_Point > 40)
		{
			Point = 79;
		}
	}
	else
	{
		for (i = 61; i >= Line_Count; )		//ʹ�ü�Ȩƽ��
		{
			i -= 2;
			Sum += Mid_Line[i] * Weight[59-i];
			Weight_Count += Weight[59-i];
		}
		Point = range_protect(Sum / Weight_Count, 1, 79);

							/*** �ϰ�������������� ***/
//		if (Left_Hazard_Flag)			//������ϰ�������Ҫ���ߣ���ʹ����Ҳ�������Ӱ��
//		{
//			Point = Mid_Line[Left_Hazard_Flag]+6;	//ʹ���ϰ�����ֵ���һ���е���ΪĿ���
//			if (Left_Hazard_Flag < 40)
//			{
//				Point += 3;
//			}
//		}
//		else if (Right_Hazard_Flag)	//�Ҳ����ϰ�������Ҫ���ߣ���ʹ����Ҳ�������Ӱ��
//		{
//			Point = Mid_Line[Right_Hazard_Flag]-6;//ʹ���ϰ�����ֵ���һ���е���ΪĿ���
//			if (Right_Hazard_Flag < 40)
//			{
//				Point -= 5;
//			}
//		}
//		Point = Mid_Line[59];
		Point = range_protect(Point, 2, 78);
		Last_Point = Point;
		
							/***** ʹ����Զ�����ݺ�Ŀ�����Ϊǰհ *****/
		if (Line_Count >= 25)
		{
			Point_Mid = Mid_Line[60-30];
		}
		else
		{
			Point_Mid = Mid_Line[60-Line_Count];
		}
	}
	Foresight = 0.8 * Error_Transform(Point_Mid, 40)	//ʹ����Զ��ƫ��ͼ�Ȩƫ��ȷ��ǰհ
			  + 0.2 * Error_Transform(Point, 	 40);
	
	return Point;
}

char Error_Transform(uchar Data, uchar Set)
{
	char Error;
	
	Error = Set - Data;
	if (Error < 0)
	{
		Error = -Error;
	}
	
	return Error;
}



/*!
*  @brief      ��ֵ��ͼ���ѹ���ռ� �� ʱ�� ��ѹ��
*  @param      dst             ͼ���ѹĿ�ĵ�ַ
*  @param      src             ͼ���ѹԴ��ַ
*  @param      srclen          ��ֵ��ͼ���ռ�ÿռ��С
*  @since      v5.0            img_extract(img, imgbuff,CAMERA_SIZE);
*  Sample usage:
*/
void img_extract(uint8 *dst, uint8 *src, uint32 srclen)
{
  uint8 tmpsrc;
  while(srclen --)
  {
    tmpsrc = *src++;
    *dst++ = colour[ (tmpsrc >> 7 ) & 0x01 ];
    *dst++ = colour[ (tmpsrc >> 6 ) & 0x01 ];
    *dst++ = colour[ (tmpsrc >> 5 ) & 0x01 ];
    *dst++ = colour[ (tmpsrc >> 4 ) & 0x01 ];
    *dst++ = colour[ (tmpsrc >> 3 ) & 0x01 ];
    *dst++ = colour[ (tmpsrc >> 2 ) & 0x01 ];
    *dst++ = colour[ (tmpsrc >> 1 ) & 0x01 ];
    *dst++ = colour[ (tmpsrc >> 0 ) & 0x01 ];
  }
}

/*!
*  @brief      ��ֵ��ͼ��ѹ�����ռ� �� ʱ�� ѹ����
*  @param      dst             ͼ��ѹ��Ŀ�ĵ�ַ
*  @param      src             ͼ��ѹ��Դ��ַ
*  @param      srclen          ��ֵ��ͼ���ռ�ÿռ��С
*  @since      v5.0            img_extract(img, imgbuff,CAMERA_SIZE);
*  Sample usage:
*/
void img_recontract(uint8 *dst, uint8 *src, uint32 srclen)
{
  uint8 tmpsrc;
  
  while(srclen --)
  {
    tmpsrc=0;
    if(*dst++ == colour[black])
    {
      tmpsrc = tmpsrc + 0x80;
    }
    
    if(*dst++ == colour[black])
    {
      tmpsrc = tmpsrc + 0x40;
    }
    
    if(*dst++ == colour[black])
    {
      tmpsrc = tmpsrc + 0x20;
    }
    
    if(*dst++ == colour[black])
    {
      tmpsrc = tmpsrc + 0x10;
    }
    
    if(*dst++ == colour[black])
    {
      tmpsrc = tmpsrc + 0x08;
    }
    
    if(*dst++ == colour[black])
    {
      tmpsrc = tmpsrc + 0x04;
    }
    
    if(*dst++ == colour[black])
    {
      tmpsrc = tmpsrc + 0x02;
    }
    
    if(*dst++ == colour[black])
    {
      tmpsrc = tmpsrc + 0x01;
    }
    
    *src++ = tmpsrc;
  }
}

/*!
*  @brief      ȡ�߽���
*  @param      dst             ͼ��ѹ��Ŀ�ĵ�ַ
*  @param      src             ͼ��ѹ��Դ��ַ
*  @param      srclen          ��ֵ��ͼ���ռ�ÿռ��С
*  @since      v5.0            img_extract(img, imgbuff,CAMERA_SIZE);
*  Sample usage:
*/
void img_getline(uint8 *dst, uint8 *src, uint32 srclen)
{
  uint8 tmpsrc,buff_y,buff_x;
  
  while(srclen --)
  {
    tmpsrc=0;
    buff_x = *(dst + 80);
    buff_y = *dst++;
    *(dst-1)=colour[white];
    if(*dst != buff_y || (buff_y != buff_x && srclen > 9))
    {
      tmpsrc = tmpsrc + 0x80;
      *(dst-1)=colour[black];
    }
    
    buff_x = *(dst + 80);
    buff_y = *dst++;
    *(dst-1)=colour[white];
    if(*dst != buff_y || (buff_y != buff_x && srclen > 9))
    {
      tmpsrc = tmpsrc + 0x40;
      *(dst-1)=colour[black];
    }
    
    buff_x = *(dst + 80);
    buff_y = *dst++;
    *(dst-1)=colour[white];
    if(*dst != buff_y || (buff_y != buff_x && srclen > 9))
    {
      tmpsrc = tmpsrc + 0x20;
      *(dst-1)=colour[black];
    }
    
    buff_x = *(dst + 80);
    buff_y = *dst++;
    *(dst-1)=colour[white];
    if(*dst != buff_y || (buff_y != buff_x && srclen > 9))
    {
      tmpsrc = tmpsrc + 0x10;
      *(dst-1)=colour[black];
    }
    
    buff_x = *(dst + 80);
    buff_y = *dst++;
    *(dst-1)=colour[white];
    if(*dst != buff_y || (buff_y != buff_x && srclen > 9))
    {
      tmpsrc = tmpsrc + 0x08;
      *(dst-1)=colour[black];
    }
    
    buff_x = *(dst + 80);
    buff_y = *dst++;
    *(dst-1)=colour[white];
    if(*dst != buff_y || (buff_y != buff_x && srclen > 9))
    {
      tmpsrc = tmpsrc + 0x04;
      *(dst-1)=colour[black];
    }
    
    buff_x = *(dst + 80);
    buff_y = *dst++;
    *(dst-1)=colour[white];
    if(*dst != buff_y || (buff_y != buff_x && srclen > 9))
    {
      tmpsrc = tmpsrc + 0x02;
      *(dst-1)=colour[black];
    }
    
    buff_x = *(dst + 80);
    buff_y = *dst++;
    *(dst-1)=colour[white];
    if((*dst != buff_y && srclen % 10 !=0 ) || (buff_y != buff_x && srclen > 9))
    {
      tmpsrc = tmpsrc + 0x01;
      *(dst-1)=colour[black];
    }
    
    *src++ = tmpsrc;
  }
}
