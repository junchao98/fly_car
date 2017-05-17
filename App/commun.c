
#include "commun.h"

char isr_dat[DATA_LEN] = {0};

uint32 get_distance(void)
{
  

    
    uint8 i=0;
    uint32 distance=0;
    
    uart_querybuff(UART1,isr_dat,8);   //��ȡ����Ϊ3��u8 [0] ��У  [1]  [2] ����
    
    for(i=0;i<DATA_LEN;i++){
    
      if(isr_dat[i] == 0xa5){   //���ͷ֡�Ƿ���ȷ������ȷ�����½���
        
           distance = isr_dat[i+1]<<8 | isr_dat[i+2];  //������ɣ���ʼ��������
           return distance;
      
      }
    
      
    }
    
    
    return 0;
  }
   


