/**
 ****************************************************************************************************
 * @file        freertos.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.4
 * @date        2022-01-04
 * @brief       FreeRTOS �����ʱ��ʵ��
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� ��ӢF103������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#include "freertos_demo.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/KEY/key.h"
/*FreeRTOS*********************************************************************************************/
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

/******************************************************************************************************/
/*FreeRTOS����*/

/* START_TASK ���� ����
 * ����: ������ �������ȼ� ��ջ��С ��������
 */
#define START_TASK_PRIO 1                   /* �������ȼ� */
#define START_STK_SIZE  128                 /* �����ջ��С */
TaskHandle_t            StartTask_Handler;  /* ������ */
void start_task(void *pvParameters);        /* ������ */

/* TASK1 ���� ����
 * ����: ������ �������ȼ� ��ջ��С ��������
 */
#define TASK1_PRIO      2                   /* �������ȼ� */
#define TASK1_STK_SIZE  128                 /* �����ջ��С */
TaskHandle_t            Task1Task_Handler;  /* ������ */
void task1(void *pvParameters);             /* ������ */

TimerHandle_t           Timer1Timer_Handler;/* ��ʱ��1��� */
TimerHandle_t           Timer2Timer_Handler;/* ��ʱ��2��� */

void Timer1Callback(TimerHandle_t xTimer);  /* ��ʱ��1��ʱ�ص����� */
void Timer2Callback(TimerHandle_t xTimer);  /* ��ʱ��2��ʱ�ص����� */

/******************************************************************************************************/

/* LCDˢ��ʱʹ�õ���ɫ */
uint16_t lcd_discolor[11] = {WHITE, BLACK, BLUE, RED,
                             MAGENTA, GREEN, CYAN, YELLOW,
                             BROWN, BRRED, GRAY};

/**
 * @brief       FreeRTOS������ں���
 * @param       ��
 * @retval      ��
 */
void freertos_demo(void)
{
    lcd_show_string(10, 10, 220, 32, 32, "STM32", RED);
    lcd_show_string(10, 47, 220, 24, 24, "Timer", RED);
    lcd_show_string(10, 76, 220, 16, 16, "ATOM@ALIENTEK", RED);
    
    lcd_draw_rectangle(5, 110, 115, 314, BLACK);
    lcd_draw_rectangle(125, 110, 234, 314, BLACK);
    lcd_draw_line(5, 130, 115, 130, BLACK);
    lcd_draw_line(125, 130, 234, 130, BLACK);
    lcd_show_string(15, 111, 110, 16, 16, "Timer1: 000", BLUE);
    lcd_show_string(135, 111, 110, 16, 16, "Timer2: 000", BLUE);
    
    xTaskCreate((TaskFunction_t )start_task,            /* ������ */
                (const char*    )"start_task",          /* �������� */
                (uint16_t       )START_STK_SIZE,        /* �����ջ��С */
                (void*          )NULL,                  /* ������������Ĳ��� */
                (UBaseType_t    )START_TASK_PRIO,       /* �������ȼ� */
                (TaskHandle_t*  )&StartTask_Handler);   /* ������ */
    vTaskStartScheduler();
}

/**
 * @brief       start_task
 * @param       pvParameters : �������(δ�õ�)
 * @retval      ��
 */
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           /* �����ٽ��� */
    /* ��ʱ��1����Ϊ���ڶ�ʱ�� */
    Timer1Timer_Handler = xTimerCreate((const char*  )"Timer1",                 /* ��ʱ���� */
                                      (TickType_t   )1000,                      /* ��ʱ����ʱʱ�� */
                                      (UBaseType_t  )pdTRUE,                    /* ���ڶ�ʱ�� */
                                      (void*        )1,                         /* ��ʱ��ID */
                                      (TimerCallbackFunction_t)Timer1Callback); /* ��ʱ���ص����� */
    /* ��ʱ��2����Ϊ���ζ�ʱ�� */
    Timer2Timer_Handler = xTimerCreate((const char*  )"Timer2",                 /* ��ʱ���� */
                                     (TickType_t    )1000,                      /* ��ʱ����ʱʱ�� */
                                     (UBaseType_t   )pdFALSE,                   /* ���ζ�ʱ�� */
                                     (void*         )2,                         /* ��ʱ��ID */
                                     (TimerCallbackFunction_t)Timer2Callback);  /* ��ʱ���ص����� */
    /* ��������1 */
    xTaskCreate((TaskFunction_t )task1,
                (const char*    )"task1",
                (uint16_t       )TASK1_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK1_PRIO,
                (TaskHandle_t*  )&Task1Task_Handler);
    vTaskDelete(StartTask_Handler); /* ɾ����ʼ���� */
    taskEXIT_CRITICAL();            /* �˳��ٽ��� */
}

/**
 * @brief       task1
 * @param       pvParameters : �������(δ�õ�)
 * @retval      ��
 */
void task1(void *pvParameters)
{
    uint8_t key = 0;
    
    while (1)
    {
        if ((Timer1Timer_Handler != NULL) && (Timer2Timer_Handler != NULL))
        {
            key = key_scan(0);
            
            switch (key)
            {
                case KEY0_PRES:
                {
                    xTimerStart((TimerHandle_t  )Timer1Timer_Handler,   /* �������Ķ�ʱ����� */
                                (TickType_t     )portMAX_DELAY);        /* �ȴ�ϵͳ������ʱ�������ʱ�� */
                    xTimerStart((TimerHandle_t  )Timer2Timer_Handler,   /* �������Ķ�ʱ����� */
                                (TickType_t     )portMAX_DELAY);        /* �ȴ�ϵͳ������ʱ�������ʱ�� */
                    break;
                }
                case KEY1_PRES:
                {
                    xTimerStop((TimerHandle_t  )Timer1Timer_Handler,    /* ��ֹͣ�Ķ�ʱ����� */
                                (TickType_t     )portMAX_DELAY);        /* �ȴ�ϵͳֹͣ��ʱ�������ʱ�� */
                    xTimerStop((TimerHandle_t  )Timer2Timer_Handler,    /* ��ֹͣ�Ķ�ʱ����� */
                                (TickType_t     )portMAX_DELAY);        /* �ȴ�ϵͳֹͣ��ʱ�������ʱ�� */
                    break;
                }
                default:
                {
                    break;
                }
            }
        }
        
        vTaskDelay(10);
    }
}

/**
 * @brief       Timer1��ʱ�ص�����
 * @param       xTimer : �������(δ�õ�)
 * @retval      ��
 */
void Timer1Callback(TimerHandle_t xTimer)
{
    static uint32_t timer1_num = 0;
    
    lcd_fill(6, 131, 114, 313, lcd_discolor[++timer1_num % 11]);    /* LCD����ˢ�� */
    lcd_show_xnum(79, 111, timer1_num, 3, 16, 0x80, BLUE);          /* ��ʾ��ʱ��1��ʱ���� */
}

/**
 * @brief       Timer2��ʱ�ص�����
 * @param       xTimer : �������(δ�õ�)
 * @retval      ��
 */
void Timer2Callback(TimerHandle_t xTimer)
{
    static uint32_t timer2_num = 0;
    
    lcd_fill(126, 131, 233, 313, lcd_discolor[++timer2_num % 11]);  /* LCD����ˢ�� */
    lcd_show_xnum(199, 111, timer2_num, 3, 16, 0x80, BLUE);         /* ��ʾ��ʱ��2��ʱ���� */
}
