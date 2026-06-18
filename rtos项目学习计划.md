# 项目学习计划制定

| Day    | 学习小步                                              | 对应模块                                   |      |
| ------ | ----------------------------------------------------- | ------------------------------------------ | ---- |
| Day 1  | 2.4 MQ2 与 ADC 采集                                   | `MQ2`、ADC                                 |      |
| Day 2  | 2.5 LCD 与 SPI 显示                                   | `LCD`、`SPI`                               |      |
| Day 3  | 2.6 TIM1 PWM 与直流有刷电机驱动（一）                 | `MOTOR`                                    |      |
| Day 4  | 2.7 TIM1 PWM 与直流有刷电机驱动（二）+ 编码器测速入门 | `MOTOR`                                    |      |
| Day 5  | 2.8 TIM2 编码器测速 + TIM4 定时测速                   | `MOTOR`                                    |      |
| Day 6  | 3.1 PID 算法 + PID 代码 + 电机闭环                    | `PID`、`MOTOR`                             |      |
| Day 7  | 4.1 FreeRTOS 启动流程 + 任务优先级                    | `FreeRTOSConfig.h`、`app_tasks.h`          |      |
| Day 8  | 4.2 互斥量、二值信号量、中断与任务同步                | `g_dataMutex`、`g_speedCalcSemaphore`      |      |
| Day 9  | 5.1 系统状态结构体 + 手动模式 + 电机控制任务          | `SystemState_t`、`MotorControlTask`        |      |
| Day 10 | 5.2 自动模式状态机 + 风速融合算法                     | `WIND`、`WindSpeedTask`                    |      |
| Day 11 | 5.3 防回流模式 + UI 显示 + 系统联调                   | `AntiBackflowTask`、`UIDisplayTask`、`LCD` |      |
| Day 12 | 6.1 Bootloader、APP、Flash 分区、DMA 接收             | `IAP`、`DMA`、`STMFLASH`                   |      |
| Day 13 | 6.2 CRC32 校验、Flash 擦写、APP 跳转                  | `CRC32`、`IAP`、`STMFLASH`                 |      |
| Day 14 | 7.1 外设、HardFault、RTOS、IAP 排查                   | 全项目                                     |      |
| Day 15 | 8.1 核心模块复现 + 项目总复盘                         | 全项目                                     |      |

阶段对应天数：

第 2 阶段：裸机核心驱动掌握，对应 **Day 1 - Day 5**。  
第 3 阶段：PID 闭环调速，对应 **Day 6**。  
第 4 阶段：FreeRTOS 移植和任务框架，对应 **Day 7 - Day 8**。  
第 5 阶段：应用层业务逻辑，对应 **Day 9 - Day 11**。  
第 6 阶段：Bootloader / IAP 固件升级，对应 **Day 12 - Day 13**。  
第 7 阶段：调试能力专项训练，对应 **Day 14**。  
第 8 阶段：综合复现与最终验收，对应 **Day 15**。

---

# 总路线

```text
第 0 阶段：先把项目跑起来
第 1 阶段：建立项目全局地图
第 2 阶段：裸机驱动逐个手敲
第 3 阶段：PID 闭环调速
第 4 阶段：FreeRTOS 移植和任务框架
第 5 阶段：应用层业务逻辑
第 6 阶段：Bootloader / IAP 固件升级
第 7 阶段：调试能力专项训练
最终阶段：自己从 0 手敲一版
```

---

# 第 0 阶段：先把项目跑起来

**时间：1～2 天**

目标不是看懂所有代码，而是先确认项目能编译、能下载、能看到现象。

## 步骤

1. 安装 Keil、ST-Link 驱动、芯片支持包、串口助手。
2. 打开原工程，先编译通过。
3. 下载到 STM32F103C8T6 板子。
4. 观察 LCD、按键、蜂鸣器、电机、串口、DHT11、MQ2 是否正常。
5. 记录哪些模块正常，哪些硬件暂时没有接，哪些功能需要后面调试。

## 掌握标准

你知道项目能不能跑，知道当前硬件连接有没有大问题。

---

# 第 1 阶段：建立项目全局地图

**时间：2 天**

目标是看到任何一个文件，都知道它属于哪一层、干什么。

## 步骤

1. 看工程目录结构：

```text
CORE
STM32F10x_FWLib
SYSTEM
BSP
FreeRTOS
APP_TASK
USER
tools
```

2. 看 `main.c` 启动流程：

```text
main()
 ├── Hardware_Init()
 ├── System_Init()
 ├── StartTask_Create()
 └── vTaskStartScheduler()
```

3. 理解三层结构：

```text
BSP 驱动层：直接控制硬件
APP_TASK 应用层：业务逻辑和任务
FreeRTOS 中间层：任务调度、同步、通信
```

4. 画出系统数据流：

```text
传感器 → SensorTask → g_systemState
                     ↓
                 WindSpeedTask
                     ↓
                 MotorControlTask
                     ↓
                  电机 PWM
                     ↑
                 编码器测速
```

## 掌握标准

你能说清楚：

```text
main.c 负责初始化和启动调度器。
app_tasks.c 才是业务逻辑核心。
BSP 是底层驱动，APP_TASK 是上层任务。
g_systemState 是多个任务共享的系统状态中心。
```

---

# 第 2 阶段：裸机驱动逐个手敲

**时间：10～12 天**

这是基础阶段。不要一开始就碰 RTOS，先把每个硬件驱动拆开，在裸机工程里单独调通。

---

## 第 2.1 步：GPIO、蜂鸣器、按键

**时间：1～2 天**

学习内容：

```text
GPIO 时钟使能
推挽输出
上拉输入
低电平触发按键
蜂鸣器控制
按键短按、长按、消抖
```

练习：

```text
按键1短按：蜂鸣器响一下
按键2长按：蜂鸣器一直响，松开停止
```

掌握标准：

```text
你能自己写 Beep_Init、Beep_On、Beep_Off。
你能写按键状态机，而不是简单 delay 消抖。
```

---

## 第 2.2 步：DHT11 温湿度驱动

**时间：2 天**

学习内容：

```text
DHT11 单总线协议
起始信号
响应信号
40bit 数据格式
校验和
超时保护
```

练习：

```text
串口打印：
Temp = xx
Humi = xx
Check = OK / FAIL
```

掌握标准：

```text
你知道为什么 DHT11 的 DATA 引脚一会儿输出、一会儿输入。
你知道 0 和 1 是靠高电平持续时间区分的。
你能解释 40bit 数据格式。
```

---

## 第 2.3 步：MQ2 + ADC

**时间：1 天**

学习内容：

```text
ADC 模拟输入
ADC 时钟分频
单通道单次转换
软件触发
ADC 校准
多次采样取平均
```

练习：

```text
串口打印：
ADC = xxxx
Voltage = x.xx V
Gas = xxx.xx
```

掌握标准：

```text
你知道 MQ2 项目里用 AO 模拟输出，而不是 DO 数字输出。
你能自己写 ADC 初始化和 ADC 读取函数。
```

---

## 第 2.4 步：LCD + SPI

**时间：2 天**

学习内容：

```text
SPI 通信
SCK / MOSI / MISO / CS
LCD 的 RST、DC、CS、BLK
命令和数据区分
字符串显示
```

练习：

```text
LCD 显示：
Hello RTOS
Temp:
RPM:
Mode:
```

掌握标准：

```text
你知道 SPI 的 CPOL、CPHA 是什么。
你知道 LCD 为什么要区分命令和数据。
你能用 LCD 显示基本字符串。
```

---

## 第 2.5 步：TIM1 PWM + 有刷直流电机

**时间：2～3 天**

学习内容：

```text
TIM1 高级定时器
PWM 频率计算
ARR / PSC / CCR
互补输出 CH1 / CH1N
H 桥驱动
死区时间
电机正反转
```

重点公式：

```text
72MHz / 72 = 1MHz
ARR = 1000 - 1
PWM频率 = 1MHz / 1000 = 1kHz
CCR = 0~1000 对应 0%~100%
```

练习：

```text
串口输入 0~1000
控制电机转速
```

进阶练习：

```text
输入正数：正转
输入负数：反转
输入 0：停止
```

掌握标准：

```text
你能解释为什么用 TIM1。
你能解释为什么电机需要 H 桥。
你能解释为什么需要死区。
你能自己写 motor_start、motor_stop、motor_pwm_set。
```

---

## 第 2.6 步：TIM2 编码器测速

**时间：2 天**

学习内容：

```text
A/B 相编码器
正交脉冲
TIM2 编码器模式
4 倍频
溢出处理
TIM4 定时采样
RPM 计算
```

转速公式：

```text
RPM = 编码器增量 × (1000 / 采样周期ms) × 60 / 减速比 / 编码器线数 / 4倍频
```

项目参数：

```text
采样周期：50ms
减速比：30
编码器线数：11
倍频：4
```

练习：

```text
PWM = 300 / 500 / 800
串口打印 RPM
观察转速变化
```

掌握标准：

```text
你知道 A/B 相如何判断方向。
你知道为什么要除以 4 倍频。
你知道为什么用固定时间窗口计算 RPM。
```

---

# 第 3 阶段：PID 闭环调速

**时间：4～5 天**

目标是把电机开环控制升级成闭环控制。

## 步骤

1. 理解开环控制问题：

```text
固定 PWM 不代表固定转速。
负载变化后，实际转速会偏离目标。
```

2. 学习位置式 PID：

```text
output = Kp * error + Ki * integral + Kd * (error - last_error)
```

3. 手敲 PID 函数：

```text
PID_Init()
PID_SetTarget()
PID_Calculate()
PID_Reset()
```

4. 进行三组实验：

```text
开环 PWM 控制
闭环目标 RPM 控制
人为增加负载扰动
```

5. 学习 PID 调参：

```text
先调 Kp
再调 Ki
最后看是否需要 Kd
```

## 掌握标准

你能讲清楚：

```text
Kp 提高响应速度。
Ki 消除稳态误差。
Kd 抑制超调，但容易放大噪声。
本项目 Kd 为 0，是因为编码器测速有噪声，微分项容易导致抖动。
PID 输出限幅 0~1000，因为 PWM CCR 最大就是 1000。
```

---

# 第 4 阶段：FreeRTOS 移植和任务框架

**时间：6～7 天**

目标是理解项目为什么要用 RTOS，以及任务是怎么协作的。

---

## 第 4.1 步：FreeRTOS 最小工程

**时间：1 天**

练习：

```text
Task1：500ms 翻转 LED
Task2：1000ms 串口打印
```

掌握：

```text
vTaskStartScheduler() 之后，main 的 while(1) 基本不会再执行。
任务通过 vTaskDelay 主动让出 CPU。
```

---

## 第 4.2 步：FreeRTOS 启动流程

**时间：1 天**

理解流程：

```text
main()
 → 创建任务
 → vTaskStartScheduler()
 → 创建空闲任务
 → 配置 SysTick
 → SVC 启动第一个任务
 → SysTick 提供节拍
 → PendSV 做任务切换
```

掌握标准：

```text
你能解释 SVC、SysTick、PendSV 各自作用。
你知道为什么 stm32f10x_it.c 里要注释掉同名中断函数。
```

---

## 第 4.3 步：任务优先级设计

**时间：1 天**

项目任务优先级：

```text
IAP：7
SpeedCalcTask：6
MotorControlTask：5
KeyScanTask：4
SensorTask：3
WindSpeedTask：3
AntiBackflowTask：2
UIDisplayTask：1
```

掌握标准：

```text
不是功能越重要优先级越高，而是越需要实时响应，优先级越高。
测速和电机控制实时性高，所以优先级高。
LCD 显示不紧急，所以优先级最低。
```

---

## 第 4.4 步：互斥量和二值信号量

**时间：1 天**

重点对象：

```text
g_dataMutex
g_speedCalcSemaphore
g_iapSemaphore
```

理解：

```text
g_dataMutex：保护 g_systemState。
g_speedCalcSemaphore：TIM4 中断通知 SpeedCalcTask。
g_iapSemaphore：DMA 完成中断通知 IAP 任务。
```

掌握标准：

```text
你知道为什么不用队列，而用互斥量 + 全局结构体。
你知道 ISR 中要用 FromISR API。
你知道中断里不做复杂业务，而是通知任务处理。
```

---

## 第 4.5 步：重写简化任务框架

**时间：2～3 天**

练习写一个简化版任务系统：

```text
StartTask
 ├── KeyScanTask
 ├── SensorTask
 ├── MotorControlTask
 ├── UIDisplayTask
 └── SpeedCalcTask
```

先不加 IAP、防回流、完整自动模式。

掌握标准：

```text
按键能切模式。
LCD 能显示模式。
传感器能更新数据。
电机能按 LOW/HIGH 闭环调速。
```

---

# 第 5 阶段：应用层业务逻辑

**时间：7～8 天**

目标是彻底理解 `app_tasks.c`。

---

## 第 5.1 步：系统状态结构体

**时间：1 天**

重点理解：

```text
currentMode
speedLevel
motorRunning
temperature
humidity
gasConcentration
windSpeedPWM
actualRPM
targetRPM
cookingEventActive
antiBackflowActive
gasThreshold
autoModeCounter
cookingEventCounter
```

掌握标准：

```text
你知道各任务不是彼此乱调用，而是通过 g_systemState 共享系统状态。
SensorTask 写传感器数据。
WindSpeedTask 写风速和 cooking event。
MotorControlTask 读状态并控制电机。
UIDisplayTask 读状态并显示。
```

---

## 第 5.2 步：手动模式

**时间：1 天**

逻辑：

```text
进入 MODE_MANUAL
如果电机没启动，先 motor_start()
根据 LOW/HIGH 设置目标 RPM
PID_SetTarget()
PID_Calculate()
motor_pwm_set()
```

掌握标准：

```text
你能解释手动模式为什么使用 PID 闭环。
你能解释 LOW/HIGH 档位如何转成目标转速。
```

---

## 第 5.3 步：自动模式状态机

**时间：2 天**

自动模式三阶段：

```text
AUTO_STATE_STARTUP
AUTO_STATE_COOKING
AUTO_STATE_DELAY_OFF
```

逻辑：

```text
启动阶段：低速运行，等待 cooking event
烹饪阶段：根据风速算法控制电机
延时关闭阶段：cooking event 消失后继续运行一段时间
```

掌握标准：

```text
你知道为什么自动模式不用一堆 if-else，而用状态机。
你能画出自动模式状态转移图。
```

---

## 第 5.4 步：风速融合算法

**时间：1 天**

算法：

```text
f_T = 温度归一化
f_H = 湿度归一化
f_G = 气体浓度归一化

F = 0.2f_T + 0.2f_H + 0.6f_G

PWM = PWM_MIN + (PWM_MAX - PWM_MIN) × F
```

掌握标准：

```text
你知道为什么要归一化。
你知道为什么气体浓度权重最高。
你知道为什么 PWM_MIN 不设为 0。
```

---

## 第 5.5 步：防回流模式

**时间：1 天**

逻辑：

```text
气体浓度超过低阈值 → 启动风机
启动后切换高阈值 → 防止重复触发
气体浓度低于正常阈值 → 关闭风机
```

掌握标准：

```text
你知道这是迟滞控制/动态阈值控制。
你能解释为什么不能只用一个阈值。
```

---

## 第 5.6 步：UI 显示和系统联调

**时间：1～2 天**

LCD 显示：

```text
Mode
Level
Temp
Humi
Gas
Target RPM
Actual RPM
PWM
Auto State
```

掌握标准：

```text
整个项目能完成：
手动调速
自动调速
防回流
传感器采集
LCD 显示
编码器测速
PID 闭环
```

---

# 第 6 阶段：Bootloader / IAP 固件升级

**时间：6～7 天**

这是项目高级亮点。

---

## 第 6.1 步：理解 Bootloader 和 APP

**时间：1 天**

掌握：

```text
Bootloader：负责接收、校验、写入、跳转新固件。
APP：需要升级的新固件，不是手机 APP。
```

Flash 分区：

```text
Bootloader 区：0x08000000 开始
APP 区：0x0800F000 开始
```

掌握标准：

```text
你知道为什么 APP 工程要修改 IROM 起始地址。
你知道为什么 APP 不能放在 0x08000000。
```

---

## 第 6.2 步：USART + DMA 接收固件

**时间：1～2 天**

流程：

```text
PC 发送 APP_crc.bin
USART1 接收
DMA 自动搬运到 receive_buff
DMA 完成中断释放 g_iapSemaphore
IAP 任务被唤醒
```

掌握标准：

```text
你知道为什么不用串口中断逐字节接收。
你知道 DMA_BufferSize 为什么必须匹配固件大小。
你知道 DMA 为什么用普通模式，不用循环模式。
```

---

## 第 6.3 步：CRC32 校验

**时间：1 天**

流程：

```text
PC 端 Python 给 APP.bin 追加 CRC32
STM32 接收后重新计算 CRC32
提取末尾 4 字节 CRC
两者一致才允许升级
```

掌握标准：

```text
你知道 CRC32 是完整性校验，不是加密。
你知道为什么要注意小端序。
```

---

## 第 6.4 步：Flash 擦写

**时间：1 天**

掌握：

```text
Flash 写之前要解锁。
写之前要擦除页。
STM32F1 Flash 通常按半字写入。
写完要上锁。
```

掌握标准：

```text
你能解释 iap_write_appbin() 做了什么。
你知道为什么写 Flash 前必须擦除。
```

---

## 第 6.5 步：跳转 APP

**时间：1 天**

核心：

```text
MSP = *(APP_ADDR)
PC  = *(APP_ADDR + 4)
```

跳转前要：

```text
关闭中断
关闭相关外设
设置 MSP
跳转 Reset_Handler
```

掌握标准：

```text
你知道 APP 首地址存的是初始栈指针。
你知道 APP_ADDR + 4 存的是复位中断入口。
你知道为什么跳转前要设置 MSP。
```

---

## 第 6.6 步：完整升级实验

**时间：1 天**

实验：

```text
APP_A：显示 Version A
APP_B：显示 Version B
通过串口发送 APP_B
升级成功后运行 APP_B
```

掌握标准：

```text
不用 ST-Link 下载 APP。
通过串口升级 APP。
CRC 失败不能升级，CRC 成功才能写 Flash 和跳转。
```

---

# 第 7 阶段：调试能力专项训练

**时间：4～5 天**

目标是你能独立排查问题。

---

## 第 7.1 步：外设不工作排查

固定顺序：

```text
查电源
查时钟
查 GPIO 模式
查复用功能
查外设初始化参数
查中断使能
查 NVIC 优先级
查标志位清除
查实际波形
```

---

## 第 7.2 步：HardFault 排查

练习制造问题：

```text
非法地址访问
栈溢出
函数指针跳错
APP 地址错误
中断优先级错误
```

掌握：

```text
看 PC
看 LR
看 SP
看调用栈
定位出错代码行
```

---

## 第 7.3 步：RTOS 调试

重点看：

```text
任务状态
任务优先级
任务栈剩余
信号量是否阻塞
是否有高优先级任务一直占用 CPU
```

---

## 第 7.4 步：波形调试

建议看：

```text
DHT11 单总线波形
PWM 波形
编码器 A/B 相波形
SPI SCK/MOSI 波形
USART TX/RX 波形
```

---

# 最终阶段：自己从 0 手敲一版

**时间：10 天**

这个阶段不是一开始就做，而是前面都学完后做最终验收。

## 重写顺序

```text
1. GPIO + 串口 printf
2. 蜂鸣器 + 按键
3. LCD
4. DHT11
5. MQ2
6. PWM 电机开环
7. 编码器测速
8. PID 闭环
9. FreeRTOS 任务
10. 自动模式 + 防回流
11. IAP + CRC32
```

## 最终掌握标准

```text
能自己写 GPIO 驱动
能自己写按键状态机
能自己写 DHT11 关键时序
能自己写 ADC 采样
能自己写 PWM 电机控制
能自己写编码器测速
能自己写 PID
能解释 FreeRTOS 任务设计
能解释 IAP 升级流程
能排查 HardFault、RPM 异常、CRC 失败、电机不转
```

