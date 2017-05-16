#ifndef __MY_I2C_H__
#define __MY_I2C_H__
#include "common.h"
#include "include.h"

#define My_SCL	PTD8	// ����SCL����
#define My_SDA	PTD9	// ����SDA����

#define ack 1      //��Ӧ��
#define no_ack 0   //��Ӧ��

typedef enum IIC
{
    IIC,
    SCCB
} IIC_type;

#define SDA        	gpio_get (My_SDA)
#define SDA0()     	gpio_set (My_SDA, 0)	// IO������͵�ƽ
#define SDA1()     	gpio_set (My_SDA, 1)	// IO������ߵ�ƽ  
#define SCL0()    	gpio_set (My_SCL, 0)	// IO������͵�ƽ
#define SCL1()		gpio_set (My_SCL, 1)	// IO������ߵ�ƽ
#define DIR_OUT()	gpio_ddr (My_SDA, GPO)	// �������
#define DIR_IN()   	gpio_ddr (My_SDA, GPI)	// ���뷽��

void  IIC_init(void);
void  IIC_start(void);
void  IIC_stop(void);
void  IIC_ack_main(uint8 ack_main);
void  send_ch(uint8 c);
uint8 read_ch(void);
void  simiic_write_reg(uint8 dev_add, uint8 reg, uint8 dat);
uint8 simiic_read_reg(uint8 dev_add, uint8 reg, IIC_type type);
uint8 simiic_write_len(uint8 dev_add, uint8 reg, uint8 len, uint8 *dat);
uint8 simiic_read_len(uint8 dev_add, uint8 reg, uint8 len, uint8 *dat);

#endif
