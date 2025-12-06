ASC-EC-FourthCheck
ASC智能车实验室电控组第四轮考核任务

A机
=================================================
目前的引脚+定时器分配：
[TIM定时器通道分配]
TIM1                    定时器定时中断


[OLED]
(软件I2C)
PB6                     SCK(SCL)
PB7                     SDA(SDA)

[按键模块]
(TIM1定时器定时扫描)
PB8                    UP
PB9                    DOWN
PB5                     CONFIRM

[传感器模块]
PA0                     (POT电位器)
PA1                     (NTC热敏电阻)A0
PA2                     (LDR光敏电阻)A0

[W25Q64存储模块]
PB0                     SS
PB1                     SCK
PB10                    MISO
PB11                    MOSI
=================================================