#ifndef __FOCUS_H_
#define __FOCUS_H_

#include "DataType.h"



void Focus_Zoom_Camera_Control(uint8 *action);
void Camera_Focus_Data_Analysis(uint8 Data); //控制数据解析


int get_target_x(void); //0为绿,1为红

int get_target_y(void);

int get_persent_x(void); //0为绿,1为红

int get_persent_y(void);
#endif



