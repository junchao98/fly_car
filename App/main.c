#include "common.h"
#include "include.h"


#include "commun.h"



uint8 ImageBuff[CAMERA_W*CAMERA_H/8];							// 图像采集缓存地址
uint8 *imgbuff = (uint8 *)(((uint8 *)&nrf_tx_buff) + COM_LEN);	// 图像地址，用于上位机观察
uint8 img[CAMERA_W*CAMERA_H]; //由于鹰眼摄像头是一字节8个像素，因而需要解压为 1字节1个像素，方便处理
int cen = 0;
int flag = 0;
int rd0 = 775;
int a=0;


/*********** main函数***********/
void main(void)
{  
  uint16 rud = 775;
  
  System_Init();		//初始化所有模块
  
  SCCB_WriteByte (OV7725_CNST, 60);	//改变图像阈值
  
  while(1)
  {   
    a++;
    
    if(a==900)a=0;
    
    /************************ 图像采集和显示  ***********************/
    while (!Image_Flag);
    Image_Flag = 0;
    img_extract(img, ImageBuff,CAMERA_SIZE);	// 解压
    camera_get_img();                           // 摄像头获取图像
    /************************* 图像算法分析 ************************/
    if (!Annulus_Flag)
    {
      Image_Handle(img);			// 普通图像处理
    }
    if (Annulus_Flag)
    {
     // Image_Handle(img);
      Annulus_Handle(img);		// 环路专用图像处理
    }
    Difference = Area_Calculate();      
    pid_rudder(Difference,&rud,flag); 
    ftm_pwm_duty(FTM0, FTM_CH0, rud);//更新舵机控制PWM 
    
    
    //main_test = get_distance();

    
    
    /********************** HMI串口数据收发   **********************/ 
    if (0)
    {
      HMI_Data_Handle();
    }
    /************************ 无线数据发送   ***********************/  
    if (0) //选择是否发送图像信息至上位机
    {
      Send_OK = 0;
      img_recontract(img, imgbuff,CAMERA_SIZE); //图像压缩
      data_send();	                          //发送至上位机
    }
  }
}











