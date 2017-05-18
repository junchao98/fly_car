#include "common.h"
#include "include.h"


#include "commun.h"



uint8 ImageBuff[CAMERA_W*CAMERA_H/8];							// ͼ��ɼ������ַ
uint8 *imgbuff = (uint8 *)(((uint8 *)&nrf_tx_buff) + COM_LEN);	// ͼ���ַ��������λ���۲�
uint8 img[CAMERA_W*CAMERA_H]; //����ӥ������ͷ��һ�ֽ�8�����أ������Ҫ��ѹΪ 1�ֽ�1�����أ����㴦��
int cen = 0;
int flag = 0;
int rd0 = 775;
int a=0;


/*********** main����***********/
void main(void)
{  
  uint16 rud = 775;
  
  System_Init();		//��ʼ������ģ��
  
  SCCB_WriteByte (OV7725_CNST, 60);	//�ı�ͼ����ֵ
  
  while(1)
  {   
    a++;
    
    if(a==900)a=0;
    
    /************************ ͼ��ɼ�����ʾ  ***********************/
    while (!Image_Flag);
    Image_Flag = 0;
    img_extract(img, ImageBuff,CAMERA_SIZE);	// ��ѹ
    camera_get_img();                           // ����ͷ��ȡͼ��
    /************************* ͼ���㷨���� ************************/
    if (!Annulus_Flag)
    {
      Image_Handle(img);			// ��ͨͼ����
    }
    if (Annulus_Flag)
    {
     // Image_Handle(img);
      Annulus_Handle(img);		// ��·ר��ͼ����
    }
    Difference = Area_Calculate();      
    pid_rudder(Difference,&rud,flag); 
    ftm_pwm_duty(FTM0, FTM_CH0, rud);//���¶������PWM 
    
    
    //main_test = get_distance();

    
    
    /********************** HMI���������շ�   **********************/ 
    if (0)
    {
      HMI_Data_Handle();
    }
    /************************ �������ݷ���   ***********************/  
    if (0) //ѡ���Ƿ���ͼ����Ϣ����λ��
    {
      Send_OK = 0;
      img_recontract(img, imgbuff,CAMERA_SIZE); //ͼ��ѹ��
      data_send();	                          //��������λ��
    }
  }
}











