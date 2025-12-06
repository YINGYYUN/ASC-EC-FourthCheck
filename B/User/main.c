#include "stm32f10x.h"                  // Device header

#include "Delay.h"
#include "Timer.h"
#include "Serial.h"
#include "MPU6050.h"

#include <math.h>

int16_t AX, AY, AZ, GX, GY, GZ;

float RollAcc;    		// 加速度计计算的横滚角
float RollGyro;   		// 陀螺仪积分的横滚角
float Roll;       		// 融合后的横滚角

float Yaw = 0;			//偏航角

float PitchAcc;			//加速度计算的俯仰角
float PitchGyro;		//陀螺仪积分的俯仰角
float Pitch;			//融合后的俯仰角	

float Roll_Sum, Yaw_Sum, Pitch_Sum;
float Roll_Result, Yaw_Result, Pitch_Result;
uint8_t Sum_Count = 0;

int main(void)
{
	Serial_Init();
	MPU6050_Init();
	
	Timer_Init();
	

	while(1)
	{
		
	}
	
}

uint16_t TimeTick = 0;
//1ms的定时中断
void TIM1_UP_IRQHandler(void)
{
	//检查标志位
	if (TIM_GetITStatus(TIM1,TIM_IT_Update) == SET )
	{
		TimeTick ++;
		if (TimeTick == 50)
		{
			Serial_Printf("@D,%.4f,%.4f,%.4f\r\n", Pitch, Roll, Yaw);
		}
		if (TimeTick >= 100)
		{
			Serial_Printf("@H\r\n");		
			TimeTick = 0;
		}
		//清除标志位
		TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
		
		
		//保证数据的及时读取
		MPU6050_GetData(&AX, &AY, &AZ, &GX, &GY, &GZ);
		
		
		//校准零飘
		GX += 10;
		GY -= 8;
		GZ += 38;
	
		if (-2 < GX && GX < 2)GX = 0;
		if (-2 < GY && GY < 2)GY = 0;
		
//		Serial_Printf("%d,%d,%d\r\n",GX,GY,GZ);
		
		// 横滚角计算
		RollAcc = atan2(AY, AZ) / 3.14159 * 180;  				// 横滚角（绕X轴）
		RollGyro = Roll + GX / 32768.0 * 2600 * 0.001;
//		RollGyro = Roll + GX / 32768.0 * 2000 * 0.001;  		// 陀螺仪X轴积分
		Roll = 0.001 * RollAcc + (1 - 0.001) * RollGyro;  		// 相同互补滤波算法
		
		// 偏航角：仅陀螺仪积分（无加速度计校准，会漂移）
		if (GZ <= -2 || 2 <= GZ){Yaw += GZ / 32768.0 * 2600 * 0.001;}
//		if (GZ <= -2 || 2 <= GZ){Yaw += GZ / 32768.0 * 2000 * 0.001;}

		// 俯仰角计算
		PitchAcc = -atan2(AX, AZ) / 3.14159 * 180;  			// 俯仰角（绕Y轴）
		PitchGyro = Pitch + GY / 32768.0 * 2600 * 0.001;
//		PitchGyro = Pitch + GY / 32768.0 * 2000 * 0.001;  		// 陀螺仪积分（2000是量程，0.001是1ms采样间隔）
		Pitch = 0.001 * PitchAcc + (1 - 0.001) * PitchGyro;  	// 互补滤波

	}
}
