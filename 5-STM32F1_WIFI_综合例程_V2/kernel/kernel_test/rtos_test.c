#include "wireless_rtos_wrapper.h"
#include <stdio.h>

#define TASK_STACK 128
#define TASK1_PRIO 11
#define TASK2_PRIO 12
#define TASKSEM_W_PRIO 15
#define TASKSEM_S_PRIO 16
#define TASKMUTEX_PIP_PRIO 5
#define TASKMUTEX1_PRIO 18
#define TASKMUTEX2_PRIO 17

rtos_stack TASK1_TASK[TASK_STACK];
rtos_stack TASK2_TASK[TASK_STACK];
rtos_stack TASKSEM_W_TASK[TASK_STACK];
rtos_stack TASKSEM_S_TASK[TASK_STACK];
rtos_stack TASKMUTEX1_TASK[TASK_STACK];
rtos_stack TASKMUTEX2_TASK[TASK_STACK];

void test_task1(void *para)
{
    while(1)
    {
        printf("task 1 runging\n");
        rtos_delay_ticks(300);

    }
}

void test_task2(void *para)
{
    while(1)
    {
        printf("task 2 runging \n");
        rtos_delay_ticks(200);
    }
}

rtos_tcb task1_tcb;
rtos_tcb task2_tcb;
void test_task_run()
{
    rtos_cre_task(test_task1,0,&task1_tcb,"test_task1",&TASK1_TASK[TASK_STACK-1],&TASK1_TASK[0],TASK_STACK,TASK1_PRIO,20);
    rtos_cre_task(test_task2,0,&task2_tcb,"test_task2",&TASK2_TASK[TASK_STACK-1],&TASK2_TASK[0],TASK_STACK,TASK2_PRIO,20);
}


rtos_sem sem_test;
void test_sem_wait(void *para)
{
    uint8_t ret;
    while(1)
    {
        printf("before wait sem \n");
        ret = rtos_wait_sem(&sem_test,500);
        if(ret == RTOS_ERR_OK)
        {
            printf("after wait sem :ok\n");
        }
        else if(ret == RTOS_ERR_SEM_TIMEOUT)
        {
            printf("after wait sem :timeout\n");
        }
        else
        {
            printf("after wait sem: err\n");
        }
    }
}

void test_sem_sig(void *para)
{
    while(1)
    {
        rtos_delay_ticks(700);
        printf("sig sem\n");
        rtos_sig_sem(&sem_test);

    }
}

rtos_tcb semw_tcb;
rtos_tcb sems_tcb;
void test_sem()
{
    rtos_cre_sem(&sem_test,0,255);
    rtos_cre_task(test_sem_wait,NULL,&semw_tcb,"sem wait",&TASKSEM_W_TASK[TASK_STACK-1],&TASKSEM_W_TASK[0],TASK_STACK,TASKSEM_W_PRIO,5);
    rtos_cre_task(test_sem_sig,NULL,&sems_tcb,"sem sig",&TASKSEM_S_TASK[TASK_STACK-1],&TASKSEM_S_TASK[0],TASK_STACK,TASKSEM_S_PRIO,5);
}

rtos_timer tmr_handle;



#if IOT_LWOS_ENABLE > 0
void timeout_handle(void *para)
#elif USOCII_ENABLE > 0
void timeout_handle(void *tmr,void *para)
#elif USOCIII_ENABLE > 0
void timeout_handle(void *tmr,void *para)
#elif FREERTOS_ENABLE > 0
void timeout_handle(void *para)
#elif RTTHREAD_ENABLE > 0
void timeout_handle(void *para)
#else
#endif
{
    printf("timeout\n");
    rtos_init_tmr(&tmr_handle,"timer handler",timeout_handle,0,500);

    rtos_start_tmr(&tmr_handle);

#if USOCII_ENABLE > 0
    rtos_del_tmr((rtos_timer *)&tmr);
#endif

#if USOCIII_ENABLE > 0
    rtos_del_tmr((rtos_timer *)&tmr);
#endif
}

void test_timer()
{
    rtos_init_tmr(&tmr_handle,"timer handler",timeout_handle,0,500);

    rtos_start_tmr(&tmr_handle);
}


rtos_mutex mutex_test;
void test_mutex_test1(void *para)
{
    while(1)
    {
        rtos_delay_ticks(1);
        rtos_lock_mutex(&mutex_test,0);
        printf("task 1 mutex in\n");
        rtos_delay_ticks(500);
        printf("task 1 mutex out\n");
        rtos_unlock_mutex(&mutex_test);
    }
}

void test_mutex_test2(void *para)
{
    while(1)
    {
        rtos_delay_ticks(1);
        rtos_lock_mutex(&mutex_test,0);
        printf("task 2 mutex in\n");
        rtos_delay_ticks(500);
        printf("task 2 mutex out\n");
        rtos_unlock_mutex(&mutex_test);
    }
}

rtos_tcb mutex1_tcb;
rtos_tcb mutex2_tcb;
void test_mutex()
{
    rtos_cre_mutex(&mutex_test,TASKMUTEX_PIP_PRIO);
    rtos_cre_task(test_mutex_test1,0,&mutex1_tcb,"mutex 1",&TASKMUTEX1_TASK[TASK_STACK-1],&TASKMUTEX1_TASK[0],TASK_STACK,TASKMUTEX1_PRIO,5);
    rtos_cre_task(test_mutex_test2,0,&mutex2_tcb,"mutex 2",&TASKMUTEX2_TASK[TASK_STACK-1],&TASKMUTEX2_TASK[0],TASK_STACK,TASKMUTEX2_PRIO,5);
}

void test_memory()
{
    uint8_t *memory8_1,*memory8_2;
    uint16_t *memory16_1, *memory16_2;

    memory8_1 = rtos_malloc_mem(7);
    memory8_2 = rtos_malloc_mem(13);

    printf("memory8_1 0x%p\n",memory8_1);
    printf("memory8_2 0x%p\n",memory8_2);
    memory16_1 = rtos_calloc_mem(2,16);
    memory16_2 = rtos_calloc_mem(3,16);
    printf("memory16_1 0x%p\n",memory16_1);
    printf("memory16_2 0x%p\n",memory16_2);
    rtos_free_mem(memory8_1);
    rtos_free_mem(memory8_2);
    rtos_free_mem(memory16_1);
    rtos_free_mem(memory16_2);
}
