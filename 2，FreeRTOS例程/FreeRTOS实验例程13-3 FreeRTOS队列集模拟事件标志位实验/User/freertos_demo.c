/**
 ****************************************************************************************************
 * @file        freertos.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.4
 * @date        2022-01-04
 * @brief       FreeRTOS ���м�ģ���¼���־λʵ��
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
#include "queue.h"

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

/* TASK2 ���� ����
 * ����: ������ �������ȼ� ��ջ��С ��������
 */
#define TASK2_PRIO      3                   /* �������ȼ� */
#define TASK2_STK_SIZE  128                 /* �����ջ��С */
TaskHandle_t            Task2Task_Handler;  /* ������ */
void task2(void *pvParameters);             /* ������ */

static QueueSetHandle_t xQueueSet;          /* ����һ�����м� */
QueueHandle_t           xQueue1;            /* �������1 */
QueueHandle_t           xQueue2;            /* �������2 */

#define QUEUE_LENGTH    1                   /* ����֧�ֵ���Ϣ���� */
#define QUEUE_ITEM_SIZE sizeof(uint32_t)    /* ������ÿ����Ϣ�Ĵ�С */
#define QUEUESET_LENGTH (2 * QUEUE_LENGTH)  /* ���м�֧�ֵ���Ϣ���� */

#define EVENTBIT_0      (1 << 0)            /* �¼�λ */
#define EVENTBIT_1      (1 << 1)
#define EVENTBIT_ALL    (EVENTBIT_0 | EVENTBIT_1)

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
    lcd_show_string(10, 47, 220, 24, 24, "Queue Event Group", RED);
    lcd_show_string(10, 76, 220, 16, 16, "ATOM@ALIENTEK", RED);
    
    lcd_draw_rectangle(5, 130, 234, 314, BLACK);        /* ������ */
    lcd_show_string(30, 110, 220, 16, 16, "Event Group Value: 0", BLUE);
    
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
    /* �������м� */
    xQueueSet = xQueueCreateSet(QUEUESET_LENGTH);
    /* �������� */
    xQueue1 = xQueueCreate(QUEUESET_LENGTH, QUEUE_ITEM_SIZE);
    xQueue2 = xQueueCreate(QUEUESET_LENGTH, QUEUE_ITEM_SIZE);
    /* ��������ӵ����м��� */
    xQueueAddToSet(xQueue1, xQueueSet);
    xQueueAddToSet(xQueue2, xQueueSet);
    /* ��������1 */
    xTaskCreate((TaskFunction_t )task1,
                (const char*    )"task1",
                (uint16_t       )TASK1_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK1_PRIO,
                (TaskHandle_t*  )&Task1Task_Handler);
    /* ��������2 */
    xTaskCreate((TaskFunction_t )task2,
                (const char*    )"task2",
                (uint16_t       )TASK2_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK2_PRIO,
                (TaskHandle_t*  )&Task2Task_Handler);
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
    uint8_t key         = 0;
    uint8_t eventbit_0  = EVENTBIT_0;
    uint8_t eventbit_1  = EVENTBIT_1;
    
    while (1)
    {
        key = key_scan(0);
        
        switch (key)
        {
            case KEY0_PRES:                                     /* ����1������Ϣ */
            {
                xQueueSend(xQueue1, &eventbit_0, portMAX_DELAY);
                break;
            }
            case KEY1_PRES:                                     /* ����2������Ϣ */
            {
                xQueueSend(xQueue2, &eventbit_1, portMAX_DELAY);
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

/**
 * @brief       task2
 * @param       pvParameters : �������(δ�õ�)
 * @retval      ��
 */
void task2(void *pvParameters)
{
    uint32_t                event_val       = 0;
    uint32_t                event_recv      = 0;
    QueueSetMemberHandle_t  activate_member = NULL;
    uint32_t                task2_num       = 0;
    
    while (1)
    {
        activate_member = xQueueSelectFromSet(xQueueSet, portMAX_DELAY);/* �ȴ����м��еĶ��н��յ���Ϣ */
        
        xQueueReceive(activate_member, &event_recv, portMAX_DELAY);     /* ���ն����е���Ϣ */
        event_val |= event_recv;                                        /* ���յ�����Ϣ�����¼��� */
        lcd_show_xnum(182, 110, event_val, 1, 16, 0, BLUE);             /* ���¼�ֵ��ʾ��LCD�� */
        
        if (event_val == EVENTBIT_ALL)                                  /* �����¼������� */
        {
            event_val = 0;
            lcd_fill(6, 131, 233, 313, lcd_discolor[++task2_num % 11]); /* LCD����ˢ�� */
        }
    }
}
