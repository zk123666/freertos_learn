/**
 ****************************************************************************************************
 * @file        freertos.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.4
 * @date        2022-01-04
 * @brief       FreeRTOS ���м�ʵ��
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
#include "./SYSTEM/usart/usart.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/KEY/key.h"
/*FreeRTOS*********************************************************************************************/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

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

static QueueSetHandle_t xQueueSet;          /* ������м� */
QueueHandle_t           xQueue1;            /* �������1 */
QueueHandle_t           xQueue2;            /* �������2 */
SemaphoreHandle_t       xSemaphore;         /* �����ֵ�ź��� */

#define QUEUE_LENGTH            1                   /* ����֧�ֵ���Ϣ���� */
#define QUEUE_ITEM_SIZE         sizeof(uint32_t)    /* ������ÿ����Ϣ�Ĵ�С */
#define SEMAPHORE_BINARY_LENGTH 1                   /* ��ֵ�ź�������Ч���� */
#define QUEUESET_LENGTH         ((2 * QUEUE_LENGTH) + SEMAPHORE_BINARY_LENGTH)  /* ���м�֧�ֵ���Ϣ���� */

/******************************************************************************************************/

/**
 * @brief       FreeRTOS������ں���
 * @param       ��
 * @retval      ��
 */
void freertos_demo(void)
{
    lcd_show_string(10, 10, 220, 32, 32, "STM32", RED);
    lcd_show_string(10, 47, 220, 24, 24, "Queue set", RED);
    lcd_show_string(10, 76, 220, 16, 16, "ATOM@ALIENTEK", RED);
    
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
    xQueue1 = xQueueCreate(QUEUE_LENGTH, QUEUE_ITEM_SIZE);
    xQueue2 = xQueueCreate(QUEUE_LENGTH, QUEUE_ITEM_SIZE);
    /* ������ֵ�ź��� */
    xSemaphore = xSemaphoreCreateBinary();
    /* �����кͶ�ֵ�ź�����ӵ����м� */
    xQueueAddToSet(xQueue1, xQueueSet);
    xQueueAddToSet(xQueue2, xQueueSet);
    xQueueAddToSet(xSemaphore, xQueueSet);
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
    uint8_t key = 0;
    
    while (1)
    {
        key = key_scan(0);
        
        switch (key)
        {
            case WKUP_PRES:                     /* ����1������Ϣ */
            {
                xQueueSend(xQueue1, &key, portMAX_DELAY);
                break;
            }
            case KEY1_PRES:                     /* ����2������Ϣ */
            {
                xQueueSend(xQueue2, &key, portMAX_DELAY);
                break;
            }
            case KEY0_PRES:                     /* �ͷŶ�ֵ�ź��� */
            {
                xSemaphoreGive(xSemaphore);
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
    QueueSetMemberHandle_t  activate_member = NULL;
    uint32_t                queue_recv      = 0;
    
    while (1)
    {
        activate_member = xQueueSelectFromSet(xQueueSet, portMAX_DELAY);/* �ȴ����м��еĶ��н��յ���Ϣ */
        
        if (activate_member == xQueue1)
        {
            xQueueReceive(activate_member, &queue_recv, portMAX_DELAY);
            printf("���յ�����xQueue1����Ϣ: %d\r\n", queue_recv);
        }
        else if (activate_member == xQueue2)
        {
            xQueueReceive(activate_member, &queue_recv, portMAX_DELAY);
            printf("���յ�����xQueue2����Ϣ: %d\r\n", queue_recv);
        }
        else if (activate_member == xSemaphore)
        {
            xSemaphoreTake(activate_member, portMAX_DELAY);
            printf("��ȡ����ֵ�ź���: xSemaphore\r\n");
        }
    }
}
