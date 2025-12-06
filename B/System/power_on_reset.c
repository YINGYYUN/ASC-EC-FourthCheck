#include "power_on_reset.h"
#include "stm32f10x_pwr.h"
#include "stm32f10x_rcc.h"
#include "Delay.h"

// 使用备份寄存器作为标志
#define RESET_FLAG_REG  BKP_DR1
#define RESET_FLAG_VAL  0x5A5A

void PowerOnReset_Once(void)
{
    // 使能备份寄存器时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
    PWR_BackupAccessCmd(ENABLE);
    
    // 检查标志
    if(BKP_ReadBackupRegister(RESET_FLAG_REG) != RESET_FLAG_VAL) {
        // 第一次上电：设置标志
        BKP_WriteBackupRegister(RESET_FLAG_REG, RESET_FLAG_VAL);
        
        // 等待1秒让系统稳定
        Delay_ms(1000);
        
        // 复位
        NVIC_SystemReset();
    }
    // 第二次（复位后）运行：清除标志，恢复正常
    else {
        BKP_WriteBackupRegister(RESET_FLAG_REG, 0x0000);
    }
}
