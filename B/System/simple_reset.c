#include "simple_reset.h"
#include "stm32f10x_iwdg.h"
#include "Delay.h"

// 使用备份寄存器DR1作为标志
#define BKP_FLAG_ADDR  BKP_DR1
#define POWER_ON_FLAG  0xAA55

static void BKP_Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
    PWR_BackupAccessCmd(ENABLE);
}

static void IWDG_Init(uint16_t timeout_ms)
{
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_SetPrescaler(IWDG_Prescaler_64);
    
    uint16_t reload_value = (timeout_ms * 625UL) / 1000UL;
    if(reload_value > 0xFFF) reload_value = 0xFFF;
    
    IWDG_SetReload(reload_value);
    IWDG_ReloadCounter();
    IWDG_Enable();
}

void SimpleReset_Init(uint16_t delay_ms)
{
    BKP_Init();
    
    // 检查是否是第一次上电
    if(BKP_ReadBackupRegister(BKP_FLAG_ADDR) != POWER_ON_FLAG) {
        // 第一次上电
        BKP_WriteBackupRegister(BKP_FLAG_ADDR, POWER_ON_FLAG);
        
        // 等待指定时间
        if(delay_ms > 0) {
            Delay_ms(delay_ms);
        }
        
        // 软件复位
        NVIC_SystemReset();
    }
    
    // 初始化看门狗（2秒超时）
    IWDG_Init(2000);
}

void SimpleReset_FeedDog(void)
{
    IWDG_ReloadCounter();
}

void SimpleReset_Now(void)
{
    NVIC_SystemReset();
}
