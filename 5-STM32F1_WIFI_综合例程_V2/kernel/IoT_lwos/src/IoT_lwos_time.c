/******************************************************************************
  * @file    IoT_lwos_time.c
  * @author  Yu-ZhongJun
  * @version V0.0.1
  * @date    2018-6-10
  * @brief   IoT_lwos_timeԴ�ļ�
******************************************************************************/
#include "IoT_lwos_time.h"
#include "IoT_lwos_port.h"
#include "IoT_lwos_conf.h"
#include "IoT_lwos_core.h"

extern pIoT_lwos_core_t pIoT_lwos_core;

/******************************************************************************
 *	������:	IoT_lwos_delay
 * ����:  		ticks(IN)	-->��ʱ���ٸ�ticks
 * ����ֵ: 	IoT_lwos_delayִ�н��
 * ����:		��ʾ��ʱ���ٸ�tick
******************************************************************************/
uint8_t IoT_lwos_delay(uint32_t ticks)
{
    uint8_t y;
    if(pIoT_lwos_core->os_int_nesting > 0)
    {
        return OS_RET_DELAY_ISR;
    }

    if(ticks <= 0)
    {
        return OS_RET_DELAY_INVALID;
    }
    /* ����tcb��ʱ */
    IoT_lwos_critial_enter();
    pIoT_lwos_core->os_tcb_current->tcb_delay = ticks;

    /* ��cur task��tcb ready�����Ƴ�Ϊ����һ��task������׼�� */
    y = pIoT_lwos_core->os_tcb_current->tcb_y;
    (pIoT_lwos_core->os_ready_table)[y] &= ~(pIoT_lwos_core->os_tcb_current->tcb_bitx);
    if (pIoT_lwos_core->os_ready_table[y] == 0u)
    {
        pIoT_lwos_core->os_ready_group &= ~(pIoT_lwos_core->os_tcb_current->tcb_bity);
    }
    IoT_lwos_critial_exit();
    IoT_lwos_sched();
    return OS_RET_OK;

}

/******************************************************************************
 *	������:	IoT_lwos_delayHMSM
 * ����:  		hours(IN)	-->��ʱ����Сʱ
 				mins(IN)	-->��ʱ���ٷ���
 				seconds(IN)-->��ʱ������
 				ms(IN)	-->��ʱ���ٺ���
 * ����ֵ: 	IoT_lwos_delayHMSMִ�н��
 * ����:		��ʾ��ʱ���ٸ�Сʱ�����ӣ��룬����
 				������Ҫע�����:�˲��ֻ���������1s�ᴥ��100��systick�ж�
 				�൱��һ��tickΪ10ms���������Ҫ��ʱ4ms����ô���޷�׼ȷ����
 				����Ϊ�˱�֤�����ܵ�׼ȷ����������һ����������ļ��㷨
 				�����>=5ms,��ô����ʱ1tick,���<5ms,��ô�Ͳ���ʱ
******************************************************************************/
uint8_t IoT_lwos_delayHMSM(uint8_t hours,uint8_t mins,uint8_t seconds,uint16_t ms)
{
    uint32_t ticks;
    if(pIoT_lwos_core->os_int_nesting > 0)
    {
        return OS_RET_DELAY_ISR;
    }
    if(mins >59 || seconds >59 || ms>999)
    {
        return OS_RET_DELAY_INVALID;
    }
    /* �˲�����������һ��systick��10ms���������Ҫ��ʱ4ms,��ô�Ͳ��ܴﵽ1��systick,Ϊ�˾��ȸ�׼ȷһ�㣬��������һ����������Ķ��� */
    ticks = (hours*3600+mins*60+seconds)*CONF_TICKS_PER_SEC + (ms+5)/(1000/CONF_TICKS_PER_SEC);

    IoT_lwos_delay(ticks);
    return OS_RET_OK;

}

/******************************************************************************
 *	������:	IoT_lwos_get_tick
 * ����:  		NULL
 * ����ֵ: 	����ϵͳ���е�ʱ��(��tickΪ��λ)
 * ����:		��ȡϵͳ���е�tick��
******************************************************************************/
uint32_t IoT_lwos_get_tick()
{
    return pIoT_lwos_core->os_time;
}

/******************************************************************************
 *	������:	IoT_lwos_set_tick
 * ����:  		ticks(IN)	-->Ҫ���õ�ticks��Ŀ
 * ����ֵ: 	NULL
 * ����:		����ϵͳ���е�tick��
******************************************************************************/
void IoT_lwos_set_tick(uint32_t ticks)
{
    pIoT_lwos_core->os_time = ticks;
}

/******************************************************************************
 *	������:	IoT_lwos_printf_runtime
 * ����:  		NULL
 * ����ֵ: 	NULL
 * ����:		��ӡϵͳ����ʱ�� HOURS:MIN:SEC:MS
******************************************************************************/
void IoT_lwos_printf_runtime()
{

    uint32_t ms_total = (pIoT_lwos_core->os_time)*(1000/CONF_TICKS_PER_SEC);
    uint16_t hours,ms;
    uint8_t mins,seconds;

    hours = ms_total/3600000;
    ms_total -= hours*3600000;

    mins = ms_total/60000;
    ms_total -= mins*60000;

    seconds = ms_total/1000;
    ms_total -= seconds*1000;

    ms = ms_total;
    os_printf("IoT_lwos runing time:%d:%02d:%02d:%d\n",hours,mins,seconds,ms);

}

