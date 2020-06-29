/******************************************************************************
  * @file    IoT_lwos_core.h
  * @author  Yu-ZhongJun
  * @version V0.0.1
  * @date    2018-6-9
  * @brief   core header file
******************************************************************************/
#ifndef OS_CORE_H_H_H
#define OS_CORE_H_H_H

#include "IoT_lwos_conf.h"
#include "IoT_lwos_port.h"
#include "IoT_lwos_tmr.h"
#include "IoT_lwos_sem.h"
#include "IoT_lwos_mutex.h"
#include "IoT_lwos_mem.h"

#define  TCB_STAT_RDY                 0x00
#define  TCB_STAT_SEM                 0x01
#define  TCB_STAT_MUTEX		0x02

#define  TCB_STAT_PEND_ANY         (TCB_STAT_SEM | TCB_STAT_MUTEX)

#define  OS_STAT_PEND_OK                0
#define  OS_STAT_PEND_TO                1



typedef struct IoT_lwos_tcb_pre
{
    os_stack *		tcb_stack_ptr;				/* task tcb�Ķ�ջָ�� */
    struct IoT_lwos_tcb_pre * tcb_next;				/* ָ����һ��tcbָ�� */
    uint8_t 			os_tcb_status;				/* task ״̬ */
    uint8_t			os_tcb_pend_status;		/* task pend״̬ */
    pIoT_lwos_sem_t	os_tcb_sem_ptr;		/* task ָ���ź�����ָ�� */
    pIoT_lwos_mutex_t	os_tcb_mutex_ptr; /* task ָ�򻥳�����ָ�� */
    uint32_t		tcb_delay;					/* task ��ʱ���ٸ�ʱ�� */
    uint8_t			tcb_prio;						/* task ���ȼ� */
    uint8_t			tcb_y;							/* ��Ӧcore os_ready_group��Ա��bitλ���Լ�os_ready_table������ */
    uint8_t			tcb_x;							/* ��Ӧos_ready_table[index]��bit��λ�� */
    uint8_t			tcb_bity;						/* 1<<tcb_y */
    uint8_t			tcb_bitx;						/* 1<<tcb_x */
} IoT_lwos_tcb_t;
typedef IoT_lwos_tcb_t * pIoT_lwos_tcb_t;


typedef struct
{
    volatile  uint32_t 	os_time; 											/* OS���н��� */

    uint8_t				os_int_nesting;  								/* OS�ж�Ƕ�ײ��� */
    uint8_t             		os_tsk_cnt;                								/* OS task���� */
    volatile  uint32_t  	os_idle_cnt;										/* idle task������ */
    bool           			os_running;                 							/* OS�Ƿ����� */
    uint32_t            		os_swcontext_cnt; 							/* OS�л���ջ�Ĵ��� */
    uint8_t           		os_ready_group;      							/* OS������ */
    uint8_t				os_ready_table[CONF_RDY_TBL_SIZE];    	/* OS������ */
    uint8_t				os_prio_current;								 /* ��ǰ���е�task�����ȼ� */
    uint8_t				os_prio_high_rdy;								 /* ����ready״̬��task������ȼ� */
    pIoT_lwos_tcb_t 	os_tcb_current;									/* ��ǰ���е�task��tcb */
    pIoT_lwos_tcb_t 	os_tcb_high_rdy;								/* ����ready״̬��task������ȼ���tcb */
    pIoT_lwos_tcb_t 	os_tcb_prio_table[CONF_LOWEST_PRIO + 1]; /* os prio table��ÿ��ָ��һ��tcb */
    IoT_lwos_tcb_t	os_tcb_table[CONF_MAX_TCB];				/* tcb table */
    pIoT_lwos_tcb_t 	os_tcb_free_list;								/* ָ�����tcb��ָ�� */
    pIoT_lwos_tcb_t 	os_tcb_list;										/* �Ѿ����ڵ�tcbָ�� */

#if CALC_CPU_LOADING>0
    volatile  uint32_t 	cpu_loading_tick;								/* CPU���еĽ����� */
    volatile  uint32_t 	cpu_idle_loading_tick;							/* CPU����idle tick�Ĵ��� */
    fp32					cpu_loading;										/* CPUʹ���� */
#endif

#if TIMER_MANAGER >0
    IoT_lwos_tmr_t 	os_timer[CONF_MAX_TIMER];					/* timer���� */
#endif

#if MEMORY_MGR > 0
    uint8_t *			lwos_ram;
    pIoT_lwos_mem_t lwos_ram_end;									/* ���һ�����ݿ� */
    pIoT_lwos_mem_t lwos_lfree;										/* ָ���һ�����е����ݿ��ָ�� */
#endif
} IoT_lwos_core_t;
typedef IoT_lwos_core_t * pIoT_lwos_core_t;

void IoT_lwos_init(void);
uint8_t  IoT_lwos_tcb_init (uint8_t prio, os_stack *ptos);
void IoT_lwos_sched(void);
void IoT_lwos_start(void);
void IoT_lwos_setos_runing(void);
uint8_t IoT_lwos_get_task_num(void);
#if CALC_CPU_LOADING>0
fp32 IoT_lwos_get_cpu_loading(void);
#endif

#endif
