#ifndef __DEVICE_TIME_H
#define __DEVICE_TIME_H
#include "stdint.h"
void timer_init(void);

/* 以毫秒为单位的 sleep 1 秒= 1000 毫秒 */
void mtime_sleep(uint32_t m_seconds);
#endif
