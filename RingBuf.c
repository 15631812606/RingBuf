/*--------------------------------------------------------------
 * File: RingBuf.c 编码类型：C语言  UTF-8
 * Author WRS (1352509846@qq.com)
 * 功能: 基于队列实现的环形缓冲区
 * Version 0.1
 * Date 2025-07-16 
 * 
 * @copyright Copyright (c) 2025
 * 
----------------------------------------------------------------*/

#include "RingBuf.h"

/*--------------------------------------------------------------
 * 功能: 初始化环形缓冲区
 * pRingBuf: 环形缓冲区
 * pBuf: 缓冲区
 * bufsize: 缓冲区大小
 * 返回值: RINGBUF_OK/RINGBUF_ERROR
----------------------------------------------------------------*/
eRingBufState_t RingBuf_Init(sRingBuf_t     *pRingBuf,
                             uint8_t        *pBuf,
                             uint32_t       bufsize)
{
    if ((pRingBuf == NULL) || (pBuf == NULL) || (bufsize == 0))
    {
        return RINGBUF_ERROR;
    }

    pRingBuf->pBuf = pBuf;
    pRingBuf->bufsize = bufsize;
    pRingBuf->wIndex = 0;
    pRingBuf->rIndex = 0;
    return RINGBUF_OK;
}

/*--------------------------------------------------------------
 * 功能: 重新初始化环形缓冲区
 * 
 * 参数: pRingBuf
 * 返回值: eRingBufState_t 
----------------------------------------------------------------*/
eRingBufState_t RingBuf_ReInit(sRingBuf_t *pRingBuf)
{
    if (pRingBuf == NULL)
    {
        return RINGBUF_ERROR;
    }
    pRingBuf->wIndex = 0;
    pRingBuf->rIndex = 0;
    return RINGBUF_OK;
}

/*--------------------------------------------------------------
 * 功能: 判断环形缓冲区是否已满
 * 
 * 参数: pRingBuf 
 * 返回值: eRingBufState_t 
----------------------------------------------------------------*/
eRingBufState_t RingBuf_isFull(sRingBuf_t *pRingBuf)
{
    if (pRingBuf == NULL)
    {
        return RINGBUF_ERROR;
    }

    if(((pRingBuf->wIndex+1) % (pRingBuf->bufsize)) == pRingBuf->rIndex)
    {
        return RINGBUF_FULL;
    }
    return RINGBUF_OK;
}

/*--------------------------------------------------------------
 * 功能: 判断环形缓冲区是否为空
 * 
 * 参数: pRingBuf 
 * 返回值: eRingBufState_t 
----------------------------------------------------------------*/
eRingBufState_t RingBuf_isEmpty(sRingBuf_t *pRingBuf)
{
    if (pRingBuf == NULL)
    {
        return RINGBUF_ERROR;
    }

    if(pRingBuf->wIndex == pRingBuf->rIndex)
    {
        return RINGBUF_EMPTY;
    }
    return RINGBUF_NOTEMPTY;
}

/*--------------------------------------------------------------
 * 功能: 写入一个字节
 * 
----------------------------------------------------------------*/
eRingBufState_t RingBuf_WriteOneByte(sRingBuf_t *pRingBuf, uint8_t data)
{
    if (pRingBuf == NULL)
    {
        return RINGBUF_ERROR;
    }
    if(RingBuf_isFull(pRingBuf))
    {
        return RINGBUF_FULL;
    }
    
    pRingBuf->pBuf[pRingBuf->wIndex] = data;
    pRingBuf->wIndex = (pRingBuf->wIndex + 1) % pRingBuf->bufsize;  // 写指针加1,循环到缓冲区尾部
    return RINGBUF_OK;
}

/*--------------------------------------------------------------
 * 功能: 读取一个字节
 * 
 * 参数: pRingBuf 
 * 参数: pBuf 
 * 返回值: eRingBufState_t 
----------------------------------------------------------------*/
eRingBufState_t RingBuf_ReadOneByte(sRingBuf_t *pRingBuf, uint8_t *pBuf)
{
    if (pRingBuf == NULL || pBuf == NULL)
    {
        return RINGBUF_ERROR;
    }
    if(RingBuf_isEmpty(pRingBuf))
    {
        return RINGBUF_EMPTY;
    }
    *pBuf = pRingBuf->pBuf[pRingBuf->rIndex];
    pRingBuf->rIndex = (pRingBuf->rIndex + 1) % pRingBuf->bufsize;  // 读指针加1,循环到缓冲区尾部
    return RINGBUF_OK;
}

/*--------------------------------------------------------------
 * 功能: 写入多数据
 * 
 * 参数: pRingBuf 
 * 参数: wBuf 
 * 参数: len 
 * 返回值: eRingBufState_t 
----------------------------------------------------------------*/
eRingBufState_t RingBuf_Write(sRingBuf_t *pRingBuf, uint8_t *wBuf, uint32_t len)
{
    uint32_t wIndex;
    uint32_t rIndex;
    uint32_t bufsize;
    uint32_t free_space;    // 空闲空间
    uint32_t first_chunk;   // 第一片可用空间

    /* 参数检查 */
    if (!pRingBuf || !wBuf || len == 0)
    {
        return RINGBUF_ERROR;
    }

    /* 原子操作获取当前状态 */
    wIndex = pRingBuf->wIndex;
    rIndex = pRingBuf->rIndex;
    bufsize = pRingBuf->bufsize;
    
    /* 计算实际可用空间 */    
    if (wIndex >= rIndex) 
    {
        free_space = bufsize - wIndex + rIndex - 1;
    }
    else
    {
        free_space = rIndex - wIndex - 1;
    }

    if (len > free_space)
    {
        return RINGBUF_FULL;
    }

    /* 处理可能的分段写入 */
    first_chunk = bufsize - wIndex;
    if (first_chunk >= len)
    {
        memcpy(&pRingBuf->pBuf[wIndex], wBuf, len);
    }
    else
    {
        memcpy(&pRingBuf->pBuf[wIndex], wBuf, first_chunk);
        memcpy(pRingBuf->pBuf, wBuf + first_chunk, len - first_chunk);
    }

    /* 更新写索引（避免取模运算） */
    wIndex += len;
    if (wIndex >= bufsize)
    {
        wIndex -= bufsize;
    }
    pRingBuf->wIndex = wIndex;

    return RINGBUF_OK;
}

/*--------------------------------------------------------------
 * 功能: 读取多数据
 * 
 * 参数: pRingBuf 
 * 参数: rBuf 
 * 参数: len 
 * 返回值: eRingBufState_t 
----------------------------------------------------------------*/
eRingBufState_t RingBuf_Read(sRingBuf_t *pRingBuf, uint8_t *rBuf, uint32_t len)
{
    uint32_t wIndex = 0, rIndex = 0, bufsize = 0;
    uint32_t avail;

    /* 参数检查 */
    if (!pRingBuf || !rBuf || len == 0)
    {
        return RINGBUF_ERROR;
    }

    /* 原子获取索引 */
    wIndex = pRingBuf->wIndex;
    rIndex = pRingBuf->rIndex;
    bufsize = pRingBuf->bufsize;

    /* 计算可读数据量 */
    avail = (wIndex >= rIndex) ? (wIndex - rIndex) : (bufsize - rIndex + wIndex);
   

    /* 分段拷贝处理（处理缓冲区回绕） */
    uint32_t first_chunk = bufsize - rIndex;
    if (first_chunk >= len)
    {
        memcpy(rBuf, &pRingBuf->pBuf[rIndex], len);
    }
    else
    {
        memcpy(rBuf, &pRingBuf->pBuf[rIndex], first_chunk);
        memcpy(rBuf + first_chunk, pRingBuf->pBuf, len - first_chunk);
    }

    /* 原子更新读索引（内存屏障保证可见性） */
    uint32_t new_rIndex = (rIndex + len) % bufsize;
    pRingBuf->rIndex = new_rIndex;

    return RINGBUF_OK;
}

/*--------------------------------------------------------------
 * 功能: 获取数据长度
 * 
 * 参数: pRingBuf 
 * 返回值: uint32_t 
----------------------------------------------------------------*/
uint32_t RingBuf_GetDataLen(sRingBuf_t *pRingBuf)
{
    uint32_t wIndex = 0, rIndex = 0, bufsize = 0;

    if (pRingBuf == NULL)
    {
        return 0;
    }
    
    wIndex = pRingBuf->wIndex;
    rIndex = pRingBuf->rIndex;
    bufsize = pRingBuf->bufsize;
    if(wIndex >= rIndex)
    {
        return wIndex - rIndex;
    }
    else
    {
        return bufsize - rIndex + wIndex;
    }

}

