/*!
*     COPYRIGHT NOTICE
*     Copyright (c) 2013,山外科技
*     All rights reserved.
*     技术讨论：山外论坛 http://www.vcan123.com
*
*     除注明出处外，以下所有内容版权均属山外科技所有，未经允许，不得用于商业用途，
*     修改内容时必须保留山外科技的版权声明。
*
* @file       MK60_it.c
* @brief      山外K60 平台中断服务函数
* @author     山外科技
* @version    v5.0
* @date       2013-06-26
*/
#include    "MK60_it.h"

#define DATA_NUM 6


  uint8 count = 0;
  uint8 cache = 0;
  uint8 Distance_Buff[4]={0};
  uint16 data_list[6] = {0};
  uint8 list_count = 0;


uchar Time_10ms = 0;
uchar Time_20ms = 0;
uchar Time_50ms = 0;
uchar Time_100ms = 0;
uchar Time_motor = 0;
int16 p1 = 0,p2 = 0;
int32 i1 = 0,i2 = 0;
int16 d1 = 0,d2 = 0;


int spl = 0;
int spr = 0;
uint16 ch1_tmp,ch2_tmp,ch3_tmp,ch4_tmp;
void pid_motor(int16 sp1, int16 sp2, int16 gsp1, int16 gsp2, uint16 *ch1, uint16 *ch2, uint16 *ch3, uint16 *ch4,int32 cen_tmp);
/*********************************中断服务函数********************************/
/*!
*  @brief      PIT0中断服务函数
*  @since      v5.0
*/
void PIT0_IRQHandler()	// 2ms进一次中断
{	
  if (Annulus_Delay)
  {
    Annulus_Delay--;
  }
  if (Time_10ms < 5)
  {
    Time_10ms++;
  }
  if(Time_10ms == 5)
  {
    spl = ftm_quad_get(FTM1);//获得左轮脉冲
    spr = -ftm_quad_get(FTM2);//获得右轮脉冲   
    ftm_quad_clean(FTM1);//清除左轮脉冲
    ftm_quad_clean(FTM2);//清楚右轮脉冲
    if(!Annulus_Flag)
    {
      pid_motor(spl,spr,195,195,&ch1_tmp,&ch2_tmp,&ch3_tmp,&ch4_tmp,Difference);
    }
    if(Annulus_Flag)
    {
      pid_motor(spl,spr,195,195,&ch1_tmp,&ch2_tmp,&ch3_tmp,&ch4_tmp,Difference);
    }    
    ftm_pwm_duty(FTM3, FTM_CH2, ch2_tmp);//左轮后
    ftm_pwm_duty(FTM3, FTM_CH3, ch4_tmp);//右轮后
    ftm_pwm_duty(FTM3, FTM_CH4, ch3_tmp);//右轮前
    ftm_pwm_duty(FTM3, FTM_CH6, ch1_tmp);//左轮前     
    //ftm_pwm_duty(FTM3, FTM_CH2, 0);//左轮后
    //ftm_pwm_duty(FTM3, FTM_CH3, 0);//右轮后
    //ftm_pwm_duty(FTM3, FTM_CH4, 2800);//右轮前
    //ftm_pwm_duty(FTM3, FTM_CH6, 2800);//左轮前
    Time_10ms = 0;
  }    
  if (Time_20ms < 10)	// 方向环20ms控制一次
  {
    Time_20ms++;
  }
  if (Time_50ms < 50)
  {
    Time_50ms++;
  }
  if (Time_100ms < 100)
  {
    Time_100ms++;
  }
  PIT_Flag_Clear(PIT0);//清除定时器中断标志位
}


/*!
*  @brief      PIT1中断服务函数
*  @since      v5.0
*/
void PIT1_IRQHandler()
{
  PIT_Flag_Clear(PIT1);//清除定时器中断标志位
}


/*!
*  @brief      UART4中断服务函数
*  @since      v5.0
*/
void UART4_IRQHandler(void)
{
  char Buff;
  UARTn_e uratn = UART4;
  
  if(UART_S1_REG(UARTN[uratn]) & UART_S1_RDRF_MASK) //接收数据寄存器满
  {
    uart_getchar(uratn, &Buff);
    switch (Buff)
    {
      //  用户代码
      
      //  用户代码
    }
  }
}






  
  
  uint32 data_average(uint16 *data)
  {
    uint8 i = 0;
    uint8 j = 0;
    uint32 all = 0;
    uint32 max = 0;
    uint32 min = 0;
    uint32 tem = 0;
    uint8  max_i = 0;
    uint8  min_i = 0;
    uint32 num = DATA_NUM;
    uint8  dis_error = 0;
    uint8  data_h[DATA_NUM] = {0};
    
    

    for(i = 1; i< DATA_NUM-2; i++){
      
    
    
    
    
    }
    
    for(i = 0; i< DATA_NUM; i++){
      
      if(data[i] < 199){                   //去除极小
       data[i] = 0;
       num --;
       continue;
      
      }
    
      
      
      if(data[i] > max){                //去除 差大于240
        
        if((data[i] - max) > 240){
          data[i] = 0;
          max_i ++;
          num --;
          continue;
        }
        max = data[i];
        
      }
      if(data[i] < min){                 //去除 差大于240
        
        if((min - data[i]) > 240){
          data[i] = 0;
          min_i ++;
          num --;
          continue;
        }
        min = data[i];
      
      }
      
      all += data[i];
      
      
    }
  
   
    tem = max - min ;
    
     if(num !=) all = all/num;
    
     if(tem > 80){
  
      
       }else{
       
       
           return all;
       
       }
     
     
     return 0;
  
  }
  
void UART1_IRQHandler(void)
{
 
  
  UARTn_e uratn = UART1;
  
  if(UART_S1_REG(UARTN[uratn]) & UART_S1_RDRF_MASK) //接收数据寄存器满
  {

 
    uart_getchar(uratn,&Distance_Buff[count]);
    
     
    if(Distance_Buff[0] != 0xA5){
      count = 0;
    
    }else {
    
      count ++;
    }
    

    
    if(count == 3){
    
      
       cache = Distance_Buff[1] << 8 | Distance_Buff[2];  
       
       data_list[list_count] = cache;       //数据列
       list_count ++;
       
       if(list_count == DATA_NUM){         //车速假设2m/s  6*20ms = 120ms  跑240 mm
       
         //添加数据处理
         list_count = 0;
         
         
       
       }
       
       
       count = 0;
      
     
    
    }
    
    
    
    
     }
  
  
    
    
}

/*!
*  @brief      PORTA中断服务函数
*  @since      v5.0
*/
void PORTA_IRQHandler()
{
  uint8  n;    //引脚号
  uint32 flag;
  
  //while(!PORTA_ISFR);
  flag = PORTA_ISFR;
  PORTA_ISFR  = ~0;                                   //清中断标志位
  
  n = 29;                                             //场中断
  if(flag & (1 << n))                                 //PTA29触发中断
  {
    camera_vsync();
  }
#if ( CAMERA_USE_HREF == 1 )                            //使用行中断
  n = 28;
  if(flag & (1 << n))                                 //PTA28触发中断
  {
    camera_href();
  }
#endif
  
}


/*!
*  @brief      PORTE中断服务函数
*  @since      v5.0
*/
void PORTE_IRQHandler()
{
  uint8  n;    //引脚号
  uint32 flag;
  
  flag = PORTE_ISFR;
  PORTE_ISFR  = ~0;                                   //清中断标志位
  
  n = 27;
  if(flag & (1 << n))                                 //PTE27触发中断
  {
    nrf_handler();
  }
}


/*!
*  @brief      DMA0中断服务函数
*  @since      v5.0
*/
void DMA0_IRQHandler()
{
  camera_dma();
}
void pid_motor(int16 sp1, int16 sp2, int16 gsp1, int16 gsp2, uint16 *ch1, uint16 *ch2, uint16 *ch3, uint16 *ch4,int32 cen_tmp)
{
  int32 out1,out2;
  int tmp;
  tmp = (int32)((float)cen_tmp * 2.5);
  if(tmp < -30)
  {
    gsp1 = gsp1 + tmp;
  }
  if(tmp > 30)
  {
    gsp2 = gsp2 - tmp;
  }
//pid算法
//左轮
  p1 = (gsp1 - sp1);
  i1 = i1 + p1;
  if (i1 > 10000) i1 = 10000;
  if (i1 < -10000) i1 = -10000;
  out1 = p1 * 66 + (int32)(i1 * 1.2) + d1 * 20;
  if (out1 > 9999) out1 = 9999;
  if (out1 < -9999) out1 = -9999; 
  d1 = p1;
  if(out1 >= 0)
  {
    *ch1 = out1;
    *ch2 = 0;
  }
  else
  {
    *ch1 = 0;
    *ch2 = -out1;
  }
//右轮
  p2 = (gsp2 - sp2);
  i2 = i2 + p2;
  if (i2 > 10000) i2 = 10000;
  if (i2 < -10000) i2 = -10000;
  out2 = p2 * 66 + (int32)(i2 * 1.2) + d2 * 20;
  if (out2 > 9999) out2 = 9999;
  if (out2 < -9999) out2 = -9999;  
  d2 = p2;
  if(out2 >= 0)
  {
    *ch3 = out2;
    *ch4 = 0;
  }
  else
  {
    *ch3 = 0;
    *ch4 = -out2;
  }
}