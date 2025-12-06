#include "stm32f10x.h"                  // Device header

#include "Delay.h"
#include "Timer.h"

#include "OLED.h"
#include "Key.h"
#include "AD.h"
#include "W25Q64.h"

#include <string.h>
#include <math.h>
#include <stdlib.h>

uint16_t Value_ADC_Mode[] = {0, 0, 0};
uint16_t Value_Store_Mode[] = {4, 5, 6};
uint16_t Value_IMU_Mode[] = {7, 8, 9};
uint8_t ADC_READ_ENABLE = 0;

// 临时字节缓冲区：uint16_t ↔ uint8_t 转换（3个uint16_t=6字节）
uint8_t Byte_Buf_Write[6];
uint8_t Byte_Buf_Read[6];

int main(void)
{
	OLED_Init();
	Key_Init();
	AD_Init();
	W25Q64_Init();
	
	Timer_Init();
	
/* =================== [START] 菜单初始化模块 [START] =================== */	
	char Main_Menu[][10] = {"ADC", "Store", "IMU"};

	#define Flag_Mian_Menu				0
	#define Flag_ADC_Mode				1
	#define Flag_Store_Mode				2
	#define Flag_IMU_Mode				3
	//光标位置
	uint8_t Main_Menu_Location = Flag_ADC_Mode;
	//功能状态
	uint8_t FUNCTION_State = Flag_Mian_Menu;
	
	OLED_Clear();
	OLED_Printf(8, 0, OLED_8X16, Main_Menu[0]);
	OLED_Printf(8, 16, OLED_8X16, Main_Menu[1]);
	OLED_Printf(8, 32, OLED_8X16, Main_Menu[2]);
	OLED_Printf(0, 16 * (Main_Menu_Location - 1) , OLED_8X16, ">");	
//	OLED_Printf(112, 0, OLED_8X16, "+");
	OLED_Update();	
/* =================== [END] 菜单初始化模块 [END] =================== */	
	
	while (1)
	{	
		/* =================== [START] 按键响应及菜单更新模块 [START] =================== */	
		// 上键单击：菜单上移
		if(Key_Check(KEY_NAME_UP, KEY_SINGLE))
		{
			if (FUNCTION_State == Flag_Mian_Menu)
			{
				OLED_Printf(0, 16 * (Main_Menu_Location - 1) , OLED_8X16, " ");
				Main_Menu_Location = (Main_Menu_Location - 1 - 1 + 3) % 3 + 1;
				OLED_Printf(0, 16 * (Main_Menu_Location - 1) , OLED_8X16, ">");			
				OLED_Update();
			}
		}
	
		// 下键单击：菜单下移
		else if(Key_Check(KEY_NAME_DOWN, KEY_SINGLE))
		{
			if (FUNCTION_State == Flag_Mian_Menu)
			{
				OLED_Printf(0, 16 * (Main_Menu_Location - 1) , OLED_8X16, " ");
				Main_Menu_Location = (Main_Menu_Location - 1 + 1) % 3 + 1;
				OLED_Printf(0, 16 * (Main_Menu_Location - 1) , OLED_8X16, ">");		
				OLED_Update();
			}
		}
		
		else if(Key_Check(KEY_NAME_CONFIRM, KEY_SINGLE))
		{
			if (FUNCTION_State == Flag_Mian_Menu)
			{
				FUNCTION_State = Main_Menu_Location;
				
				//显示更新
				switch(FUNCTION_State)
				{
					case Flag_ADC_Mode:
						ADC_READ_ENABLE = 1;
					
						OLED_Clear();
						OLED_Printf(8, 16, OLED_8X16, "POT val:%d",Value_ADC_Mode[0]);
						OLED_Printf(8, 32, OLED_8X16, "NTC val:%d",Value_ADC_Mode[1]);
						OLED_Printf(8, 48, OLED_8X16, "LDR val:%d",Value_ADC_Mode[2]);
						OLED_Update();
									
						break;
					
					case Flag_Store_Mode:
						
					    W25Q64_ReadData(0x000000, Byte_Buf_Read, 6);
    
						// 合并字节流到uint16_t
						Value_Store_Mode[0] = (uint16_t)Byte_Buf_Read[1] << 8 | Byte_Buf_Read[0];
						Value_Store_Mode[1] = (uint16_t)Byte_Buf_Read[3] << 8 | Byte_Buf_Read[2];
						Value_Store_Mode[2] = (uint16_t)Byte_Buf_Read[5] << 8 | Byte_Buf_Read[4];
									
						OLED_Clear();
						OLED_Printf(8, 16, OLED_8X16, "POT val:%d",Value_Store_Mode[0]);
						OLED_Printf(8, 32, OLED_8X16, "NTC val:%d",Value_Store_Mode[1]);
						OLED_Printf(8, 48, OLED_8X16, "LDR val:%d",Value_Store_Mode[2]);
						OLED_Update();
					
														
						break;
					
					case Flag_IMU_Mode:
						OLED_Clear();
						OLED_Printf(8, 16, OLED_8X16, "Pitch:%d",Value_IMU_Mode[0]);
						OLED_Printf(8, 32, OLED_8X16, "Roll:%d",Value_IMU_Mode[1]);
						OLED_Printf(8, 48, OLED_8X16, "Yaw:%d",Value_IMU_Mode[2]);
						OLED_Update();					
					
						break;		

					default:
						
						break;
				}
				
			}
			else
			{	
				ADC_READ_ENABLE = 0;
				FUNCTION_State = Flag_Mian_Menu;
				
				//显示更新
				OLED_Clear();
				OLED_Printf(8, 0, OLED_8X16, Main_Menu[0]);
				OLED_Printf(8, 16, OLED_8X16, Main_Menu[1]);
				OLED_Printf(8, 32, OLED_8X16, Main_Menu[2]);			
				OLED_Printf(0, 16 * (Main_Menu_Location - 1) , OLED_8X16, ">");			
				OLED_Update();	
			}
		// 菜单切换逻辑
			
		}	
		
		// 确认键长按：保存数据（复用模板的KEY_LONG标志位）
		else if(Key_Check(KEY_NAME_CONFIRM, KEY_LONG))
		{
			if(FUNCTION_State == Flag_ADC_Mode)
			{
				//数据保存
				// 拆分3个uint16_t为6个字节（小端模式：低字节在前，高字节在后）
				Byte_Buf_Write[0] = Value_ADC_Mode[0] & 0xFF;        // POT低字节
				Byte_Buf_Write[1] = (Value_ADC_Mode[0] >> 8) & 0xFF; // POT高字节
				Byte_Buf_Write[2] = Value_ADC_Mode[1] & 0xFF;        // NTC低字节
				Byte_Buf_Write[3] = (Value_ADC_Mode[1] >> 8) & 0xFF; // NTC高字节
				Byte_Buf_Write[4] = Value_ADC_Mode[2] & 0xFF;        // LDR低字节
				Byte_Buf_Write[5] = (Value_ADC_Mode[2] >> 8) & 0xFF; // LDR高字节
				
				W25Q64_SectorErase(0x000000);					// 擦除0号扇区（模板原有函数）
				W25Q64_PageProgram(0x000000, Byte_Buf_Write, 6);	// 写入6字节（替换模板的4字节）
			}		
		}
		/* =================== [END] 按键响应及菜单更新模块 [END] =================== */	
		
		
		
		
		/* =================== [START] 功能响应及更新性质模块 [START] =================== */	
		switch(FUNCTION_State)
		{
			case Flag_Mian_Menu:
			
				break;
			
			case Flag_ADC_Mode:
			{
				//读取在定时器定时中断里面了
//				float Voltage = (float)Value_ADC_Mode[0] / 4095 * 3.3;	

//				OLED_Printf(72, 16, OLED_8X16, "%1.3f V",Voltage);
				OLED_Printf(72, 16, OLED_8X16, "%d",Value_ADC_Mode[0]);
				OLED_Printf(72, 32, OLED_8X16, "%d",Value_ADC_Mode[1]);
				OLED_Printf(72, 48, OLED_8X16, "%d",Value_ADC_Mode[2]);
				OLED_Update();
								
				break;
			}
			
			case Flag_Store_Mode:
			
				break;
			
			case Flag_IMU_Mode:
			
				break;
			
			default:
				
				break;
			
		}
		/* =================== [END] 功能响应及更新性质模块 [END] =================== */	
		
		
		
		
	}
}

uint16_t Timetick = 0;
//1ms的定时器定时中断
void TIM1_UP_IRQHandler(void)
{
	//检查标志位
	Timetick ++;
	if (Timetick >= 50)
	{
		Timetick = 0;
		if (ADC_READ_ENABLE){
			AD_GetValue();
			memcpy(Value_ADC_Mode, AD_Value, 6);
		}
	}
	
	if (TIM_GetITStatus(TIM1,TIM_IT_Update) == SET )
	{
		Key_Tick();
		//清除标志位
		TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
	}
}
