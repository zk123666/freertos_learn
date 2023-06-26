/**
 ****************************************************************************************************
 * @file        btim.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-20
 * @brief       ������ʱ�� ��������
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� STM32F103������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 * �޸�˵��
 * V1.0 20200420
 * ��һ�η���
 *
 ****************************************************************************************************
 */

#include "./BSP/TIMER/btim.h"
#include "FreeRTOS.h"

TIM_HandleTypeDef g_tim3_handle;        /* ��ʱ��3��� */

uint32_t FreeRTOSRunTimeTicks;          /* FreeRTOSʱ��ͳ�����õĽ��ļ����� */

void ConfigureTimeForRunTimeStats(void)
{
    FreeRTOSRunTimeTicks = 0;           /* ���ļ�������ʼ��Ϊ0 */
    btim_tim3_int_init(10-1, 720-1);    /* ��ʼ��TIM3 */
}

/**
 * @brief       ������ʱ��TIM3��ʱ�жϳ�ʼ������
 * @note
 *              ������ʱ����ʱ������APB1,��PPRE1 �� 2��Ƶ��ʱ��
 *              ������ʱ����ʱ��ΪAPB1ʱ�ӵ�2��, ��APB1Ϊ36M, ���Զ�ʱ��ʱ�� = 72Mhz
 *              ��ʱ�����ʱ����㷽��: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=��ʱ������Ƶ��,��λ:Mhz
 *
 * @param       arr: �Զ���װֵ��
 * @param       psc: ʱ��Ԥ��Ƶ��
 * @retval      ��
 */
void btim_tim3_int_init(uint16_t arr, uint16_t psc)
{
    BTIM_TIM3_INT_CLK_ENABLE();                                      /* ʹ��TIM3ʱ�� */
    
    g_tim3_handle.Instance = BTIM_TIM3_INT;                          /* ͨ�ö�ʱ��3 */
    g_tim3_handle.Init.Prescaler = psc;                              /* ��Ƶ */
    g_tim3_handle.Init.CounterMode = TIM_COUNTERMODE_UP;             /* ���ϼ����� */
    g_tim3_handle.Init.Period = arr;                                 /* �Զ�װ��ֵ */
    g_tim3_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;       /* ʱ�ӷ�Ƶ���� */
    HAL_TIM_Base_Init(&g_tim3_handle);
    
    HAL_NVIC_SetPriority(BTIM_TIM3_INT_IRQn, 4, 0);  /* �����ж����ȼ�����ռ���ȼ�4�������ȼ�0 */
    HAL_NVIC_EnableIRQ(BTIM_TIM3_INT_IRQn);          /* ����ITM3�ж� */
    
    HAL_TIM_Base_Start_IT(&g_tim3_handle);           /* ʹ�ܶ�ʱ��3�Ͷ�ʱ��3�����ж� */
}

/**
 * @brief       ��ʱ���жϷ�����
 * @param       ��
 * @retval      ��
 */
void BTIM_TIM3_INT_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_tim3_handle);
}

/**
 * @brief       ��ʱ�������жϻص�����
* @param        htim:��ʱ�����ָ��
 * @retval      ��
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim == (&g_tim3_handle))
    {
        FreeRTOSRunTimeTicks++;
    }
}










