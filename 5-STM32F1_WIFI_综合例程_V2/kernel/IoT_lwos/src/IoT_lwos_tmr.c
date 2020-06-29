/******************************************************************************
  * @file    IoT_lwos_tmr.h
  * @author  Yu-ZhongJun
  * @version V0.0.1
  * @date    2018-6-13
  * @brief   timer manager source file
******************************************************************************/
#include "IoT_lwos_tmr.h"
#include "IoT_lwos_conf.h"
#include "IoT_lwos_core.h"

extern pIoT_lwos_core_t pIoT_lwos_core;

/* ���������� */
#if TIMER_MANAGER >0
static uint8_t IoT_lwos_find_idle_tmr(uint8_t *tmr_handle);
#endif


/******************************************************************************
 *	������:	IoT_lwos_tmr_add
 * ����:  		tmr_handle(OUT)		-->add timer����ɹ����صľ��
 				tmr_func(IN)			-->timer�ص�����ָ��
 				para(IN)				-->timer�ص���������
 				ticks(IN)				-->Ҫ׼����ʱ��tick��
 * ����ֵ: 	���timer���ؽ��
 * ����:		��timer���������һ��timer�����ǻ�û������������ready״̬
******************************************************************************/
#if TIMER_MANAGER >0
uint8_t IoT_lwos_tmr_add(uint8_t *tmr_handle,tmr_func tmr_func,void *para,uint32_t ticks)
{
    uint8_t ret;
    uint8_t temp_handle;

    if(tmr_handle == NULL || tmr_func == NULL)
    {
        return OS_RET_TMR_INVALID_PARA;
    }
    ret = IoT_lwos_find_idle_tmr(tmr_handle);
    if(ret == OS_RET_OK)
    {
        IoT_lwos_critial_enter();
        temp_handle = *tmr_handle;
        (pIoT_lwos_core->os_timer)[temp_handle].tmr_handle = temp_handle;
        (pIoT_lwos_core->os_timer)[temp_handle].tmr_status = TMR_STAT_READY;
        (pIoT_lwos_core->os_timer)[temp_handle].tmr_ticks = ticks;
        (pIoT_lwos_core->os_timer)[temp_handle].tmr_func = tmr_func;
        (pIoT_lwos_core->os_timer)[temp_handle].para = para;
        IoT_lwos_critial_exit();
    }
    return ret;
}
#endif


/******************************************************************************
 *	������:	IoT_lwos_tmr_mod
 * ����:  		tmr_handle(IN)		-->timer���
 				ticks(IN)				-->Ҫ�޸ĵ�ʱ�ӽ�����
 * ����ֵ: 	�޸�timer���ؽ��
 * ����:		�޸�һ��timer��ticks
******************************************************************************/
#if TIMER_MANAGER >0
uint8_t IoT_lwos_tmr_mod(uint8_t *tmr_handle,uint32_t ticks)
{

    if(*tmr_handle >= CONF_MAX_TIMER )
    {
        return OS_RET_INAVLID_TMR_HDL;
    }

    if((pIoT_lwos_core->os_timer)[*tmr_handle].tmr_status == TMR_STAT_IDLE)
    {
        return OS_RET_NO_TIMER;
    }

    IoT_lwos_critial_enter();
    (pIoT_lwos_core->os_timer)[*tmr_handle].tmr_ticks = ticks;
    IoT_lwos_critial_exit();

    return OS_RET_OK;
}
#endif
/******************************************************************************
 *	������:	IoT_lwos_tmr_start
 * ����:  		tmr_handle(IN)		-->timer���
 * ����ֵ: 	start timer���ؽ��
 * ����:		����һ��timer
******************************************************************************/
#if TIMER_MANAGER >0
uint8_t IoT_lwos_tmr_start(uint8_t *tmr_handle)
{

    if(*tmr_handle >= CONF_MAX_TIMER )
    {
        return OS_RET_INAVLID_TMR_HDL;
    }

    if((pIoT_lwos_core->os_timer)[*tmr_handle].tmr_status != TMR_STAT_READY)
    {
        return OS_RET_TIMER_NOT_RDY;
    }

    IoT_lwos_critial_enter();
    (pIoT_lwos_core->os_timer)[*tmr_handle].tmr_status = TMR_STAT_START;
    IoT_lwos_critial_exit();
    return OS_RET_OK;
}
#endif
/******************************************************************************
 *	������:	IoT_lwos_tmr_del
 * ����:  		tmr_handle(IN)		-->timer���
 * ����ֵ: 	del timer���ؽ��
 * ����:		ɾ��һ��timer
******************************************************************************/
#if TIMER_MANAGER >0
uint8_t IoT_lwos_tmr_del(uint8_t *tmr_handle)
{
    if(*tmr_handle >= CONF_MAX_TIMER )
    {
        return OS_RET_INAVLID_TMR_HDL;
    }

    if((pIoT_lwos_core->os_timer)[*tmr_handle].tmr_status == TMR_STAT_IDLE)
    {
        return OS_RET_INAVLID_TMR_HDL;
    }

    IoT_lwos_critial_enter();
    (pIoT_lwos_core->os_timer)[*tmr_handle].tmr_handle = 0;
    (pIoT_lwos_core->os_timer)[*tmr_handle].tmr_status = TMR_STAT_IDLE;
    (pIoT_lwos_core->os_timer)[*tmr_handle].tmr_ticks = 0;
    (pIoT_lwos_core->os_timer)[*tmr_handle].tmr_func = 0;
    (pIoT_lwos_core->os_timer)[*tmr_handle].para = 0;
    IoT_lwos_critial_exit();
    return OS_RET_OK;
}
#endif
/******************************************************************************
 *	������:	IoT_lwos_tmr_polling
 * ����:  		NULL
 * ����ֵ: 	NULL
 * ����:		timer polling���鿴�Ƿ���timer��ʱ
******************************************************************************/
#if TIMER_MANAGER >0
void IoT_lwos_tmr_polling()
{
    uint8_t index = 0;
    IoT_lwos_critial_enter();
    for(index = 0; index < CONF_MAX_TIMER; index++)
    {
        if((pIoT_lwos_core->os_timer)[index].tmr_status == TMR_STAT_START)
        {
            (pIoT_lwos_core->os_timer)[index].tmr_ticks--;
            if((pIoT_lwos_core->os_timer)[index].tmr_ticks == 0)
            {
                ((pIoT_lwos_core->os_timer)[index].tmr_func)((pIoT_lwos_core->os_timer)[index].para);
                (pIoT_lwos_core->os_timer)[index].tmr_handle = 0;
                (pIoT_lwos_core->os_timer)[index].tmr_status = TMR_STAT_IDLE;
                (pIoT_lwos_core->os_timer)[index].tmr_ticks = 0;
                (pIoT_lwos_core->os_timer)[index].tmr_func = 0;
                (pIoT_lwos_core->os_timer)[index].para = 0;
            }
        }
    }
    IoT_lwos_critial_exit();
}
#endif
/******************************************************************************
 *	������:	IoT_lwos_find_idle_tmr
 * ����:  		tmr_handle(OUT)	-->����ҵ�timer����ô�ͷ���timer���
 * ����ֵ: 	���ز��ҽ��
 * ����:		���ҿ���timer,�����ؾ��
******************************************************************************/
#if TIMER_MANAGER >0
static uint8_t IoT_lwos_find_idle_tmr(uint8_t *tmr_handle)
{
    uint8_t index = 0;

    if(tmr_handle == NULL)
    {
        return OS_RET_TMR_INVALID_PARA;
    }

    for(index = 0; index < CONF_MAX_TIMER; index++)
    {
        if((pIoT_lwos_core->os_timer)[index].tmr_status == TMR_STAT_IDLE)
        {
            *tmr_handle = index;
            return OS_RET_OK;
        }
    }
    return OS_RET_NO_TIMER;
}
#endif
