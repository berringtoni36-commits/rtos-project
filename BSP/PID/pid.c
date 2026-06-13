#include "pid.h"
/***********************
项目名：PID文件
作者：不甘心的咸鱼--闲鱼/不搭(414192836)--小红书
闲鱼号：tb43915564
修改日期：2026/2/1
项目已申请版权，请勿倒卖！
************************/  

/**
 * @brief 初始化PID控制器
 * @param pid: PID结构体指针
 * @param Kp: 比例系数
 * @param Ki: 积分系数
 * @param Kd: 微分系数
 * @param out_max: 输出上限
 * @param out_min: 输出下限
 */
void PID_Init(PID_TypeDef *pid, float Kp, float Ki, float Kd, float out_max, float out_min)
{
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
    
    pid->target = 0.0f;
    pid->actual = 0.0f;
    
    pid->error = 0.0f;
    pid->last_error = 0.0f;
    pid->integral = 0.0f;
    
    pid->output = 0.0f;
    pid->output_max = out_max;
    pid->output_min = out_min;
    
    /* 积分限幅设为输出限幅的一半，防止积分饱和 */
    pid->integral_max = out_max / 2.0f;
}

/**
 * @brief 设置PID目标值
 * @param pid: PID结构体指针
 * @param target: 目标值
 */
void PID_SetTarget(PID_TypeDef *pid, float target)
{
    pid->target = target;
}

/**
 * @brief PID计算（位置式）
 * @note 位置式PID公式：output = Kp*e(k) + Ki*Σe(k) + Kd*[e(k)-e(k-1)]
 * @param pid: PID结构体指针
 * @param actual: 当前实际值
 * @return PID输出值
 */
float PID_Calculate(PID_TypeDef *pid, float actual)
{
    float p_out, i_out, d_out;
    
    /* 更新实际值 */
    pid->actual = actual;
    
    /* 计算当前误差 */
    pid->error = pid->target - pid->actual;
    
    /* 积分累加 */
    pid->integral += pid->error;
    
    /* 积分限幅，防止积分饱和 */
    if (pid->integral > pid->integral_max)
    {
        pid->integral = pid->integral_max;
    }
    else if (pid->integral < -pid->integral_max)
    {
        pid->integral = -pid->integral_max;
    }
    
    /* 计算PID三个分量 */
    p_out = pid->Kp * pid->error;                           /* 比例项 */
    i_out = pid->Ki * pid->integral;                        /* 积分项 */
    d_out = pid->Kd * (pid->error - pid->last_error);       /* 微分项 */
    
    /* 计算PID输出 */
    pid->output = p_out + i_out + d_out;
    
    /* 输出限幅 */
    if (pid->output > pid->output_max)
    {
        pid->output = pid->output_max;
    }
    else if (pid->output < pid->output_min)
    {
        pid->output = pid->output_min;
    }
    
    /* 保存当前误差供下次使用 */
    pid->last_error = pid->error;
    
    return pid->output;
}

/**
 * @brief 复位PID控制器
 * @param pid: PID结构体指针
 */
void PID_Reset(PID_TypeDef *pid)
{
    pid->error = 0.0f;
    pid->last_error = 0.0f;
    pid->integral = 0.0f;
    pid->output = 0.0f;
}
