#include "stm32f10x.h"                  // Device header

#include "Delay.h"
#include "Timer.h"

#include "OLED.h"
#include "Key.h"

/**
  * 坐标轴定义：
  * 左上角为(0, 0)点
  * 横向向右为X轴，取值范围：0~127
  * 纵向向下为Y轴，取值范围：0~63
  * 
  *       0             X轴           127 
  *      .------------------------------->
  *    0 |
  *      |
  *      |
  *      |
  *  Y轴 |
  *      |
  *      |
  *      |
  *   63 |
  *      v
  * 
  */

int main(void)
{
	OLED_Init();
	Key_Init();
	
	Timer_Init();
	
/* =================== [START] 菜单初始化模块 [START] =================== */	
	char Main_Menu[][10] = {"ADC", "Store", "IMU"};
	uint16_t Value_ADC_Mode[] = {1, 2, 3};
	uint16_t Value_Store_Mode[] = {4, 5, 6};
	uint16_t Value_IMU_Mode[] = {7, 8, 9};
	#define Flag_Mian_Menu				0
	#define Flag_ADC_Mode				1
	#define Flag_Store_Mode			2
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
						OLED_Clear();
						OLED_Printf(8, 16, OLED_8X16, "POT   val:%d",Value_ADC_Mode[0]);
						OLED_Printf(8, 32, OLED_8X16, "NTC   val:%d",Value_ADC_Mode[1]);
						OLED_Printf(8, 48, OLED_8X16, "LDR   val:%d",Value_ADC_Mode[2]);
						OLED_Update();
					
						break;
					
					case Flag_Store_Mode:
						OLED_Clear();
						OLED_Printf(8, 16, OLED_8X16, "POT   val:%d",Value_Store_Mode[0]);
						OLED_Printf(8, 32, OLED_8X16, "NTC   val:%d",Value_Store_Mode[1]);
						OLED_Printf(8, 48, OLED_8X16, "LDR   val:%d",Value_Store_Mode[2]);
						OLED_Update();
					
					
						break;
					
					case Flag_IMU_Mode:
						OLED_Clear();
						OLED_Printf(8, 16, OLED_8X16, "Pitch:%d",Value_IMU_Mode[0]);
						OLED_Printf(8, 32, OLED_8X16, "Roll:%d",Value_IMU_Mode[1]);
						OLED_Printf(8, 48, OLED_8X16, "Yaw:%d",Value_IMU_Mode[2]);
						OLED_Update();
					
					
						break;
					
					
					
				}
				
			}
			else
			{
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
			// 数据保存逻辑
		}
	
		

	}
}

//1ms的定时器定时中断
void TIM1_UP_IRQHandler(void)
{
	//检查标志位
	if (TIM_GetITStatus(TIM1,TIM_IT_Update) == SET )
	{
		Key_Tick();
		//清除标志位
		TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
	}
}
