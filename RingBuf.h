/*--------------------------------------------------------------
 * File: RingBuf.h 编码类型：C语言  UTF-8
 * Author WRS (1352509846@qq.com)
 * 功能: 基于队列实现的环形缓冲区
 * Version 0.1
 * Date 2025-07-16 
 * 
 * @copyright Copyright (c) 2025
 * 
----------------------------------------------------------------*/
#include <stdint.h>

// ringbuf的结构体
typedef struct __RingBuf
{
    uint8_t *pBuf;      // 缓冲区
    uint32_t bufsize;   // 缓冲区大小
    uint32_t wIndex;    // 写指针
    uint32_t rIndex;    // 读指针    
}sRingBuf_t;


typedef enum __RingBufState
{
    RINGBUF_OK = 0,
    RINGBUF_ERROR,
    RINGBUF_EMPTY,
    RINGBUF_NOTEMPTY,
    RINGBUF_FULL,
    RINGBUF_HALFULL,    
}eRingBufState_t;


eRingBufState_t RingBuf_Init(sRingBuf_t *pRingBuf, uint8_t *pBuf, uint32_t bufsize);
eRingBufState_t RingBuf_ReInit(sRingBuf_t *pRingBuf);
eRingBufState_t RingBuf_isFull(sRingBuf_t *pRingBuf);
eRingBufState_t RingBuf_isEmpty(sRingBuf_t *pRingBuf);
eRingBufState_t RingBuf_WriteOneByte(sRingBuf_t *pRingBuf, uint8_t data);
eRingBufState_t RingBuf_ReadOneByte(sRingBuf_t *pRingBuf, uint8_t *pBuf);
eRingBufState_t RingBuf_Write(sRingBuf_t *pRingBuf, uint8_t *wBuf, uint32_t len);
eRingBufState_t RingBuf_Read(sRingBuf_t *pRingBuf, uint8_t *rBuf, uint32_t len);
uint32_t RingBuf_GetDataLen(sRingBuf_t *pRingBuf);

