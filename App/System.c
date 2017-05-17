#include "System.h"

uint8  nrf_rx_buff[CAMERA_SIZE + 2*COM_LEN + DATA_PACKET];         //预多
uint8  nrf_tx_buff[CAMERA_SIZE + 2*COM_LEN + DATA_PACKET];         //预多
uchar Image_Send_Flag = 1;	// 图像发送控制，1为发送，0为不发送
uchar Send_OK = 0;
uchar System_OK = 0;

void System_Init()
{
  int i;
  /************************ 配置 K60 的优先级  *****************************/
  
  NVIC_SetPriorityGrouping(4);            // 设置优先级分组,4bit 抢占优先级,没有亚优先级
  
  NVIC_SetPriority(PORTA_IRQn,0);         // 配置优先级
  NVIC_SetPriority(PORTE_IRQn,1);         // 配置优先级
  NVIC_SetPriority(DMA0_IRQn,3);          // 配置优先级
  NVIC_SetPriority(PIT0_IRQn,4);          // 配置优先级
  NVIC_SetPriority(PIT1_IRQn,5);          // 配置优先级
  NVIC_SetPriority(UART4_RX_TX_IRQn,6);   // 配置优先级
  
  
  /************************ 参数初始化 **********************************/
  Image_Para_Init();
  
  
  //用户代码
  
  
    //超声波串口初始化
  uart_init (UART4, 115200);
  
  
  //用户代码
  
  
  
  /************************ 串口 初始化 ************************************/
  uart_init(UART4, 115200);	// 蓝牙
  uart_init(UART0, 115200);	// HMI液晶屏
  set_vector_handler(UART4_RX_TX_VECTORn ,UART4_IRQHandler);	// 设置 UART4 的中断服务函数为 UART4_IRQHandler
  uart_rx_irq_en(UART4);		// 开串口4接收中断
  HMI_Send_End();		// 发送一次结束符，清除上电产生的串口杂波数据
  
  
  /************************ 电机 舵机 初始化 ************************************/
  //电机PWM控制初始化
  ftm_pwm_init(FTM3, FTM_CH2, 10000, 0);//左轮后
  ftm_pwm_init(FTM3, FTM_CH3, 10000, 0);//右轮后
  ftm_pwm_init(FTM3, FTM_CH4, 10000, 2000);//右轮前
  ftm_pwm_init(FTM3, FTM_CH6, 10000, 2000);//左轮前
  
    //舵机PWM控制初始化
  ftm_pwm_init(FTM0, FTM_CH0, 50, 1550);
  
  /************************ 正交解码 ***************************************/   
  ftm_quad_init(FTM1);	// 正交解码初始化
  ftm_quad_init(FTM2);	// 使用定时器0触发，50ms执行一次
  
  
  
  /************************ 摄像头 初始化 **********************************/
  camera_init(ImageBuff);                                   // 摄像头初始化，把图像采集到 ImageBuff 地址
  set_vector_handler(PORTA_VECTORn ,PORTA_IRQHandler);	// 设置 PORTA 的中断服务函数为 PORTA_IRQHandler
  set_vector_handler(DMA0_VECTORn ,DMA0_IRQHandler);      // 设置 DMA0 的中断服务函数为 DMA0_IRQHandler
  /************************ 无线模块 NRF 初始化 ****************************/
  if (0)
  {
    LCD_init();
    while(!nrf_init());
    set_vector_handler(PORTE_VECTORn ,PORTE_IRQHandler);	//设置 PORTE 的中断服务函数为 PORTE_VECTORn
    enable_irq(PORTE_IRQn);
    nrf_msg_init();                                        	//无线模块消息初始化
    i = 20;	// 摄像头模块，需要 发送 空命令给 调试板模块，这样可以清掉 接收图像失败而产生多余数据
    while(i--)
    {
      nrf_msg_tx(COM_RETRAN,nrf_tx_buff);               	//发送多个 空 命令过去，用于清空 接收端的缓存数据
    }
  }
  /************************ 定时器 初始化  *********************************/ 
  pit_init_ms(PIT0, 2);                              	// 初始化PIT0，定时时间为： 2ms
  set_vector_handler(PIT0_VECTORn ,PIT0_IRQHandler);	// 设置PIT0的中断服务函数为 PIT0_IRQHandler
  enable_irq (PIT0_IRQn); 							// 使能中断

  pit_init_ms(PIT1, 1000);                           	// 初始化PIT1，定时时间为： 1s
  set_vector_handler(PIT1_VECTORn ,PIT1_IRQHandler); 	// 设置PIT1的中断服务函数为 PIT1_IRQHandler
  enable_irq (PIT1_IRQn); 							// 使能中断
  /************************ 图像采集 ***************************************/
  
  camera_get_img();	// 先采集一次图像
  
  /************************ LED 初始化  ************************************/
  led_init(LED_MAX);
  led(LED_MAX,LED_ON);// 打开LED指示灯		全部初始化成功
}


void data_send()
{
  Site_t site     = {0, 0};                           //显示图像左上角位置
  Size_t imgsize  = {CAMERA_W, CAMERA_H};             //图像大小
  Size_t size     = {80, 60}; //{LCD_W, LCD_H};       //显示区域图像大小
  
  com_e     com;
  nrf_result_e nrf_result;
  uint8 imgtxflag = 0;    //0表示发送图像
  
  /************************ 无线发送和接收数据  ***********************/
  do
  {
    nrf_result = nrf_msg_rx(&com, nrf_rx_buff);
    if(nrf_result == NRF_RESULT_RX_VALID)
    {
      switch(com)
      {	
      case COM_IMG:
        //显示 到 屏幕上
#if     ( CAMERA_COLOR == 1 )                                       //灰度摄像头
        LCD_Img_gray_Z(site,size, imgbuff,imgsize);
#elif   ( CAMERA_COLOR == 0 )                                       //黑白摄像头
        LCD_Img_Binary_Z(site,size, imgbuff,imgsize);
#endif
        break;
        
      case COM_VAR:
        if(last_tab == CAR_CTRL)                    //如果是 控制 小车 命令
        {
          switch(car_ctrl_get())                  //获取小车控制命令，并进行处理
          {
          case CAR_START:                         //开车
            //此处加入 开车 代码
            if (Run_Flag)
            {
              Run_Flag = 0;
              led(LED0,LED_OFF);
            }
            else
            {
              Run_Flag = 1;
              led(LED0,LED_ON);
            }														
            
            break;
            
          case CAR_STOP:                          //停车
            //此处加入 停车 代码 ，例如：关闭电机驱动（关闭使能）
            
            break;
            
          case CAR_LEFT:					
            
            break;
            
          case CAR_RIGHT:
            
            break;
            
          case CAR_IMG_OPEN:
            imgtxflag = 0;
            break;
            
          case CAR_IMG_CLOSE:
            imgtxflag = 1;
            break;
            
          default:
            //无效，不进行任何处理
            break;
          }
        }			
        break;
        
      default:
        break;
      }
    }
  }while(nrf_result != NRF_RESULT_RX_NO);         //接收不到数据 才退出
  
  if(!imgtxflag)
  {
    //把图像 发送出去
    nrf_msg_tx(COM_IMG,nrf_tx_buff);
  }
  
#if     ( CAMERA_COLOR == 1 )                                       //灰度摄像头
  LCD_Img_gray_Z       (site,size, imgbuff,imgsize);
#elif   ( CAMERA_COLOR == 0 )                                       //黑白摄像头
  LCD_Img_Binary_Z(site,size, imgbuff,imgsize);
#endif
  
  if(!imgtxflag)
  {
    while(nrf_tx_state() == NRF_TXING);             //等待发送完成
  }
}


void HMI_Data_Handle(void)
{
  char  UART0_Buff[3];
  
  if (uart_query (UART0) == 1)
  {
    uart_getchar(UART0, UART0_Buff);
    //用户需要处理接收数据
    if (UART0_Buff[0] == 0x66)
    {			
      uart_putstr(UART0, "page Run");	//HMI屏转到运动界面
      HMI_Send_End();
      uart_putstr(UART0, "dim=10");	//降低显示屏亮度，节约用电
      HMI_Send_End();
      System_OK = 1;					//调参结束，开始定时起跑
      enable_irq (PIT1_IRQn);			//使能定时器1，做自动起跑准备
    }
    else if (UART0_Buff[0] == 0x01)
    {			
      uart_putstr(UART0, "t2.txt=\"140\"");
      HMI_Send_End();
      Mode_Set = 1;	//匀速140
    }
    else if (UART0_Buff[0] == 0x02)
    {
      Mode_Set = 2;	//匀速150
      uart_putstr(UART0, "t2.txt=\"150\"");
      HMI_Send_End();
    }
    else if (UART0_Buff[0] == 0x03)
    {
      Mode_Set = 3;	//匀速160
      uart_putstr(UART0, "t2.txt=\"160\"");
      HMI_Send_End();
    }
    else if (UART0_Buff[0] == 0x04)
    {
      Mode_Set = 4;	//匀速170
      uart_putstr(UART0, "t2.txt=\"170\"");
      HMI_Send_End();
    }
    else if (UART0_Buff[0] == 0x05)
    {
      Mode_Set = 5;	//稳定模式2米3
      uart_putstr(UART0, "t2.txt=\"稳定模式\"");
      HMI_Send_End();
    }
    else if (UART0_Buff[0] == 0x06)
    {
      Mode_Set = 6;	//快速模式2米35
      uart_putstr(UART0, "t2.txt=\"快速模式\"");
      HMI_Send_End();
    }
    else if (UART0_Buff[0] == 0x07)
    {
      Mode_Set = 7;	//疯狗模式，看脸
      uart_putstr(UART0, "t2.txt=\"疯狗模式\"");
      HMI_Send_End();
    }
    else if (UART0_Buff[0] == 0x11)
    {
      SCCB_WriteByte (OV7725_CNST, 50);	//阈值改为50，明亮使用
      uart_putstr(UART0, "t1.txt=\"50\"");
      HMI_Send_End();
    }
    else if (UART0_Buff[0] == 0x12)
    {
      SCCB_WriteByte (OV7725_CNST, 60);	//阈值改为60
      uart_putstr(UART0, "t1.txt=\"60\"");
      HMI_Send_End();
    }
    else if (UART0_Buff[0] == 0x13)
    {
      SCCB_WriteByte (OV7725_CNST, 40);	//阈值改为70，昏暗使用
      uart_putstr(UART0, "t1.txt=\"40\"");
      HMI_Send_End();
    }
    UART0_Buff[0] = 0;
  }
}

void HMI_Send_End(void)		//发送HMI指令结束符
{
  uart_putchar(UART0, 0xff); //从串口丢一个字节数据出去
  uart_putchar(UART0, 0xff); //从串口丢一个字节数据出去
  uart_putchar(UART0, 0xff); //从串口丢一个字节数据出去
}
