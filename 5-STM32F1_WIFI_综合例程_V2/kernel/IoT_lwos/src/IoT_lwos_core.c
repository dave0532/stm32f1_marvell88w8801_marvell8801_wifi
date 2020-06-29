/******************************************************************************
  * @file    IoT_lwos_core.c
  * @author  Yu-ZhongJun
  * @version V0.0.1
  * @date    2018-6-9
  * @brief   IoT_lwos_coreԴ�ļ�
******************************************************************************/
#include "IoT_lwos_core.h"
#include "IoT_lwos_conf.h"
#include "IoT_lwos_task.h"

/* ����IoT_lwos_core��ȫ�ֱ��� */
IoT_lwos_core_t IoT_lwos_core;
pIoT_lwos_core_t pIoT_lwos_core = &IoT_lwos_core;

os_stack IoT_lwos_idle_task_stack[CONF_IDLE_TASK_STK_SIZE];


/* �������� */
static void IoT_lwos_core_reset(void);
static void IoT_lwos_tcb_init_list(void);
static  void  IoT_lwos_find_high_rdy (void);
static  void  IoT_lwos_idle_task_init (void);
#if CALC_CPU_LOADING>0
static void  IoT_lwos_calc_cpu_loading_enter(void);
static void  IoT_lwos_calc_cpu_loading_exit(void);
#endif

/* IoT_lwos��������ȼ�unmap�� */
uint8_t  const  IoT_lwos_unmap_tbl[256] =
{
    0u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x00 to 0x0F */
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x10 to 0x1F */
    5u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x20 to 0x2F */
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x30 to 0x3F */
    6u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x40 to 0x4F */
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x50 to 0x5F */
    5u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x60 to 0x6F */
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x70 to 0x7F */
    7u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x80 to 0x8F */
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x90 to 0x9F */
    5u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0xA0 to 0xAF */
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0xB0 to 0xBF */
    6u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0xC0 to 0xCF */
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0xD0 to 0xDF */
    5u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0xE0 to 0xEF */
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u  /* 0xF0 to 0xFF */
};


/******************************************************************************
 *	������:	IoT_lwos_init
 *	����:		NULL
 *	����ֵ:	NULL
 *	����:		IoT_lwos�ĳ�ʼ��
******************************************************************************/
void IoT_lwos_init(void)
{
    /* core�ĳ�ʼ�� */
    IoT_lwos_core_reset();
    /* ��ʼ��tcb list */
    IoT_lwos_tcb_init_list();
    /* ��ʼ��memory */
#if MEMORY_MGR > 0
    IoT_lwos_mem_init();
#endif
    /* ������������task */
    IoT_lwos_idle_task_init();
}

/******************************************************************************
 *	������:	IoT_lwos_tcb_init
 *	����:		prio(IN)	-->task���ȼ�����
 				ptos(IN)	-->task��ջ��
 *	����ֵ:	task tcb init���
 *	����:		IoT_lwos_tcb_init�ĳ�ʼ��
******************************************************************************/
uint8_t  IoT_lwos_tcb_init (uint8_t prio, os_stack *ptos)
{
    pIoT_lwos_tcb_t ptcb;

    if(ptos == NULL)
    {
        return OS_RET_TASK_PIP_INVALID;
    }

    if(prio > CONF_LOWEST_PRIO)
    {
        return OS_RET_PRIO_INVALID;
    }

    /* �ҳ���һ�����е�tcb */
    ptcb = pIoT_lwos_core->os_tcb_free_list;
    if(ptcb == (pIoT_lwos_tcb_t)0)
    {
        return OS_RET_TASK_NO_MORE_TCB;
    }

    IoT_lwos_critial_enter();
    /* ��tcb freeָ��ָ����һ��tcb�� */
    pIoT_lwos_core->os_tcb_free_list = ptcb->tcb_next;

    ptcb->tcb_stack_ptr = ptos;

    /* �Ѵ��ڵ�tcb����һ����������,pIoT_lwos_core->os_tcb_list��ͷָ�� */
    ptcb->tcb_next = pIoT_lwos_core->os_tcb_list;
    pIoT_lwos_core->os_tcb_list = ptcb;
    ptcb->tcb_delay = 0;
    ptcb->tcb_prio = prio;
    ptcb->tcb_y = (prio >> 3) & 0xff;
    ptcb->tcb_x = prio & 0x07;
    ptcb->tcb_bity = (1 << ptcb->tcb_y) & 0xff;
    ptcb->tcb_bitx = (1 << ptcb->tcb_x) & 0xff;

    /* ����core��Ա */
    (pIoT_lwos_core->os_tcb_prio_table)[prio] = ptcb;
    pIoT_lwos_core->os_ready_group |= ptcb->tcb_bity;
    pIoT_lwos_core->os_tsk_cnt += 1;
    (pIoT_lwos_core->os_ready_table)[ptcb->tcb_y] |= ptcb->tcb_bitx;

    IoT_lwos_critial_exit();
    return OS_RET_OK;
}


/******************************************************************************
 *	������:	IoT_lwos_sched
 *	����:		NULL
 *	����ֵ:	NULL
 *	����:		�鿴�Ƿ��и����ȼ���task���������������ô�ͽ���task�л�
******************************************************************************/
void IoT_lwos_sched(void)
{
    if(pIoT_lwos_core->os_int_nesting == 0)
    {
        IoT_lwos_find_high_rdy();
        pIoT_lwos_core->os_tcb_high_rdy = (pIoT_lwos_core->os_tcb_prio_table)[pIoT_lwos_core->os_prio_high_rdy];
        /* �и��ߵ�task��������Ҫ�л���ջ */
        if(pIoT_lwos_core->os_tcb_high_rdy != pIoT_lwos_core->os_tcb_current)
        {
            (pIoT_lwos_core->os_swcontext_cnt)++;
            IoT_lwos_task_sw();
        }
    }
}

/******************************************************************************
 *	������:	IoT_lwos_start
 *	����:		NULL
 *	����ֵ:	NULL
 *	����:		����������ȼ���ִ��
******************************************************************************/
void IoT_lwos_start(void)
{
    if(pIoT_lwos_core->os_running == FALSE)
    {
        IoT_lwos_find_high_rdy();
        pIoT_lwos_core->os_prio_current = pIoT_lwos_core->os_prio_high_rdy;
        pIoT_lwos_core->os_tcb_high_rdy = (pIoT_lwos_core->os_tcb_prio_table)[pIoT_lwos_core->os_prio_high_rdy];
        pIoT_lwos_core->os_tcb_current = pIoT_lwos_core->os_tcb_high_rdy;
        IoT_lwos_start_high_ready();

    }
}

/******************************************************************************
 *	������:	IoT_lwos_setos_runing
 *	����:		NULL
 *	����ֵ:	NULL
 *	����:		��IoT_lwos��״̬����Ϊruning
******************************************************************************/
void IoT_lwos_setos_runing(void)
{
    pIoT_lwos_core->os_running = TRUE;
}

/******************************************************************************
 *	������:	IoT_lwos_get_task_num
 *	����:		NULL
 *	����ֵ:	����task������
 *	����:		��ȡ���ڵ�task������
******************************************************************************/
uint8_t IoT_lwos_get_task_num(void)
{
	return pIoT_lwos_core->os_tsk_cnt;
}

/******************************************************************************
 *	������:	IoT_lwos_getos_runing
 *	����:		NULL
 *	����ֵ:	NULL
 *	����:		��ȡIoT_lwos��״̬
******************************************************************************/
bool IoT_lwos_getos_runing(void)
{
    return pIoT_lwos_core->os_running;
}

/******************************************************************************
 *	������:	IoT_lwos_update_stack
 *	����:		stack(IN)
 *	����ֵ:	NULL
 *	����:		���µ�ǰtcb��taskָ��
******************************************************************************/
void IoT_lwos_update_stack(os_stack * stack)
{
    pIoT_lwos_core->os_tcb_current->tcb_stack_ptr = stack;
}

/******************************************************************************
 *	������:	IoT_lwos_update_prio
 *	����:		NULL
 *	����ֵ:	NULL
 *	����:		���µ�ǰtask�����ȼ�
******************************************************************************/
void IoT_lwos_update_prio()
{
    pIoT_lwos_core->os_prio_current = pIoT_lwos_core->os_prio_high_rdy;

}

/******************************************************************************
 *	������:	IoT_lwos_update_tcb
 *	����:		NULL
 *	����ֵ:	NULL
 *	����:		���µ�ǰtask��tcb
******************************************************************************/
void IoT_lwos_update_tcb()
{
    pIoT_lwos_core->os_tcb_current = pIoT_lwos_core->os_tcb_high_rdy;
}

/******************************************************************************
 *	������:	IoT_lwos_get_stack
 *	����:		NULL
 *	����ֵ:	���ص�ǰtcb��stackָ��
 *	����:		���ص�ǰtcb��stackָ��
******************************************************************************/
os_stack *IoT_lwos_get_stack()
{
    return pIoT_lwos_core->os_tcb_current->tcb_stack_ptr;
}

/******************************************************************************
 *	������:	IoT_lwos_int_enter
 *	����:		NULL
 *	����ֵ:	NULL
 *	����:		���OS���У���ô�˲��ֻ��ۼӽ���systick���ж�Ƕ�ײ���
******************************************************************************/
void  IoT_lwos_int_enter (void)
{
    if (IoT_lwos_getos_runing() == TRUE)
    {
#if CALC_CPU_LOADING>0
        IoT_lwos_calc_cpu_loading_enter();
#endif
        if (pIoT_lwos_core->os_int_nesting < 255u)
        {
            pIoT_lwos_core->os_int_nesting++;
        }
    }
}

/******************************************************************************
 *	������:	IoT_lwos_int_exit
 *	����:		NULL
 *	����ֵ:	NULL
 *	����:		���OS���У���ô�˲��ֻ��ۼ�����systick���ж�Ƕ�ײ���
 				���ҷ����Ƿ��и������ȼ���ready״̬���Ա�ִ��
******************************************************************************/
void  IoT_lwos_int_exit (void)
{
    if (IoT_lwos_getos_runing() == TRUE)
    {
#if CALC_CPU_LOADING>0
        IoT_lwos_calc_cpu_loading_exit();
#endif
        if (pIoT_lwos_core->os_int_nesting > 0)
        {
            pIoT_lwos_core->os_int_nesting--;
        }

        if(pIoT_lwos_core->os_int_nesting == 0)
        {
            IoT_lwos_find_high_rdy();
            pIoT_lwos_core->os_tcb_high_rdy = (pIoT_lwos_core->os_tcb_prio_table)[pIoT_lwos_core->os_prio_high_rdy];
            /* �и��ߵ�task��������Ҫ�л���ջ */
            if(pIoT_lwos_core->os_tcb_high_rdy != pIoT_lwos_core->os_tcb_current)
            {
                (pIoT_lwos_core->os_swcontext_cnt)++;
                IoT_lwos_task_sw();
            }
        }
    }

}

/******************************************************************************
 *	������:	IoT_lwos_time_tick
 *	����:		NULL
 *	����ֵ:	NULL
 *	����:		polling�Ƿ���task��ʱtimeout
******************************************************************************/
void  IoT_lwos_time_tick(void)
{
    pIoT_lwos_tcb_t ptcb;
    if (IoT_lwos_getos_runing() == TRUE)
    {
        /* ��ѵ�鿴�Ƿ���timer timeout */
#if TIMER_MANAGER >0
        IoT_lwos_tmr_polling();
#endif

        pIoT_lwos_core->os_time++;
        ptcb = pIoT_lwos_core->os_tcb_list;

        while(ptcb->tcb_prio != CONF_IDL_TASK_PRIO)
        {
            /* ������ʱ���⣬�������ʱ��û����һ�ξ�-1,ֱ��Ϊ0�����¼���ready���У��ȴ�����*/
            if(ptcb->tcb_delay != 0)
            {
                ptcb->tcb_delay--;
                if(ptcb->tcb_delay == 0)
                {
                    if((ptcb->os_tcb_status & TCB_STAT_PEND_ANY) != TCB_STAT_RDY)
                    {
                        ptcb->os_tcb_status  &= (uint8_t)~(uint8_t)TCB_STAT_PEND_ANY;
                        ptcb->os_tcb_pend_status = OS_STAT_PEND_TO;
                    }
                    else
                    {

                        ptcb->os_tcb_pend_status = OS_STAT_PEND_OK;
                    }
                    pIoT_lwos_core->os_ready_group |= ptcb->tcb_bity;
                    (pIoT_lwos_core->os_ready_table)[ptcb->tcb_y] |= ptcb->tcb_bitx;
                }
            }

            /* ָ���������һ��tcb */
            ptcb = ptcb->tcb_next;
        }
    }

}


/******************************************************************************
 *	������:	IoT_lwos_calc_cpu_loading_enter
 *	����:		NULL
 *	����ֵ:	NULL
 *	����:		����CPU loading����ں���
 				ԭ��Ϊ:��ÿ��systick�ж�ʱ���ۼ�cpu_loading_tick��
 				���������idle task��ִ�У���ô���ۼ�cpu_idle_loading_tick
******************************************************************************/
#if CALC_CPU_LOADING>0
static void  IoT_lwos_calc_cpu_loading_enter(void)
{
    pIoT_lwos_core->cpu_loading_tick++;
    if(pIoT_lwos_core->os_tcb_current->tcb_prio == CONF_IDL_TASK_PRIO)
    {
        pIoT_lwos_core->cpu_idle_loading_tick++;
    }
}
#endif
/******************************************************************************
 *	������:	IoT_lwos_calc_cpu_loading_exit
 *	����:		NULL
 *	����ֵ:	NULL
 *	����:		����CPU loading�ĳ��ں���,���Ҽ���CPU loading
 				�������cpu_loading_tick�ﵽ��CONF_CALC_CPU_LOADING_TICKS����
 				��ô�ͻ����һ��idle taskִ�д�����ռ��CONF_CALC_CPU_LOADING_TICKS
 				�İٷֱȣ���������Ϊ���еģ�Ҫ�õ�CPU loading����1-����
 				���⣬��Ҫע�����CONF_CALC_CPU_LOADING_TICKSָԽ��ÿ�λ�ȡ��ʱ��
 				��䳤
******************************************************************************/
#if CALC_CPU_LOADING>0
static void  IoT_lwos_calc_cpu_loading_exit(void)
{
    if(pIoT_lwos_core->cpu_loading_tick == CONF_CALC_CPU_LOADING_TICKS)
    {
        pIoT_lwos_core->cpu_loading = 1 - ((fp32)pIoT_lwos_core->cpu_idle_loading_tick) / CONF_CALC_CPU_LOADING_TICKS;
        pIoT_lwos_core->cpu_loading_tick = 0;
        pIoT_lwos_core->cpu_idle_loading_tick = 0;
    }
}
#endif


/******************************************************************************
 *	������:	IoT_lwos_swtsk_hook
 *	����:		NULL
 *	����ֵ:	NULL
 *	����:		�����л��Ĺ��ӳ������APP��Ҫʵʱ��ʾ�����л�
 				����APP������ʵ��IoT_lwos_app_swtsk_hook
 				���Ұ�SW_TASK_HOOK�ĺ궨���
******************************************************************************/
void IoT_lwos_swtsk_hook()
{
#if SW_TASK_HOOK >0
    extern void IoT_lwos_app_swtsk_hook(void);
    IoT_lwos_app_swtsk_hook();
#endif
}

/******************************************************************************
 *	������:	IoT_lwos_get_cpu_loading
 *	����:		NULL
 *	����ֵ:	NULL
 *	����:		��ȡCPU loading
******************************************************************************/
#if CALC_CPU_LOADING>0
fp32 IoT_lwos_get_cpu_loading(void)
{
    return pIoT_lwos_core->cpu_loading;
}
#endif

/******************************************************************************
 *	������:	IoT_core_reset
 *	����:		NULL
 *	����ֵ:	NULL
 *	����:		reset IoT_lwos_core����
******************************************************************************/
static void IoT_lwos_core_reset(void)
{
    os_memset(&IoT_lwos_core, 0, sizeof(IoT_lwos_core_t));
}

/******************************************************************************
 *	������:	IoT_lwos_tcb_init_list
 *	����:		NULL
 *	����ֵ:	NULL
 *	����:		��pIoT_lwos_core�е�tcb table������һ������
 				tcb[0]-->tcb[1]-->tcb[2]-->.........->tcb[CONF_MAX_TCB-1]
 				���Ұ�pIoT_lwos_core->os_tcb_free_listָ��tcb[0]
 				���Ѿ�����tcb�ÿ�
******************************************************************************/
static void IoT_lwos_tcb_init_list(void)
{
    uint8_t index = 0;
    pIoT_lwos_tcb_t ptcb_1;
    pIoT_lwos_tcb_t ptcb_2;
    /* ��core�е�tcb table���һ�������ֱ�ָ����һ��tcb */
    for(index = 0; index < CONF_MAX_TCB - 1; index++)
    {
        ptcb_1 = &(pIoT_lwos_core->os_tcb_table)[index];
        ptcb_2 = &(pIoT_lwos_core->os_tcb_table)[index + 1];
        ptcb_1->tcb_next = ptcb_2;
    }
    ptcb_1 = &(pIoT_lwos_core->os_tcb_table)[index];
    ptcb_1->tcb_next = (pIoT_lwos_tcb_t)0;

    /* ��ָ����ڵ�tcbָ����Ϊ�գ��ѿ��е�tcbָ��ָ���һ��tcb */
    pIoT_lwos_core->os_tcb_list = (pIoT_lwos_tcb_t)0;
    pIoT_lwos_core->os_tcb_free_list = &(pIoT_lwos_core->os_tcb_table)[0];
}

/******************************************************************************
 *	������:	IoT_lwos_find_high_rdy
 *	����:		NULL
 *	����ֵ:	NULL
 *	����:		�ҳ�����ready״̬task��������ȼ�
******************************************************************************/
static  void  IoT_lwos_find_high_rdy (void)
{
    uint8_t y;

    y = IoT_lwos_unmap_tbl[pIoT_lwos_core->os_ready_group];
    pIoT_lwos_core->os_prio_high_rdy = (uint8_t)((y << 3u) + IoT_lwos_unmap_tbl[(pIoT_lwos_core->os_ready_table)[y]]);
}


/******************************************************************************
 *	������:	IoT_lwos_idle_task
 *	����:		NULL
 *	����ֵ:	NULL
 *	����:		idle taskִ�к���
******************************************************************************/
void  IoT_lwos_idle_task(void *p_arg)
{
    p_arg = p_arg;

    while(1)
    {
        pIoT_lwos_core->os_idle_cnt++;
    }
}

/******************************************************************************
 *	������:	IoT_lwos_idle_task_init
 *	����:		NULL
 *	����ֵ:	NULL
 *	����:		������������task
******************************************************************************/
static  void  IoT_lwos_idle_task_init (void)
{
#if IoT_lwos_stack_growth == 1
    IoT_lwos_cre_tsk(IoT_lwos_idle_task,
                     (void *)0,
                     &IoT_lwos_idle_task_stack[CONF_IDLE_TASK_STK_SIZE - 1u],
                     CONF_IDL_TASK_PRIO);
#else
    IoT_lwos_cre_tsk(IoT_lwos_idle_task,
                     (void *)0,
                     &IoT_lwos_idle_task_stack[0],
                     CONF_IDL_TASK_PRIO);
#endif
}
