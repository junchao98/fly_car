#include "System.h"

uint8  nrf_rx_buff[CAMERA_SIZE + 2*COM_LEN + DATA_PACKET];         //Ԥ��
uint8  nrf_tx_buff[CAMERA_SIZE + 2*COM_LEN + DATA_PACKET];         //Ԥ��
uchar Image_Send_Flag = 1;	// ͼ���Ϳ��ƣ�1Ϊ���ͣ�0Ϊ������
uchar Send_OK = 0;
uchar System_OK = 0;

void System_Init()
{
  int i;
  /************************ ���� K60 �����ȼ�  *****************************/
  
  NVIC_SetPriorityGrouping(4);            // �������ȼ�����,4bit ��ռ���ȼ�,û�������ȼ�
  
  NVIC_SetPriority(PORTA_IRQn,0);         // �������ȼ�
  NVIC_SetPriority(PORTE_IRQn,1);         // �������ȼ�
  NVIC_SetPriority(DMA0_IRQn,3);          // �������ȼ�
  NVIC_SetPriority(PIT0_IRQn,4);          // �������ȼ�
  NVIC_SetPriority(PIT1_IRQn,5);          // �������ȼ�
  NVIC_SetPriority(UART4_RX_TX_IRQn,6);   // �������ȼ�
  
  
  /************************ ������ʼ�� **********************************/
  Image_Para_Init();
  
  
  //�û�����
  
  
    //���������ڳ�ʼ��
  uart_init (UART4, 115200);
  
  
  //�û�����
  
  
  
  /************************ ���� ��ʼ�� ************************************/
  uart_init(UART4, 115200);	// ����
  uart_init(UART0, 115200);	// HMIҺ����
  set_vector_handler(UART4_RX_TX_VECTORn ,UART4_IRQHandler);	// ���� UART4 ���жϷ�����Ϊ UART4_IRQHandler
  uart_rx_irq_en(UART4);		// ������4�����ж�
  HMI_Send_End();		// ����һ�ν�����������ϵ�����Ĵ����Ӳ�����
  
  
  /************************ ��� ��� ��ʼ�� ************************************/
  //���PWM���Ƴ�ʼ��
  ftm_pwm_init(FTM3, FTM_CH2, 10000, 0);//���ֺ�
  ftm_pwm_init(FTM3, FTM_CH3, 10000, 0);//���ֺ�
  ftm_pwm_init(FTM3, FTM_CH4, 10000, 2000);//����ǰ
  ftm_pwm_init(FTM3, FTM_CH6, 10000, 2000);//����ǰ
  
    //���PWM���Ƴ�ʼ��
  ftm_pwm_init(FTM0, FTM_CH0, 50, 1550);
  
  /************************ �������� ***************************************/   
  ftm_quad_init(FTM1);	// ���������ʼ��
  ftm_quad_init(FTM2);	// ʹ�ö�ʱ��0������50msִ��һ��
  
  
  
  /************************ ����ͷ ��ʼ�� **********************************/
  camera_init(ImageBuff);                                   // ����ͷ��ʼ������ͼ��ɼ��� ImageBuff ��ַ
  set_vector_handler(PORTA_VECTORn ,PORTA_IRQHandler);	// ���� PORTA ���жϷ�����Ϊ PORTA_IRQHandler
  set_vector_handler(DMA0_VECTORn ,DMA0_IRQHandler);      // ���� DMA0 ���жϷ�����Ϊ DMA0_IRQHandler
  /************************ ����ģ�� NRF ��ʼ�� ****************************/
  if (0)
  {
    LCD_init();
    while(!nrf_init());
    set_vector_handler(PORTE_VECTORn ,PORTE_IRQHandler);	//���� PORTE ���жϷ�����Ϊ PORTE_VECTORn
    enable_irq(PORTE_IRQn);
    nrf_msg_init();                                        	//����ģ����Ϣ��ʼ��
    i = 20;	// ����ͷģ�飬��Ҫ ���� ������� ���԰�ģ�飬����������� ����ͼ��ʧ�ܶ�������������
    while(i--)
    {
      nrf_msg_tx(COM_RETRAN,nrf_tx_buff);               	//���Ͷ�� �� �����ȥ��������� ���ն˵Ļ�������
    }
  }
  /************************ ��ʱ�� ��ʼ��  *********************************/ 
  pit_init_ms(PIT0, 2);                              	// ��ʼ��PIT0����ʱʱ��Ϊ�� 2ms
  set_vector_handler(PIT0_VECTORn ,PIT0_IRQHandler);	// ����PIT0���жϷ�����Ϊ PIT0_IRQHandler
  enable_irq (PIT0_IRQn); 							// ʹ���ж�

  pit_init_ms(PIT1, 1000);                           	// ��ʼ��PIT1����ʱʱ��Ϊ�� 1s
  set_vector_handler(PIT1_VECTORn ,PIT1_IRQHandler); 	// ����PIT1���жϷ�����Ϊ PIT1_IRQHandler
  enable_irq (PIT1_IRQn); 							// ʹ���ж�
  /************************ ͼ��ɼ� ***************************************/
  
  camera_get_img();	// �Ȳɼ�һ��ͼ��
  
  /************************ LED ��ʼ��  ************************************/
  led_init(LED_MAX);
  led(LED_MAX,LED_ON);// ��LEDָʾ��		ȫ����ʼ���ɹ�
}


void data_send()
{
  Site_t site     = {0, 0};                           //��ʾͼ�����Ͻ�λ��
  Size_t imgsize  = {CAMERA_W, CAMERA_H};             //ͼ���С
  Size_t size     = {80, 60}; //{LCD_W, LCD_H};       //��ʾ����ͼ���С
  
  com_e     com;
  nrf_result_e nrf_result;
  uint8 imgtxflag = 0;    //0��ʾ����ͼ��
  
  /************************ ���߷��ͺͽ�������  ***********************/
  do
  {
    nrf_result = nrf_msg_rx(&com, nrf_rx_buff);
    if(nrf_result == NRF_RESULT_RX_VALID)
    {
      switch(com)
      {	
      case COM_IMG:
        //��ʾ �� ��Ļ��
#if     ( CAMERA_COLOR == 1 )                                       //�Ҷ�����ͷ
        LCD_Img_gray_Z(site,size, imgbuff,imgsize);
#elif   ( CAMERA_COLOR == 0 )                                       //�ڰ�����ͷ
        LCD_Img_Binary_Z(site,size, imgbuff,imgsize);
#endif
        break;
        
      case COM_VAR:
        if(last_tab == CAR_CTRL)                    //����� ���� С�� ����
        {
          switch(car_ctrl_get())                  //��ȡС��������������д���
          {
          case CAR_START:                         //����
            //�˴����� ���� ����
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
            
          case CAR_STOP:                          //ͣ��
            //�˴����� ͣ�� ���� �����磺�رյ���������ر�ʹ�ܣ�
            
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
            //��Ч���������κδ���
            break;
          }
        }			
        break;
        
      default:
        break;
      }
    }
  }while(nrf_result != NRF_RESULT_RX_NO);         //���ղ������� ���˳�
  
  if(!imgtxflag)
  {
    //��ͼ�� ���ͳ�ȥ
    nrf_msg_tx(COM_IMG,nrf_tx_buff);
  }
  
#if     ( CAMERA_COLOR == 1 )                                       //�Ҷ�����ͷ
  LCD_Img_gray_Z       (site,size, imgbuff,imgsize);
#elif   ( CAMERA_COLOR == 0 )                                       //�ڰ�����ͷ
  LCD_Img_Binary_Z(site,size, imgbuff,imgsize);
#endif
  
  if(!imgtxflag)
  {
    while(nrf_tx_state() == NRF_TXING);             //�ȴ��������
  }
}


void HMI_Data_Handle(void)
{
  char  UART0_Buff[3];
  
  if (uart_query (UART0) == 1)
  {
    uart_getchar(UART0, UART0_Buff);
    //�û���Ҫ�����������
    if (UART0_Buff[0] == 0x66)
    {			
      uart_putstr(UART0, "page Run");	//HMI��ת���˶�����
      HMI_Send_End();
      uart_putstr(UART0, "dim=10");	//������ʾ�����ȣ���Լ�õ�
      HMI_Send_End();
      System_OK = 1;					//���ν�������ʼ��ʱ����
      enable_irq (PIT1_IRQn);			//ʹ�ܶ�ʱ��1�����Զ�����׼��
    }
    else if (UART0_Buff[0] == 0x01)
    {			
      uart_putstr(UART0, "t2.txt=\"140\"");
      HMI_Send_End();
      Mode_Set = 1;	//����140
    }
    else if (UART0_Buff[0] == 0x02)
    {
      Mode_Set = 2;	//����150
      uart_putstr(UART0, "t2.txt=\"150\"");
      HMI_Send_End();
    }
    else if (UART0_Buff[0] == 0x03)
    {
      Mode_Set = 3;	//����160
      uart_putstr(UART0, "t2.txt=\"160\"");
      HMI_Send_End();
    }
    else if (UART0_Buff[0] == 0x04)
    {
      Mode_Set = 4;	//����170
      uart_putstr(UART0, "t2.txt=\"170\"");
      HMI_Send_End();
    }
    else if (UART0_Buff[0] == 0x05)
    {
      Mode_Set = 5;	//�ȶ�ģʽ2��3
      uart_putstr(UART0, "t2.txt=\"�ȶ�ģʽ\"");
      HMI_Send_End();
    }
    else if (UART0_Buff[0] == 0x06)
    {
      Mode_Set = 6;	//����ģʽ2��35
      uart_putstr(UART0, "t2.txt=\"����ģʽ\"");
      HMI_Send_End();
    }
    else if (UART0_Buff[0] == 0x07)
    {
      Mode_Set = 7;	//�蹷ģʽ������
      uart_putstr(UART0, "t2.txt=\"�蹷ģʽ\"");
      HMI_Send_End();
    }
    else if (UART0_Buff[0] == 0x11)
    {
      SCCB_WriteByte (OV7725_CNST, 50);	//��ֵ��Ϊ50������ʹ��
      uart_putstr(UART0, "t1.txt=\"50\"");
      HMI_Send_End();
    }
    else if (UART0_Buff[0] == 0x12)
    {
      SCCB_WriteByte (OV7725_CNST, 60);	//��ֵ��Ϊ60
      uart_putstr(UART0, "t1.txt=\"60\"");
      HMI_Send_End();
    }
    else if (UART0_Buff[0] == 0x13)
    {
      SCCB_WriteByte (OV7725_CNST, 40);	//��ֵ��Ϊ70���谵ʹ��
      uart_putstr(UART0, "t1.txt=\"40\"");
      HMI_Send_End();
    }
    UART0_Buff[0] = 0;
  }
}

void HMI_Send_End(void)		//����HMIָ�������
{
  uart_putchar(UART0, 0xff); //�Ӵ��ڶ�һ���ֽ����ݳ�ȥ
  uart_putchar(UART0, 0xff); //�Ӵ��ڶ�һ���ֽ����ݳ�ȥ
  uart_putchar(UART0, 0xff); //�Ӵ��ڶ�һ���ֽ����ݳ�ȥ
}
