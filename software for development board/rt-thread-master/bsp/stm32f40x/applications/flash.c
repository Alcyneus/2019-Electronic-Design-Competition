/*
 * flash.c
 *
 *  Created on: 2019��2��30��
 *      Author: zengwangfa
 *      Notes:  Flash��д����
 */
#define LOG_TAG       "flash"


#include <math.h>
#include <easyflash.h>
#include <rtthread.h>
#include <elog.h>

#include "oled.h"
#include "Control.h"
#include "PID.h"
#include "debug.h"
#include "flash.h"
#include "sensor.h"
#include "drv_ano.h"
#include "servo.h"
#include "propeller.h"
#include "PropellerControl.h"
#include "gyroscope.h"
#include "EasyThread.h"
#include <stdio.h>
#include "HMI.h"
#include "DataProcess.h"
/*---------------------- Constant / Macro Definitions -----------------------*/

//FLASH��ʼ��ַ   W25Q128 16M ��������Ϊ 256 ���飨Block��
#define Nor_FLASH_ADDRESS    (0x0000) 	//W25Q128 FLASH�� ��ͨ��ʼ��ַ   ���ڸ�������

#define IMP_FLASH_ADDRESS    (0x1000) 	//W25Q128 FLASH�� ��Ҫ������ʼ��ַ ���ڸ�������

/*----------------------- Variable Declarations -----------------------------*/

uint32 Normal_Parameter[100]={0};





/*----------------------- Function Implement --------------------------------*/



/*******************************************
* �� �� ����Normal_Parameter_Init_With_Flash
* ��    �ܣ���ȡFlash��ͨ�������ǳ���Ҫ��
* ���������none
* �� �� ֵ��none
* ע    �⣺NORMAL_PARAMETER_TABLE ö�ٱ� ������������Ҫ�ĵĲ����� ö��ֵ
						NORMAL_PARAMETER_TABLE ö�ٱ� ������������Ҫ�ĵĲ���

********************************************/
int Normal_Parameter_Init_With_Flash(void)
{
		uint8 i = 0;
		for(i = 0;i < 100 ;i++ ){
				ef_port_read(Nor_FLASH_ADDRESS+4*i,(Normal_Parameter+i),4);		 //Flash��ȡ
		}
		ef_port_read(Nor_FLASH_ADDRESS+4*100,(uint32 *)&KT_Board_Value_In_Flash,4);		 //Flash��ȡ
		ef_port_read(Nor_FLASH_ADDRESS+4*101,(uint32 *)&Fiber_Board_Value_In_Flash,4);		 //Flash��ȡ
		
		for(int i = 0;i < 100 ;i++){
				Parameter_SelfCheck( (uint32 *)&FDC2214_Data_In_Flash[i],&Normal_Parameter[i] );//����������� 3s/4s/6s
		}
		log_i("Flash_Read()");
		log_i("                      ----------");
		log_i("debug_tool            |%s     |",debug_tool_name[debug_tool]);
		log_i("                      ----------");
		return 0;
}




/* FLASH ���� ��ͨ���� */
void Flash_Update(void)
{
		ef_port_erase(Nor_FLASH_ADDRESS,4);	//����ͨ����FLASH���Ȳ���д  ������Ϊһ������4096 Byte 

		for(int i = 0;i <100;i++){
				ef_port_write(Nor_FLASH_ADDRESS + 4*(i) ,(uint32 *)&FDC2214_Data_In_Flash[i],4); //
		}
		ef_port_write(Nor_FLASH_ADDRESS + 4*(100) ,(uint32 *)&KT_Board_Value_In_Flash,4); //����������� 3s/4s/6s
		ef_port_write(Nor_FLASH_ADDRESS + 4*(101) ,(uint32 *)&Fiber_Board_Value_In_Flash,4); //����������� 3s/4s/6s
		
}	


/* FLASH ���� ��ͨ���� */
//void Capacity_Flash_Update(float array[],uint8 number)
//{
//		float temp = 0.0f;
//		char str[20];
//	
//		sprintf(str,"[flash] NO.%d:%f\n",number,array[number]);
//		rt_kprintf(str);

//	
//		
//		ef_port_read( Nor_FLASH_ADDRESS+4*BATTERY_CAPACITY_e + 4*(number+1) ,(uint32 *)&temp,4);		 //Flash��ȡ
//	
//		sprintf(str,"[flash] NO.%d:%f\n",number,temp);
//		rt_kprintf(str);
//}	


/* list �����Ҫ���� */
void list_value(void)
{	
		
		static char str[30] = {0};
    log_i("---------------------- ");
		for(int i = 0;i < 50; i++){
				sprintf(str,"NO.%d : %f ",i,FDC2214_Data_In_Flash[i]);
				log_i(str);
		}
    log_i("----------------------");
		
    rt_kprintf("\n");
}
MSH_CMD_EXPORT(list_value,list some important values);





void Parameter_SelfCheck(uint32 *RealParameter,uint32 *TempParameter)
{
		//int isnan(x)���� ��xʱnan����1����������0
		if( isnan(*TempParameter) == 0 ) {//���������Ч���� not a number,���ж�Ϊ��ȷ
				*RealParameter = *TempParameter; //Flash ������ȷ���滻Ϊ��ʵ����
		}
		else{
				*RealParameter = 0;
		}
}


