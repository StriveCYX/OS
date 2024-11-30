#include "init.h"
#include "print.h"
#include "interrupt.h"
#include "memory.h"
#include "thread.h"
#include "keyboard.h"
#include "tss.h"
/*负责初始化所有模块 */
void init_all() {
	put_str("init_all\n");
	idt_init();	//初始化中断
	timer_init();	// 初始化 PIT
	mem_init();	//初始化内存池
	thread_init();
	console_init();
	keyboard_init();
	tss_init();		/* 在 gdt 中创建 tss 并重新加载 gdt */
	

}
