#ifndef TSS
#define TSS
#include "global.h"
#include "thread.h"
 /* 任务状态段 tss 结构 */ 
struct tss; 

 
/* 更新 tss 中 esp0 字段的值为 pthread 的 0 级线 */ 
void update_tss_esp(struct task_struct* pthread);
 
 
/* 在 gdt 中创建 tss 并重新加载 gdt */ 
void tss_init();

#endif
