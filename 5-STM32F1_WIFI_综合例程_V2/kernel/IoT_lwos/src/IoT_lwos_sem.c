/******************************************************************************
  * @file    IoT_lwos_sem.c
  * @author  Yu-ZhongJun
  * @version V0.0.1
  * @date    2018-6-19
  * @brief   IoT_lwos_semԴ�ļ�
******************************************************************************/
#include "IoT_lwos_sem.h"
#include "IoT_lwos_core.h"

extern pIoT_lwos_core_t pIoT_lwos_core;
extern uint8_t  const  IoT_lwos_unmap_tbl[256];
/* ���������� */
static uint8_t  IoT_lwos_sem_evt_remove (pIoT_lwos_tcb_t   ptcb,pIoT_lwos_sem_t psem);

/******************************************************************************
 *	������:	IoT_lwos_cre_sem
 * ����:  		psem(IN)		-->semָ��
 				cnt(IN)		-->sem�ĳ�ʼ��ֵ
 * ����ֵ: 	����sem�Ľ��
 * ����:		����һ���ź���
******************************************************************************/
uint8_t  IoT_lwos_cre_sem(pIoT_lwos_sem_t psem,uint16_t cnt)
{
    if(psem == NULL)
    {
        return OS_RET_SEM_INVALID_PARA;
    }

    psem->sem_cnt = cnt;

    return OS_RET_OK;
}

/******************************************************************************
 *	������:	IoT_lwos_wait_sem
 * ����:  		psem(IN)		-->semָ��
 				ticks(IN)		-->sem�ȴ�timeoutֵ�����Ϊ0,��һֱ�ȴ�
 * ����ֵ: 	���صȴ��ź����Ľ��
 * ����:		�ȴ�һ���ź���
******************************************************************************/
uint8_t  IoT_lwos_wait_sem(pIoT_lwos_sem_t psem,uint32_t ticks)
{
    uint8_t y;
    uint8_t ret;

    if(psem == NULL)
    {
        return OS_RET_SEM_INVALID_PARA;
    }

    if(pIoT_lwos_core->os_int_nesting > 0)
    {
        return OS_RET_SEM_CREATE_ISR;
    }

    /* sem���ã�ֱ��return */
    if(psem->sem_cnt > 0)
    {
        psem->sem_cnt--;
        return OS_RET_OK;
    }

    IoT_lwos_critial_enter();
    /* sem�����ã���ʼ��tcb�Ĳ���ֵ */
    pIoT_lwos_core->os_tcb_current->os_tcb_status |= TCB_STAT_SEM;
    pIoT_lwos_core->os_tcb_current->os_tcb_pend_status = OS_STAT_PEND_OK;
    pIoT_lwos_core->os_tcb_current->tcb_delay = ticks;
    pIoT_lwos_core->os_tcb_current->os_tcb_sem_ptr = psem;

    y = pIoT_lwos_core->os_tcb_current->tcb_y;
    psem->sem_evt_table[y] |= pIoT_lwos_core->os_tcb_current->tcb_bitx;
    psem->sem_evt_group |= pIoT_lwos_core->os_tcb_current->tcb_bity;


    (pIoT_lwos_core->os_ready_table)[y] &= ~pIoT_lwos_core->os_tcb_current->tcb_bitx;

    if((pIoT_lwos_core->os_ready_table)[y] == 0)
    {
        pIoT_lwos_core->os_ready_group &= ~pIoT_lwos_core->os_tcb_current->tcb_bity;
    }
    IoT_lwos_critial_exit();
    IoT_lwos_sched();
    switch(pIoT_lwos_core->os_tcb_current->os_tcb_pend_status)
    {
    case OS_STAT_PEND_OK:
        ret =  OS_RET_OK;
        break;
    case OS_STAT_PEND_TO:
    default:
    {
        IoT_lwos_sem_evt_remove(pIoT_lwos_core->os_tcb_current, psem);
        ret = OS_RET_SEM_TIEMOUT;
        break;
    }
    }

    IoT_lwos_critial_enter();
    pIoT_lwos_core->os_tcb_current->os_tcb_status |= TCB_STAT_RDY;
    pIoT_lwos_core->os_tcb_current->os_tcb_pend_status = OS_STAT_PEND_OK;
    pIoT_lwos_core->os_tcb_current->os_tcb_sem_ptr = 0;
    IoT_lwos_critial_exit();
    return ret;

}

/******************************************************************************
 *	������:	IoT_lwos_sig_sem
 * ����:  		psem(IN)		-->semָ��
 * ����ֵ: 	���ط����ź����Ľ��
 * ����:		����һ���ź���
******************************************************************************/
uint8_t  IoT_lwos_sig_sem(pIoT_lwos_sem_t psem)
{
    pIoT_lwos_tcb_t   ptcb;
    uint8_t     y;
    uint8_t     x;
    uint8_t     prio;

    if(psem == NULL)
    {
        return OS_RET_SEM_INVALID_PARA;
    }
    /* �����task�ڵ�sem,��ôֱ�ӵ��� */
    if(psem->sem_evt_group != 0)
    {
        IoT_lwos_critial_enter();
        y    = IoT_lwos_unmap_tbl[psem->sem_evt_group];
        x    = IoT_lwos_unmap_tbl[psem->sem_evt_table[y]];
        prio = (uint8_t)((y << 3u) + x);
        ptcb =  pIoT_lwos_core->os_tcb_prio_table[prio];
        ptcb->tcb_delay  =  0;
        ptcb->os_tcb_status = TCB_STAT_RDY;
        ptcb->os_tcb_pend_status = OS_STAT_PEND_OK;

        /* �ѵ�ǰtcb�ĸ��µ������б��� */
        pIoT_lwos_core->os_ready_group |= ptcb->tcb_bity;
        (pIoT_lwos_core->os_ready_table)[y] |= ptcb->tcb_bitx;
        IoT_lwos_critial_exit();
        IoT_lwos_sem_evt_remove(ptcb,psem);
        IoT_lwos_sched();
        return OS_RET_OK;
    }
    /* ���û��task�ڵȣ�������+1 ���� */
    psem->sem_cnt++;
    return OS_RET_OK;
}


/******************************************************************************
 *	������:	IoT_lwos_sem_evt_remove
 * ����:  		ptcb(IN)		-->TCBָ��
 				psem(IN)		-->semָ��
 * ����ֵ: 	ɾ��sem��event�ĳ�Ա���
 * ����:		���sem��event��Ա
******************************************************************************/
static uint8_t  IoT_lwos_sem_evt_remove (pIoT_lwos_tcb_t   ptcb,pIoT_lwos_sem_t psem)
{
    uint8_t y;

    if(ptcb == NULL ||psem == NULL )
    {
        return OS_RET_SEM_INVALID_PARA;
    }
    y = ptcb->tcb_y;
    psem->sem_evt_table[y] &= ~ptcb->tcb_bitx;
    if(psem->sem_evt_table[y] == 0)
    {
        psem->sem_evt_group &= ~ptcb->tcb_bity;
    }
    return OS_RET_OK;
}
