#include "timer.h"
#include "io.h"
#include "print.h"
#include "debug.h"
#include "interrupt.h"
#include "thread.h"

#define	IRQ0_FREQUENCY		100
#define	INPUT_FREQUENCY		1193180
#define	COUNTER0_VALUE		INPUT_FREQUENCY / IRQ0_FREQUENCY
#define	CONTRER0_PORT		0x40
#define	COUNTER0_NO			0
#define	COUNTER_MODE		2
#define	READ_WRITE_LATCH	3
#define	PIT_CONTROL_PORT	0x43

uint32_t ticks;		// ticks 是内核自中断开启以来总共的嘀嗒数

/* 时钟的中断处理函数 */
static void intr_timer_handler(void) {
	struct task_struct* cur_thread = running_thread();

	console_put_str("intr_timer_handler put name:");
	console_put_str(cur_thread->name);
	console_put_str("  ");

	ASSERT(cur_thread->stack_magic == 0x19870916); // 检查栈是否溢出

	cur_thread->elapsed_ticks++;	// 记录此线程占用的 cpu 时间
	ticks++;	//从内核第一次处理时间中断后开始至今的滴哒数,内核态和用户态总共的嘀哒数
	if (cur_thread->ticks == 0) {
		// 若进程时间片用完,就开始调度新的进程上 cpu
		schedule();
	} else {
		// 将当前进程的时间片-1
		cur_thread->ticks--;
	}
}

/* 把操作的计数器 counter_no 、 读写锁属性 rwl 、 计数器模式
counter_mode 写入模式控制寄存器并赋予初始值 counter_value */
static void frequency_set(uint8_t counter_port, \
						  uint8_t counter_no, \
						  uint8_t rwl, \
						  uint8_t counter_mode, \
						  uint16_t counter_value) {
/* 往控制字寄存器端口 0x43 中写入控制字 */
	outb(PIT_CONTROL_PORT, \
		(uint8_t)(counter_no << 6 | rwl << 4 | counter_mode << 1));
/* 先写入 counter_value 的低 8 位 */
	outb(counter_port, (uint8_t)counter_value);
/* 再写入 counter_value 的高 8 位 */
	outb(counter_port, (uint8_t)counter_value >> 8);
}

/* 初始化 PIT8253 */
void timer_init() {
	put_str("timer_init start\n");
	/* 设置 8253 的定时周期,也就是发中断的周期 */
	frequency_set(CONTRER0_PORT, \
				  COUNTER0_NO, \
				  READ_WRITE_LATCH,\
				  COUNTER_MODE, \
				  COUNTER0_VALUE);
	register_handler(0x20, intr_timer_handler);
	put_str("timer_init done\n");
}
