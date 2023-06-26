/**
 ****************************************************************************************************
 * @file        btim.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-20
 * @brief       基本定时器 驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 STM32F103开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.0 20200420
 * 第一次发布
 *
 ****************************************************************************************************
 */

#include "./BSP/TIMER/btim.h"
#include "FreeRTOS.h"

TIM_HandleTypeDef g_tim3_handle;        /* 定时器3句柄 */

uint32_t FreeRTOSRunTimeTicks;          /* FreeRTOS时间统计所用的节拍计数器 */

void ConfigureTimeForRunTimeStats(void)
{
    FreeRTOSRunTimeTicks = 0;           /* 节拍计数器初始化为0 */
    btim_tim3_int_init(10-1, 720-1);    /* 初始化TIM3 */
}

/**
 * @brief       基本定时器TIM3定时中断初始化函数
 * @note
 *              基本定时器的时钟来自APB1,当PPRE1 ≥ 2分频的时候
 *              基本定时器的时钟为APB1时钟的2倍, 而APB1为36M, 所以定时器时钟 = 72Mhz
 *              定时器溢出时间计算方法: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=定时器工作频率,单位:Mhz
 *
 * @param       arr: 自动重装值。
 * @param       psc: 时钟预分频数
 * @retval      无
 */
void btim_tim3_int_init(uint16_t arr, uint16_t psc)
{
    BTIM_TIM3_INT_CLK_ENABLE();                                      /* 使能TIM3时钟 */
    
    g_tim3_handle.Instance = BTIM_TIM3_INT;                          /* 通用定时器3 */
    g_tim3_handle.Init.Prescaler = psc;                              /* 分频 */
    g_tim3_handle.Init.CounterMode = TIM_COUNTERMODE_UP;             /* 向上计数器 */
    g_tim3_handle.Init.Period = arr;                                 /* 自动装载值 */
    g_tim3_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;       /* 时钟分频因子 */
    HAL_TIM_Base_Init(&g_tim3_handle);
    
    HAL_NVIC_SetPriority(BTIM_TIM3_INT_IRQn, 4, 0);  /* 设置中断优先级，抢占优先级4，子优先级0 */
    HAL_NVIC_EnableIRQ(BTIM_TIM3_INT_IRQn);          /* 开启ITM3中断 */
    
    HAL_TIM_Base_Start_IT(&g_tim3_handle);           /* 使能定时器3和定时器3更新中断 */
}

/**
 * @brief       定时器中断服务函数
 * @param       无
 * @retval      无
 */
void BTIM_TIM3_INT_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_tim3_handle);
}

/**
 * @brief       定时器更新中断回调函数
* @param        htim:定时器句柄指针
 * @retval      无
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim == (&g_tim3_handle))
    {
        FreeRTOSRunTimeTicks++;
    }
}










