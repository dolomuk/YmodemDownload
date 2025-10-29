#include "cqueue.h"
#include "usart.h"

#ifdef USART_QUEUE

#ifndef MEM_MALLOC
	#include "string.h"
	struct SQueue g_queue;
	static __IO uint8_t rx_buffer[DBG_RX_BUFFER_SIZE];
#endif

/**
 * [CreateQueue ? ??]
 * @param  nCount [?? ??]
 * @return        [? ???]
 */
p_queue CreateQueue(uint32_t nCount)
{
#ifdef MEM_MALLOC
    p_queue p = malloc(sizeof(struct SQueue));
    p->m_pData = malloc(sizeof(DATA) * nCount); // ?? ??? ??
    p->m_nCount = nCount; // ? ??
#else
	p_queue p = &g_queue;
    p->m_pData = (DATA *)rx_buffer;
	
	// ? ??
	if(nCount >= DBG_RX_BUFFER_SIZE)
		p->m_nCount = DBG_RX_BUFFER_SIZE;
	else
		p->m_nCount = nCount;
#endif
    p->m_nSize = 0; // ? ? ?? ??
    p->m_nHead = p->m_nTail = 0; // ? ??? ??
    return p;
}
 
/**
 * [RemoveAll ? ? ??? ??]
 * @param p [? ???]
 */
void RemoveAll(p_queue p)
{
    if(!p) return;
#ifdef MEM_MALLOC
	p->m_nCount = p->m_nSize = 0;
    p->m_nHead = p->m_nTail = 0;
    free(p->m_pData);
    free(p);
#else
	memset(p, 0, p->m_nCount);
	p->m_nCount = p->m_nSize = 0;
    p->m_nHead = p->m_nTail = 0;
	p = NULL;
#endif
}
 
/**
 * [GetSize ? ? ?? ?? ????]
 * @param  p [? ???]
 * @return   [? ? ?? ??]
 */
int GetSize(p_queue p)
{
    ASSERT_ERROR(p);
    return p->m_nSize;
}
 
/**
 * [IsFull ?? ?? ??? ??]
 * @param  p [? ???]
 * @return   [?? ?? 1, ??? 0 ??]
 */
int IsFull(p_queue p)
{
    ASSERT_ERROR(p);
    return (p->m_nTail + 1) % p->m_nCount == p->m_nHead;
}
 
/**
 * [IsEmpty ?? ????? ??]
 * @param  p [? ???]
 * @return   [????? 1, ??? 0 ??]
 */
int IsEmpty(p_queue p)
{
    ASSERT_ERROR(p);
    return p->m_nHead == p->m_nTail;
}
 
/**
 * [Push ??? ?? ??]
 * @param  p     [? ???]
 * @param  pdata [??? ??? ??]
 * @return       [????? ???? TRUE, ??? FALSE]
 */
int Push(p_queue p, const DATA *pdata)
{
    ASSERT_ERROR(p);
    if(IsFull(p))
        return FALSE;
    ++(p->m_nSize);
    p->m_nTail = p->m_nTail % p->m_nCount;
    p->m_pData[(p->m_nTail)++] = *pdata;
    return TRUE;
}
 
/**
 * [PushData ??? ?? ??]
 * @param  p [? ???]
 * @param  d [??? ???]
 * @return   [????? ???? TRUE, ??? FALSE]
 */
int PushData(p_queue p, const DATA d)
{
    ASSERT_ERROR(p);
    if(IsFull(p))
        return FALSE;
    ++(p->m_nSize);
    p->m_nTail = p->m_nTail % p->m_nCount;
    p->m_pData[(p->m_nTail)++] = d;
    return TRUE;
}
 
/**
 * [Pop ??? ??? ??]
 * @param  p     [? ???]
 * @param  pdata [??? ???? ?? ???, ?? ??? NULL ??]
 * @return       [????? ???? TRUE, ??? FALSE]
 */
int Pop(p_queue p, DATA *pdata)
{
    ASSERT_ERROR(p);
    if (IsEmpty(p))
        return FALSE;
    --(p->m_nSize);
    p->m_nHead = p->m_nHead % p->m_nCount;
    if(pdata)
        *pdata = p->m_pData[p->m_nHead];
    ++(p->m_nHead);
    return TRUE;
}
#endif /* USART_QUEUE */
