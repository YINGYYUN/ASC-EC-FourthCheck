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
	#define F_Mian_Menu				0
	#define F_ADC_Mode				1
	#define F_Store_Mode			2
	#define F_IMU_Mode			2
	//光标位置
	uint8_t Main_Menu_Location = F_ADC_Mode;
	//功能状态
	uint8_t FUNCTION_State = F_Mian_Menu;
	
	OLED_Clear();
	
	OLED_Printf(16, 0, OLED_8X16, Main_Menu[0]);
	OLED_Printf(16, 16, OLED_8X16, Main_Menu[1]);
	OLED_Printf(16, 32, OLED_8X16, Main_Menu[2]);
	
	OLED_Printf(0, 16 * (Main_Menu_Location - 1) , OLED_8X16, ">");
	
	OLED_Update();
	
/* =================== [END] 菜单初始化模块 [END] =================== */	
	
	uint16_t i = 0;
	
	while (1)
	{	
		// 上键单击：菜单上移
		if(Key_Check(KEY_NAME_UP, KEY_SINGLE))
		{
			// 上移逻辑
			i += 1000;
		}
	
		// 下键单击：菜单下移
		else if(Key_Check(KEY_NAME_DOWN, KEY_SINGLE))
		{
			// 下移逻辑
			i += 100;
		}
		
		if(Key_Check(KEY_NAME_CONFIRM, KEY_SINGLE))
		{
		// 菜单切换逻辑
			i += 1;
		}	
		
		// 确认键长按：保存数据（复用模板的KEY_LONG标志位）
		else if(Key_Check(KEY_NAME_CONFIRM, KEY_LONG))
		{
			// 数据保存逻辑
			i += 10;
		}
	
		
		
		
		
		OLED_Printf(0, 48, OLED_8X16, "%d", i);
		OLED_Update();
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
