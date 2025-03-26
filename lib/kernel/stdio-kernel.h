#ifndef __LIB_KERNEL_STDIOSYS_H
#define __LIB_KERNEL_STDIOSYS_H
#include "stdint.h"
/* 供内核使用的格式化输出函数 */
void printk(const char* format, ...);
#endif