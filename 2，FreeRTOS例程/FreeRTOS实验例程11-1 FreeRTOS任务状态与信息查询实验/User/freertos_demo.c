/**
 ****************************************************************************************************
 * @file        freertos.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.4
 * @date        2022-01-04
 * @brief       FreeRTOS ����״̬����Ϣ��ѯʵ��
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
#include "./MALLOC/malloc.h"
#include "string.h"
/*FreeRTOS*********************************************************************************************/
#include "FreeRTOS.h"
#include "task.h"

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

/******************************************************************************************************/

/**
 * @brief       FreeRTOS������ں���
 * @param       ��
 * @retval      ��
 */
void freertos_demo(void)
{
    lcd_show_string(10, 10, 220, 32, 32, "STM32", RED);
    lcd_show_string(10, 47, 220, 24, 24, "Task Info Query", RED);
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
    uint32_t        i               = 0;
    UBaseType_t     task_num        = 0;
    TaskStatus_t    *status_array   = NULL;
    TaskHandle_t    task_handle     = NULL;
    TaskStatus_t    *task_info      = NULL;
    eTaskState      task_state      = eInvalid;
    char            *task_state_str = NULL;
    char            *task_info_buf  = NULL;
    
    /* ��һ��������uxTaskGetSystemState()��ʹ�� */
    printf("/********��һ��������uxTaskGetSystemState()��ʹ��**********/\r\n");
    task_num = uxTaskGetNumberOfTasks();                            /* ��ȡϵͳ�������� */
    status_array = mymalloc(SRAMIN, task_num * sizeof(TaskStatus_t));
    task_num = uxTaskGetSystemState((TaskStatus_t* )status_array,   /* ����״̬��Ϣbuffer */
                                    (UBaseType_t   )task_num,       /* buffer��С */
                                    (uint32_t*     )NULL);          /* ����ȡ��������ʱ����Ϣ */
    printf("������\t\t���ȼ�\t\t������\r\n");
    for (i=0; i<task_num; i++)
    {
        printf("%s\t%s%ld\t\t%ld\r\n",
                status_array[i].pcTaskName,
                strlen(status_array[i].pcTaskName) > 7 ? "": "\t",
                status_array[i].uxCurrentPriority,
                status_array[i].xTaskNumber);
    }
    myfree(SRAMIN, status_array);
    printf("/**************************����***************************/\r\n");
    printf("����KEY0������!\r\n\r\n\r\n");
    while (key_scan(0) != KEY0_PRES)
    {
        vTaskDelay(10);
    }
    
    /* �ڶ���������vTaskGetInfo()��ʹ�� */
    printf("/************�ڶ���������vTaskGetInfo()��ʹ��**************/\r\n");
    task_info = mymalloc(SRAMIN, sizeof(TaskStatus_t));
    task_handle = xTaskGetHandle("task1");                          /* ��ȡ������ */
    vTaskGetInfo((TaskHandle_t  )task_handle,                       /* ������ */
                 (TaskStatus_t* )task_info,                         /* ������Ϣbuffer */
                 (BaseType_t    )pdTRUE,                            /* ����ͳ�������ջ��ʷ��Сֵ */
                 (eTaskState    )eInvalid);                         /* ��ȡ��������״̬ */
    printf("������:\t\t\t%s\r\n", task_info->pcTaskName);
    printf("������:\t\t%ld\r\n", task_info->xTaskNumber);
    printf("����׳̬:\t\t%d\r\n", task_info->eCurrentState);
    printf("����ǰ���ȼ�:\t\t%ld\r\n", task_info->uxCurrentPriority);
    printf("��������ȼ�:\t\t%ld\r\n", task_info->uxBasePriority);
    printf("�����ջ����ַ:\t\t0x%p\r\n", task_info->pxStackBase);
    printf("�����ջ��ʷʣ����Сֵ:\t%d\r\n", task_info->usStackHighWaterMark);
    myfree(SRAMIN, task_info);
    printf("/**************************����***************************/\r\n");
    printf("����KEY0������!\r\n\r\n\r\n");
    while (key_scan(0) != KEY0_PRES)
    {
        vTaskDelay(10);
    }
    /* ������������eTaskGetState()��ʹ��  */
    printf("/***********������������eTaskGetState()��ʹ��*************/\r\n");
    task_state_str = mymalloc(SRAMIN, 10);
    task_handle = xTaskGetHandle("task1");
    task_state = eTaskGetState(task_handle);                        /* ��ȡ��������״̬ */
    sprintf(task_state_str, task_state == eRunning ? "Runing" :
                            task_state == eReady ? "Ready" :
                            task_state == eBlocked ? "Blocked" :
                            task_state == eSuspended ? "Suspended" :
                            task_state == eDeleted ? "Deleted" :
                            task_state == eInvalid ? "Invalid" :
                                                     "");
    printf("����״ֵ̬: %d����Ӧ״̬Ϊ: %s\r\n", task_state, task_state_str);
    myfree(SRAMIN, task_state_str);
    printf("/**************************����***************************/\r\n");
    printf("����KEY0������!\r\n\r\n\r\n");
    while (key_scan(0) != KEY0_PRES)
    {
        vTaskDelay(10);
    }
    
    /* ���Ĳ�������vTaskList()��ʹ�� */
    printf("/*************���Ĳ�������vTaskList()��ʹ��*************/\r\n");
    task_info_buf = mymalloc(SRAMIN, 500);
    vTaskList(task_info_buf);                                       /* ��ȡ�����������Ϣ */
    printf("������\t\t״̬\t���ȼ�\tʣ��ջ\t�������\r\n");
    printf("%s\r\n", task_info_buf);
    myfree(SRAMIN, task_info_buf);
    printf("/************************ʵ�����***************************/\r\n");
    
    while (1)
    {
        vTaskDelay(10);
    }
}
