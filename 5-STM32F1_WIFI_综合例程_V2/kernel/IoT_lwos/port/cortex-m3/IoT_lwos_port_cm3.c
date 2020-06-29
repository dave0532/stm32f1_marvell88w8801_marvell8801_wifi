/******************************************************************************
  * @file    IoT_lwos_port_cm3.c
  * @author  Yu-ZhongJun
  * @version V0.0.1
  * @date    2018-6-9
  * @brief   IoT_lwos����cortex M3����ֲ
******************************************************************************/
#include "IoT_lwos_port.h"
#include "IoT_lwos_task.h"
#include "IoT_lwos_core.h"
#include "IoT_lwos_conf.h"

#define NVIC_INT_CTRL 0xE000ED04	/* �жϿ��ƺ�״̬�Ĵ��� */
#define NVIC_SYSPRI14 0xE000ED22	/* PENDSV�ж����ȼ� */
#define NVIC_PENDSV_PRI 0xFF		/* ������ȼ� */
#define NVIC_PENDSVSET 0x10000000/* �ն˿��ƺ�״̬�Ĵ�����bit28 ����pendsv�ж� */


/******************************************************************************
 *	������:	IoT_lwos_stack_init
 * ����:  		task_function(IN)	-->task ִ�к���
 				p_arg(IN)			-->task ִ�к����Ĳ���
 				ptos(IN)			-->task ִ�к�����ջ��
 * ����ֵ: 	����taskִ�к����Ķ�ջָ��
 * ����:		task���ж�ջָ��ĳ�ʼ����
 				�Ĵ����ĳ�ʼ��˳���ܱ��(����cortex M3�ܹ�)
******************************************************************************/
os_stack *IoT_lwos_stack_init (task_func_t task_function,void *p_arg,os_stack *ptos)
{
    os_stack *stack = ptos;

    if(task_function == NULL || stack == NULL)
    {
    	return NULL;
    }
    /* �˲��ּĴ����ڽ����ж�ʱ��������д�����˳����Զ���ѹ���ջ�� */
    *(stack)    = (uint32_t)0x01000000L;		/* xPSR�Ĵ��� */
    *(--stack)  = (uint32_t)task_function;       /* Entry Point PC�Ĵ��� */
    *(--stack)  = (uint32_t)0xFFFFFFFEL;          /* R14 (LR)�Ĵ��� */
    *(--stack)  = (uint32_t)0x12121212L;          /* R12�Ĵ��� */
    *(--stack)  = (uint32_t)0x03030303L;          /* R3�Ĵ��� */
    *(--stack)  = (uint32_t)0x02020202L;          /* R2�Ĵ��� */
    *(--stack)  = (uint32_t)0x01010101L;          /* R1�Ĵ��� */
    *(--stack)  = (uint32_t)p_arg;                   	/* R0�Ĵ��� : argument */

    /* �˲��ֵļĴ�����Ҫuser�ڽ����жϺ󱸷ݣ��ڶ�ջ�л�ʱ�ָ����Ĵ����� */
    *(--stack)  = (uint32_t)0x11111111L;		/* R11�Ĵ��� */
    *(--stack)  = (uint32_t)0x10101010L;		/* R10�Ĵ��� */
    *(--stack)  = (uint32_t)0x09090909L;		/* R9�Ĵ��� */
    *(--stack)  = (uint32_t)0x08080808L;		/* R8�Ĵ��� */
    *(--stack)  = (uint32_t)0x07070707L;		/* R7�Ĵ��� */
    *(--stack)  = (uint32_t)0x06060606L;		/* R6�Ĵ��� */
    *(--stack)  = (uint32_t)0x05050505L;		/* R5�Ĵ��� */
    *(--stack)  = (uint32_t)0x04040404L;             	/* R4�Ĵ��� */

    return (stack);
}

/******************************************************************************
 *	������:	IoT_lwos_task_sw
 * ����:  		NULL
 * ����ֵ: 	NULL
 * ����:		��Ҫ���ڶ�ջ�л���ԭ���Ǵ���PENDSV���жϣ�
 				��PENDSV�ж��н��ж�ջ�л�
******************************************************************************/
__asm void IoT_lwos_task_sw(void)
{
    /* ��ջ8�ֽڶ��� */
    PRESERVE8

    /* ��R4,R5ѹ���ջ */
    PUSH    {R4, R5}

    /* ����PENDSV�ж� */
    LDR     R4, =NVIC_INT_CTRL
	LDR     R5, =NVIC_PENDSVSET
	STR     R5, [R4]

	/* ��R4,R5�Ӷ�ջ�е��� */
	POP     {R4, R5}

	/* ���� */
	BX      LR
	NOP
}

/******************************************************************************
 *	������:	IoT_lwos_start_high_ready
 * ����:  		NULL
 * ����ֵ: 	NULL
 * ����:		��Ҫ���ڿ�����һ��������ȼ���task������PENDSV�ж�
 				��PENDSV�ж��н��е��õ�һ�����ȼ���ߵ�task
******************************************************************************/
__asm void IoT_lwos_start_high_ready(void)
{
	extern IoT_lwos_setos_runing
	/* ��ջ8�ֽڶ��� */
	PRESERVE8

	/* ����PENDSVΪ��͵����ȼ� */
	LDR     R4, =NVIC_SYSPRI14
	LDR     R5, =NVIC_PENDSV_PRI
	STR     R5, [R4]

	/* ����PSPΪ0 */
	MOV     R4, #0
	MSR     PSP, R4

	/* ����ΪIoT_lwosΪruning״̬ */
	BL IoT_lwos_setos_runing

	/* ����PENDSV�ж� */
	LDR     R4, =NVIC_INT_CTRL
	LDR     R5, =NVIC_PENDSVSET
	STR     R5, [R4]

	/* �����ж� */
	CPSIE   I

IoT_lwos_start_loop
	B	IoT_lwos_start_loop
}


/******************************************************************************
 *	������:	IoT_lwos_pendSV_hdl
 * ����:  		NULL
 * ����ֵ: 	NULL
 * ����:		PENDSV�жϴ���������Ҫ����
 				1.���PSPΪ0,��ô�����е�һ�����ȼ���ߵ�TASK
 				2.���PSP��Ϊ0����ô�ȱ���Ĵ�������ǰ��ջָ����
 				  Ȼ�����л�����һ��������ȼ���task��������ջָ������
******************************************************************************/
__asm void IoT_lwos_pendSV_hdl(void)
{
    extern IoT_lwos_update_prio
    extern IoT_lwos_update_tcb
    extern IoT_lwos_get_stack
    extern IoT_lwos_update_stack
    extern IoT_lwos_swtsk_hook

    /* ��ջ8�ֽڶ��� */
    PRESERVE8

    /* �ر��ж� */
    CPSID   I

    /* ��PSP����R0�Ĵ����� */
    MRS		R0, PSP

    /* ���R0Ϊ0,Ҳ����PSPΪ0,��ô���Ǿ���Ϊ�ǵ�һ������ */
    CBZ		R0, IoT_lwos_pendSV_nosave

    /* ���PSP��Ϊ0,��ô����Ӧ���Ȱ�R4-R11ѹ�뵱ǰTCB�Ķ�ջָ���� */
    SUBS	R0, R0, #0x20
    STM		R0, {R4-R11}

    /* �����µ�PSPָ����µ�OSTCBCur�� */
    BL	IoT_lwos_update_stack

IoT_lwos_pendSV_nosave

    /* ��ȡ����ߵ�ready״̬��task���ȼ�����ֵ��current task prio */
    BL	IoT_lwos_update_prio

	PUSH    {R14}
    LDR     R0, =IoT_lwos_swtsk_hook
    BLX     R0
    POP     {R14}
	
    /* ��ȡ����ߵ�ready״̬��task TCB����ֵ��current task tcb */
    BL	IoT_lwos_update_tcb

    /* ��ȡ����ǰTCB��stackָ��,����ֵ����R0��,R0�����µĶ�ջָ�� */
    BL	IoT_lwos_get_stack

    /* �Ӷ�ջָ���е���R4~R11������Ӧ�Ļظ������Ҹ���PSP */
    LDM     R0, {R4-R11}
    ADDS    R0, R0, #0x20
    MSR     PSP, R0

    /* ����ΪPSP��Ϊ��ջָ�� */
    MOV			LR,#0xfffffffd
    CPSIE   I
    BX      LR
    NOP
}

/******************************************************************************
 *	������:	IoT_lwos_critial_enter
 * ����:  		NULL
 * ����ֵ: 	NULL
 * ����:		�ر��жϲ�����
******************************************************************************/
__asm void IoT_lwos_critial_enter(void)
{	
    CPSID	I
    BX LR
}

/******************************************************************************
 *	������:	IoT_lwos_critial_exit
 * ����:  		NULL
 * ����ֵ: 	NULL
 * ����:		�����жϲ�����
******************************************************************************/
__asm void IoT_lwos_critial_exit(void)
{
    CPSIE	I
    BX LR
}

/******************************************************************************
 *	������:	IoT_lwos_SV_hdl
 * ����:  		NULL
 * ����ֵ: 	NULL
 * ����:		SV�ж�
******************************************************************************/
__asm void IoT_lwos_SV_hdl(void)
{

}


/******************************************************************************
 *	������:	IoT_lwos_hardfault_hdl
 * ����:  		NULL
 * ����ֵ: 	NULL
 * ����:		Ӳ������
******************************************************************************/
void IoT_lwos_hardfault_hdl(void)
{
	while(1);
}
