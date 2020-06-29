/******************************************************************************
  * @file    IoT_lwos_mem.c
  * @author  Yu-ZhongJun
  * @version V0.0.1
  * @date    2018-7-1
  * @brief   IoT_lwos_memԴ�ļ�
******************************************************************************/
#include "IoT_lwos_mem.h"
#include "IoT_lwos_core.h"

extern pIoT_lwos_core_t pIoT_lwos_core;

/* ȫ�����飬Ҳ����memory����Ĵ�С */
#if MEMORY_MGR > 0
uint8_t lwos_ram_heap[CONF_MEM_SIZE+CONF_MEM_ALIGNMENT+SIZEOF_STRUCT_MEM];
#endif

/* ���������� */
#if MEMORY_MGR > 0
static void IoT_lwos_plug_holes(pIoT_lwos_mem_t mem);
#endif

/******************************************************************************
 *	������:	IoT_lwos_mem_init
 * ����:  		NULL
 * ����ֵ: 	NULL
 * ����:		�ڴ�ĳ�ʼ��
******************************************************************************/
#if MEMORY_MGR > 0
void IoT_lwos_mem_init(void)
{
    pIoT_lwos_mem_t mem;
    /* ��ȫ��������� */
    pIoT_lwos_core->lwos_ram = (uint8_t *)LWOS_MEM_ALIGN(lwos_ram_heap);
    /* ��ʼ����һ��mem�ṹ�� */
    mem = (pIoT_lwos_mem_t)(void *)(pIoT_lwos_core->lwos_ram);
    mem->next = MEM_SIZE_ALIGNED;
    mem->prev = 0;
    mem->used = 0;
    /* ��ʼ�����һ���ṹ�� */
    pIoT_lwos_core->lwos_ram_end = (pIoT_lwos_mem_t)(void *)&(pIoT_lwos_core->lwos_ram)[MEM_SIZE_ALIGNED];
    pIoT_lwos_core->lwos_ram_end->used = 1;
    pIoT_lwos_core->lwos_ram_end->next = MEM_SIZE_ALIGNED;
    pIoT_lwos_core->lwos_ram_end->prev = MEM_SIZE_ALIGNED;

    /* ��freeָ��ָ���һ��mem */
    pIoT_lwos_core->lwos_lfree = (pIoT_lwos_mem_t)(void *)(pIoT_lwos_core->lwos_ram);
}
#endif


/******************************************************************************
 *	������:	IoT_lwos_mem_malloc
 * ����:  		size(IN)		-->Ҫ�����size
 * ����ֵ: 	�ڴ�ָ��
 * ����:		�ڴ�����
******************************************************************************/
#if MEMORY_MGR > 0
void *IoT_lwos_mem_malloc(mem_size_t size)
{
    mem_size_t ptr, ptr2;
    pIoT_lwos_mem_t mem;
    pIoT_lwos_mem_t mem2;

    if (size == 0 || size > MEM_SIZE_ALIGNED)
    {
        return NULL;
    }

    /* ��size�޸�Ϊ4�ֽڵı��� */
    size = LWOS_MEM_ALIGN_SIZE(size);

    if (size < MIN_SIZE_ALIGNED)
    {
        /* ���С����С��size�����size����Ϊ��С��size����ֹ�ڴ���Ƭ */
        size = MIN_SIZE_ALIGNED;
    }

    IoT_lwos_critial_enter();
    for (ptr = (mem_size_t)((uint8_t *)(pIoT_lwos_core->lwos_lfree) - pIoT_lwos_core->lwos_ram); ptr < MEM_SIZE_ALIGNED - size;
            ptr = ((pIoT_lwos_mem_t)(void *)&(pIoT_lwos_core->lwos_ram)[ptr])->next)
    {
        mem = (pIoT_lwos_mem_t)(void *)&(pIoT_lwos_core->lwos_ram)[ptr];

        if ((!mem->used) &&
                (mem->next - (ptr + SIZEOF_STRUCT_MEM)) >= size)
        {

            if (mem->next - (ptr + SIZEOF_STRUCT_MEM) >= (size + SIZEOF_STRUCT_MEM + MIN_SIZE_ALIGNED))
            {
                ptr2 = ptr + SIZEOF_STRUCT_MEM + size;
                /* ����mem2�Ľṹ�� */
                mem2 = (pIoT_lwos_mem_t)(void *)&(pIoT_lwos_core->lwos_ram)[ptr2];
                mem2->used = 0;
                mem2->next = mem->next;
                mem2->prev = ptr;
                /* mem��next�ṹ���Աָ����һ���ĵ�ַ */
                mem->next = ptr2;
                mem->used = 1;

                if (mem2->next != MEM_SIZE_ALIGNED)
                {
                    ((pIoT_lwos_mem_t)(void *)&(pIoT_lwos_core->lwos_ram)[mem2->next])->prev = ptr2;
                }
            }
            else
            {
                mem->used = 1;
            }
            if (mem == (pIoT_lwos_core->lwos_lfree))
            {
                pIoT_lwos_mem_t cur = pIoT_lwos_core->lwos_lfree;
                while (cur->used && cur != (pIoT_lwos_core->lwos_ram_end))
                {
                    cur = (pIoT_lwos_mem_t)(void *)&(pIoT_lwos_core->lwos_ram)[cur->next];
                }
                pIoT_lwos_core->lwos_lfree = cur;
            }
            IoT_lwos_critial_exit();
            return (uint8_t *)mem + SIZEOF_STRUCT_MEM;
        }
    }
    IoT_lwos_critial_exit();
    return NULL;

}
#endif

/******************************************************************************
 *	������:	IoT_lwos_mem_calloc
 * ����:  		count(IN)		-->Ҫ�����count
 				size(IN)		-->Ҫ�����size
 * ����ֵ: 	�ڴ�ָ��
 * ����:		�ڴ�����,����ȫ�����ݸ�ֵΪ0
******************************************************************************/
#if MEMORY_MGR > 0
void *IoT_lwos_mem_calloc(mem_size_t count, mem_size_t size)
{
    void *p;

    if ((count*size) == 0 || (count*size) > MEM_SIZE_ALIGNED)
    {
        return NULL;
    }

    p = IoT_lwos_mem_malloc(count * size);
    if (p)
    {
        os_memset(p, 0, count * size);
    }
    return p;
}
#endif

/******************************************************************************
 *	������:	IoT_lwos_mem_free
 * ����:  		rmem(IN)		-->������ڴ��ַ
 * ����ֵ: 	NULL
 * ����:		�ͷ��ڴ�
******************************************************************************/
#if MEMORY_MGR > 0
void IoT_lwos_mem_free(void *rmem)
{
    pIoT_lwos_mem_t mem;

    if (rmem == NULL)
    {
        return;
    }

    /* �ж�����Ƿ��������ȫ�����ݵ�ַ��Χ�� */
    if ((uint8_t *)rmem < (uint8_t *)(pIoT_lwos_core->lwos_ram) || (uint8_t *)rmem >= (uint8_t *)(pIoT_lwos_core->lwos_ram_end))
    {
        return;
    }
    IoT_lwos_critial_enter();

    mem = (pIoT_lwos_mem_t)(void *)((uint8_t *)rmem - SIZEOF_STRUCT_MEM);
    mem->used = 0;

    if (mem < (pIoT_lwos_core->lwos_lfree))
    {
        pIoT_lwos_core->lwos_lfree = mem;
    }
    IoT_lwos_critial_exit();
    IoT_lwos_plug_holes(mem);

}
#endif

/******************************************************************************
 *	������:	IoT_lwos_plug_holes
 * ����:  		rmem(IN)		-->������ڴ��ַ
 * ����ֵ: 	NULL
 * ����:		�ϲ�ǰ���ͷŵ��ڴ�
******************************************************************************/
#if MEMORY_MGR > 0
static void IoT_lwos_plug_holes(pIoT_lwos_mem_t mem)
{
    pIoT_lwos_mem_t nmem;
    pIoT_lwos_mem_t pmem;

    if(mem == NULL)
    {
        return;
    }
    IoT_lwos_critial_enter();
    nmem = (pIoT_lwos_mem_t)(void *)&(pIoT_lwos_core->lwos_ram)[mem->next];
    if (mem != nmem && nmem->used == 0 && (uint8_t *)nmem != (uint8_t *)(pIoT_lwos_core->lwos_ram_end))
    {
        if ((pIoT_lwos_core->lwos_lfree) == nmem)
        {
            pIoT_lwos_core->lwos_lfree = mem;
        }
        mem->next = nmem->next;
        ((pIoT_lwos_mem_t)(void *)&(pIoT_lwos_core->lwos_ram)[nmem->next])->prev = (mem_size_t)((uint8_t *)mem - (pIoT_lwos_core->lwos_ram));
    }

    pmem = (pIoT_lwos_mem_t)(void *)&(pIoT_lwos_core->lwos_ram)[mem->prev];
    if (pmem != mem && pmem->used == 0)
    {
        if ((pIoT_lwos_core->lwos_lfree) == mem)
        {
            pIoT_lwos_core->lwos_lfree = pmem;
        }
        pmem->next = mem->next;
        ((pIoT_lwos_mem_t)(void *)&(pIoT_lwos_core->lwos_ram)[mem->next])->prev = (mem_size_t)((uint8_t *)pmem - (pIoT_lwos_core->lwos_ram));
    }
    IoT_lwos_critial_exit();
}
#endif

