/*
 * led.c
 *
 *  Created on: 2019年2月30日
 *      Author: zengwangfa
 *      Notes:  LED指示
 */
#define LOG_TAG    "led"

#include "led.h"
#include "flash.h"
#include "drv_ano.h"
#include "ioDevices.h"
#include <string.h>
#include <elog.h>
#include <drivers/pin.h>
#include <easyflash.h>
#include "sensor.h"
#include "EasyThread.h"
/*---------------------- Constant / Macro Definitions -----------------------*/

const uint8 inputdata[8] = {0x00,0x04,0x02,0x01,0x03,0x05,0x06,0x07};

/*----------------------- Variable Declarations -----------------------------*/


Bling_Light Light_Red,Light_Green,Light_Blue;

uint8 Bling_Mode = 0;
/*----------------------- Function Implement --------------------------------*/

int led_thread_init(void)
{
    rt_thread_t led_tid;
		/*创建动态线程*/
    led_tid = rt_thread_create("led",//线程名称
                    led_thread_entry,				 //线程入口函数【entry】
                    RT_NULL,							   //线程入口函数参数【parameter】
                    1024,										 //线程栈大小，单位是字节【byte】
                    15,										 	 //线程优先级【priority】
                    10);										 //线程的时间片大小【tick】= 100ms

    if (led_tid != RT_NULL){
				rt_pin_mode(LED_Red, 	 PIN_MODE_OUTPUT);//设置输出模式	
				rt_pin_mode(LED_Green, PIN_MODE_OUTPUT);	
				rt_pin_mode(LED_Blue,  PIN_MODE_OUTPUT);
				rt_pin_mode(LED_Camera,PIN_MODE_OUTPUT); //OV2640 LED
			
				rt_pin_mode(Light_PIN, 	PIN_MODE_OUTPUT);	  //探照灯
				rt_pin_write(Light_PIN ,PIN_LOW);
				log_i("LED_Init()");
				//rt_event_send(&init_event, LED_EVENT);
				rt_thread_startup(led_tid);
		}
		return 0;
}
INIT_APP_EXPORT(led_thread_init);




	

/* 系统初始化led闪烁状态【显示7种颜色】 -->[颜色节拍表> 空  红  绿  蓝  青  粉  黄  白] */
void system_init_led_blink(void)
{
		static uint8 i=0;/*颜色节拍表> 空   红   绿   蓝   青   粉   黄   白 */

		do{
				if(inputdata[i] & 0x04){	
								LED_ON(LED_Red); }
				else{ 	LED_OFF(LED_Red);}
				
				if(inputdata[i] & 0x02){	
							LED_ON(LED_Green); }
				else{ LED_OFF(LED_Green);}
				
				if(inputdata[i] & 0x01){	
							 LED_ON(LED_Blue); }
				else{  LED_OFF(LED_Blue);}
				rt_thread_mdelay(300);//等待系统初始化  完毕，系统稳定后，在读取数据
		}while((i++) <= 7);//闪烁8中颜色
		
		ALL_LED_OFF();//关闭RGB
}


/***************************************************
函数名: void Bling_Set(Bling_Light *Light,
uint32_t Continue_time,//持续时间
uint16_t Period,//周期100ms~1000ms
float Percent,//0~100%
uint16_t  Cnt,
uint16_t Pin
,uint8_t Flag)
说明:	状态指示灯设置函数
入口:	时间、周期、占空比、端口等
出口:	无
备注:	程序初始化后、始终运行
****************************************************/
void Bling_Set(Bling_Light *Light,
               uint32 Continue_time,//持续时间
               uint32 Period,//周期100ms~1000ms
               float Percent,//0~100%
               uint32  Cnt,
               uint32 Pin,
               uint8 Flag)
{
		Light->Bling_Contiune_Time=(Continue_time/5);//持续时间
		Light->Bling_Period=Period;//周期
		Light->Bling_Percent=Percent;//占空比
		Light->Pin=Pin;//引脚
		Light->Endless_Flag=Flag;//无尽模式
}

/***************************************************
函数名: void Bling_Process(Bling_Light *Light)//闪烁运行线程
说明:	状态指示灯实现
入口:	状态灯结构体     
出口:	无
备注:	程序初始化后、始终运行
****************************************************/
void Bling_Process(Bling_Light *Light)//闪烁运行线程 Cnt 
{
		if(Light->Bling_Contiune_Time>=1) { 
				Light->Bling_Contiune_Time--;
		}
		else {LED_ON(Light->Pin);}//亮
		if(Light->Bling_Contiune_Time != 0//总时间未清0
				||Light->Endless_Flag==1)//判断无尽模式是否开启
		{
				Light->Bling_Cnt++;
				if(5*Light->Bling_Cnt>=Light->Bling_Period){
						Light->Bling_Cnt=0;//计满清零
				}
				if(5*Light->Bling_Cnt <= Light->Bling_Period * Light->Bling_Percent){	
						LED_ON(Light->Pin);   //亮
				}
				else {LED_OFF(Light->Pin);}//灭
		}
		else {	
				LED_OFF(Light->Pin);		//高电平 【熄灭】		
		}
}



/***************************************************
函数名: Bling_Working(uint16 bling_mode)
说明:	状态指示灯状态机
入口:	当前模式
出口:	无
备注:	程序初始化后、始终运行
****************************************************/
void Bling_Working(uint8 bling_mode)
{
		if(0 == bling_mode)
		{
				Bling_Process(&Light_Red);
				Bling_Process(&Light_Green);
				Bling_Process(&Light_Blue);
		}
		else if(1 == bling_mode)//
		{
				Bling_Process(&Light_Red);
		}
		else if(2 == bling_mode)//
		{
				Bling_Process(&Light_Green);
		}
		else if(3 == bling_mode)//全灭
		{
				Bling_Process(&Light_Blue);
		}
		 
}



/* led on MSH方法 */
int led_on(int argc, char **argv)
{
    int result = 0;

    if (argc != 2){
        log_e("Error! Proper Usage: led_on r\n Species:r/g/b/c/l");
				result = -RT_ERROR;
				return result;
    }
		
		switch(*argv[1]){
				case 'r':LED_ON(LED_Red);break;
				case 'g':LED_ON(LED_Green);break;
				case 'b':LED_ON(LED_Blue);break;
				case 'c':LED_OFF(LED_Camera);break;
				case 'l':LED_OFF(Light_PIN);break;
				default:log_e("Error! Proper Usage: led_on R\n Species:r/g/b/c/l");break;
		}

		return result;
}
MSH_CMD_EXPORT(led_on,ag: led_on r  );


/* led off MSH方法 */
int led_off(int argc, char **argv)
{
    int result = 0;

    if (argc != 2){
        log_e("Error! Proper Usage: led_off r\n Species:r/g/b/c/l");
				result = -RT_ERROR;
        goto _exit;
    }
		
		switch(*argv[1]){
			
				case 'r':LED_OFF(LED_Red);break;
				case 'g':LED_OFF(LED_Green);break;
				case 'b':LED_OFF(LED_Blue);break;
				case 'c':LED_ON(LED_Camera);break;
				case 'l':LED_ON(Light_PIN);break;
				default:log_e("Error! Proper Usage: led_off r\n Species:r/g/b/c/l");break;
		}
_exit:
    return result;
}
MSH_CMD_EXPORT(led_off,ag:led_off r);


void ErrorStatus_LED(void)
{
		LED_ON(LED_Red);			//初始化为高电平 【熄灭】
		LED_OFF(LED_Green);			
		LED_OFF(LED_Blue);
}

void ALL_LED_OFF(void)
{
		LED_OFF(LED_Red);			//初始化为高电平 【熄灭】
		LED_OFF(LED_Green);			
		LED_OFF(LED_Blue);
}
