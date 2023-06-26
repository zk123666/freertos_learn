/**
 ****************************************************************************************************
 * @file        freertos.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.4
 * @date        2022-01-04
 * @brief       FreeRTOS ���񴴽���ɾ��ʵ��(��̬����)
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

/******************************************************************************************************/
/*FreeRTOS����*/

static StackType_t  IdleTaskStack[configMINIMAL_STACK_SIZE];        /* �������������ջ */
static StaticTask_t IdleTaskTCB;                                    /* ����������ƿ� */
static StackType_t  TimerTaskStack[configTIMER_TASK_STACK_DEPTH];   /* ��ʱ�����������ջ */
static StaticTask_t TimerTaskTCB;                                   /* ��ʱ������������ƿ� */

/**
 * @brief       ��ȡ��������������ջ��������ƿ��ڴ棬��Ϊ������ʹ�õ�
                ��̬�ڴ棬��˿�������������ջ��������ƿ���ڴ��Ӧ��
                ���û����ṩ��FreeRTOS�ṩ�˽ӿں���vApplicationGetIdleTaskMemory()
                ʵ�ִ˺������ɡ�
 * @param       ppxIdleTaskTCBBuffer:������ƿ��ڴ�
                ppxIdleTaskStackBuffer:�����ջ�ڴ�
                pulIdleTaskStackSize:�����ջ��С
 * @retval      ��
 */
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, 
                                   StackType_t  **ppxIdleTaskStackBuffer, 
                                   uint32_t     *pulIdleTaskStackSize)
{
    *ppxIdleTaskTCBBuffer   = &IdleTaskTCB;
    *ppxIdleTaskStackBuffer = IdleTaskStack;
    *pulIdleTaskStackSize   = configMINIMAL_STACK_SIZE;
}

/**
 * @brief       ��ȡ��ʱ����������������ջ��������ƿ��ڴ�
 * @param       ppxTimerTaskTCBBuffer:������ƿ��ڴ�
                ppxTimerTaskStackBuffer:�����ջ�ڴ�
                pulTimerTaskStackSize:�����ջ��С
 * @retval      ��
 */
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
                                    StackType_t  **ppxTimerTaskStackBuffer,
                                    uint32_t     *pulTimerTaskStackSize)
{
    *ppxTimerTaskTCBBuffer  = &TimerTaskTCB;
    *ppxTimerTaskStackBuffer= TimerTaskStack;
    *pulTimerTaskStackSize  = configTIMER_TASK_STACK_DEPTH;
}

/* START_TASK ���� ����
 * ����: ������ �������ȼ� ��ջ��С ��������
 */
#define START_TASK_PRIO 1                   /* �������ȼ� */
#define START_STK_SIZE  128                 /* �����ջ��С */
StackType_t StartTaskStack[START_STK_SIZE]; /* �����ջ */
StaticTask_t            StartTaskTCB;       /* ������ƿ� */
TaskHandle_t            StartTask_Handler;  /* ������ */
void start_task(void *pvParameters);        /* ������ */

/* TASK1 ���� ����
 * ����: ������ �������ȼ� ��ջ��С ��������
 */
#define TASK1_PRIO      2                   /* �������ȼ� */
#define TASK1_STK_SIZE  128                 /* �����ջ��С */
StackType_t Task1TaskStack[TASK1_STK_SIZE]; /* �����ջ */
StaticTask_t            Task1TaskTCB;       /* ������ƿ� */
TaskHandle_t            Task1Task_Handler;  /* ������ */
void task1(void *pvParameters);             /* ������ */

/* TASK2 ���� ����
 * ����: ������ �������ȼ� ��ջ��С ��������
 */
#define TASK2_PRIO      3                   /* �������ȼ� */
#define TASK2_STK_SIZE  128                 /* �����ջ��С */
StackType_t Task2TaskStack[TASK2_STK_SIZE]; /* �����ջ */
StaticTask_t            Task2TaskTCB;       /* ������ƿ� */
TaskHandle_t            Task2Task_Handler;  /* ������ */
void task2(void *pvParameters);             /* ������ */

/* TASK3 ���� ����
 * ����: ������ �������ȼ� ��ջ��С ��������
 */
#define TASK3_PRIO      4                   /* �������ȼ� */
#define TASK3_STK_SIZE  128                 /* �����ջ��С */
StackType_t Task3TaskStack[TASK3_STK_SIZE]; /* �����ջ */
StaticTask_t            Task3TaskTCB;       /* ������ƿ� */
TaskHandle_t            Task3Task_Handler;  /* ������ */
void task3(void *pvParameters);             /* ������ */

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
    lcd_show_string(10, 47, 220, 24, 24, "Task Create & Del", RED);
    lcd_show_string(10, 76, 220, 16, 16, "ATOM@ALIENTEK", RED);
    
    lcd_draw_rectangle(5, 110, 115, 314, BLACK);
    lcd_draw_rectangle(125, 110, 234, 314, BLACK);
    lcd_draw_line(5, 130, 115, 130, BLACK);
    lcd_draw_line(125, 130, 234, 130, BLACK);
    lcd_show_string(15, 111, 110, 16, 16, "Task1: 000", BLUE);
    lcd_show_string(135, 111, 110, 16, 16, "Task2: 000", BLUE);
    
    StartTask_Handler = xTaskCreateStatic((TaskFunction_t   )start_task,        /* ������ */
                                          (const char*      )"start_task",      /* �������� */
                                          (uint32_t         )START_STK_SIZE,    /* �����ջ��С */
                                          (void*            )NULL,              /* ���ݸ��������Ĳ��� */
                                          (UBaseType_t      )START_TASK_PRIO,   /* �������ȼ� */
                                          (StackType_t*     )StartTaskStack,    /* �����ջ */
                                          (StaticTask_t*    )&StartTaskTCB);    /* ������ƿ� */
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
    /* ��������1 */
    Task1Task_Handler = xTaskCreateStatic((TaskFunction_t   )task1,         /* ������ */
                                          (const char*      )"task1",       /* �������� */
                                          (uint32_t         )TASK1_STK_SIZE,/* �����ջ��С */
                                          (void*            )NULL,          /* ���ݸ��������Ĳ��� */
                                          (UBaseType_t      )TASK1_PRIO,    /* �������ȼ� */
                                          (StackType_t*     )Task1TaskStack,/* �����ջ */
                                          (StaticTask_t*    )&Task1TaskTCB);/* ������ƿ� */
    /* ��������2 */
    Task2Task_Handler = xTaskCreateStatic((TaskFunction_t   )task2,         /* ������ */
                                          (const char*      )"task2",       /* �������� */
                                          (uint32_t         )TASK2_STK_SIZE,/* �����ջ��С */
                                          (void*            )NULL,          /* ���ݸ��������Ĳ��� */
                                          (UBaseType_t      )TASK2_PRIO,    /* �������ȼ� */
                                          (StackType_t*     )Task2TaskStack,/* �����ջ */
                                          (StaticTask_t*    )&Task2TaskTCB);/* ������ƿ� */
    /* ��������3 */
    Task3Task_Handler = xTaskCreateStatic((TaskFunction_t   )task3,         /* ������ */
                                          (const char*      )"task3",       /* �������� */
                                          (uint32_t         )TASK3_STK_SIZE,/* �����ջ��С */
                                          (void*            )NULL,          /* ���ݸ��������Ĳ��� */
                                          (UBaseType_t      )TASK3_PRIO,    /* �������ȼ� */
                                          (StackType_t*     )Task3TaskStack,/* �����ջ */
                                          (StaticTask_t*    )&Task3TaskTCB);/* ������ƿ� */
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
    uint32_t task1_num = 0;
    
    while (1)
    {
        lcd_fill(6, 131, 114, 313, lcd_discolor[++task1_num % 11]);
        lcd_show_xnum(71, 111, task1_num, 3, 16, 0x80, BLUE);
        
        vTaskDelay(500);
    }
}

/**
 * @brief       task2
 * @param       pvParameters : �������(δ�õ�)
 * @retval      ��
 */
void task2(void *pvParameters)
{
    uint32_t task2_num = 0;
    
    while (1)
    {
        lcd_fill(126, 131, 233, 313, lcd_discolor[11 - (++task2_num % 11)]);
        lcd_show_xnum(191, 111, task2_num, 3, 16, 0x80, BLUE);
        
        vTaskDelay(500);
    }
}

/**
 * @brief       task3
 * @param       pvParameters : �������(δ�õ�)
 * @retval      ��
 */
void task3(void *pvParameters)
{
    uint8_t key = 0;
    
    while (1)
    {
        key = key_scan(0);
        
        switch (key)
        {
            case KEY0_PRES:                     /* ɾ������1 */
            {
                vTaskDelete(Task1Task_Handler);
                break;
            }
            case KEY1_PRES:                     /* ɾ������2 */
            {
                vTaskDelete(Task2Task_Handler);
                break;
            }
            default:
            {
                break;
            }
        }
        
        vTaskDelay(10);
    }
}
