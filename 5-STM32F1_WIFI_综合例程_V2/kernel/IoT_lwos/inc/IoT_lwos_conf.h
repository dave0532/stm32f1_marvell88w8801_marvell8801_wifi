/******************************************************************************
  * @file    IoT_lwos_conf.h
  * @author  Yu-ZhongJun
  * @version V0.0.1
  * @date    2018-6-9
  * @brief   IoT_lwos�����ļ�������:
  				1.����ÿ�������Ƿ���
  				2.�������ܵ�������Ϣ
  				3.RET����ֵ
  				4.ͨ����������			
******************************************************************************/

#ifndef OS_CONFIG_H_H_H
#define OS_CONFIG_H_H_H
#include <string.h>
#include <stdio.h>
#include "wireless_rtos_conf.h"
/* 1:���ܿ��� */
#define CALC_CPU_LOADING	1/* ����CPU loading���� */
#define TIMER_MANAGER	1
#define SW_TASK_HOOK		0
#define MEMORY_MGR		1

/* 2.�������ܵ�������Ϣ */
#define CONF_LOWEST_PRIO CONF_RTOS_MAX_PRIO		/* ������ȼ� */
#define CONF_IDL_TASK_PRIO CONF_LOWEST_PRIO		/* ������������ȼ� */
#define CONF_RDY_TBL_SIZE   ((CONF_LOWEST_PRIO) / 8 + 1) /* ��������size���� */
#define CONF_IDLE_TASK_STK_SIZE   CONF_IDLE_TASK_STACK_SIZE
#define CONF_MAX_TCB	20				/* TCB������ */
#define CONF_MAX_TIMER 32			/* timer������ */
#define CONF_TICKS_PER_SEC CONF_RTOS_TICKS_PER_SEC	/* ÿ�����е�tick(10ms����systick�ж�һ��) */
#define CONF_CALC_CPU_LOADING_TICKS	100	/* ���ٸ�tick����һ��CPU loading */
#define CONF_MEM_ALIGNMENT 4 		/* 4�ֽڶ��� */
#define CONF_MIN_SIZE             12		/* ������СΪ12�ֽڣ���ֹ������Ƭ�� */
#define CONF_MEM_SIZE CONF_RTOS_MEM_SIZE	/* ������ֽ��� */


/* 3.ERR����ֵ */
#define OS_RET_OK	0

/* task ��ص�error */
#define OS_RET_PRIO_INVALID	1
#define OS_RET_TASK_CREATE_ISR	2
#define OS_RET_PRIO_EXIST			3
#define OS_RET_TASK_NO_MORE_TCB	4
#define OS_RET_TASK_PIP_INVALID	5

/* time��ص�error */
#define OS_RET_DELAY_ISR	11
#define OS_RET_DELAY_INVALID	12

/* timer��ص�error */
#define OS_RET_NO_TIMER			21
#define OS_RET_TIMER_NOT_RDY	22
#define OS_RET_INAVLID_TMR_HDL	23
#define OS_RET_TMR_INVALID_PARA 24

/* sem��ص�error */
#define OS_RET_SEM_INVALID_PARA		31
#define OS_RET_SEM_CREATE_ISR	32
#define OS_RET_SEM_TIEMOUT	33

/* mutex��ص�error */
#define OS_RET_MUTEX_INVALID_PARA		41
#define OS_RET_MUTEX_CREATE_ISR	42
#define OS_RET_MUTEX_TIEMOUT	43
#define OS_RET_MUTEX_PIP_EXIST	44
#define OS_RET_MUTEX_PIP_LOWER	45
#define OS_RET_MUTEX_NOT_OWER 46

/* 4.ͨ����������	 */
typedef unsigned char 	bool;
#define  FALSE                       0u
#define  TRUE                        1u

#define os_memset	memset
#define os_memcpy	memcpy
#define os_memcmp	memcmp
#define os_printf		printf
#endif

