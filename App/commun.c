
#include "commun.h"

char isr_dat[DATA_LEN] = {0};

uint32 get_distance(void)
{
  

    
    uint8 i=0;
    uint32 distance=0;
    
    uart_querybuff(UART1,isr_dat,8);   //获取长度为3的u8 [0] 验校  [1]  [2] 数据
    
    for(i=0;i<DATA_LEN;i++){
    
      if(isr_dat[i] == 0xa5){   //检查头帧是否正确，不正确就重新接收
        
           distance = isr_dat[i+1]<<8 | isr_dat[i+2];  //接收完成，开始处理数据
           return distance;
      
      }
    
      
    }
    
    
    return 0;
  }
   


