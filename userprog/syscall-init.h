#ifndef SYSCALL_INIT
#define SYSCALL_INIT

#include "stdint.h"
/* 返回当前任务的 pid */ 
uint32_t sys_getpid(void);

/* 打印字符串 str（未实现文件系统前的版本）*/ 
uint32_t sys_write(char* str);

/* 初始化系统调用 */ 
void syscall_init(void);
#endif