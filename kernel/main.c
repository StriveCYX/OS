#include "print.h"
#include "init.h"
#include "debug.h"
#include "thread.h"
#include "interrupt.h"
#include "io.h"
#include "console.h"
/* 临时为测试添加 */
#include "ioqueue.h"
#include "keyboard.h"
#include "process.h"
#include "global.h"

void k_thread_a(void*);
void k_thread_b(void*);
void u_prog_a(void);
void u_prog_b(void);
int test_var_a = 0, test_var_b = 0;

int main(void)
{

	//intr_enable();
	put_str("I am kernel____________\n");
	init_all();
	//asm volatile("sti");
//
	//asm volatile("cli");
	put_str("memory size: ");
	put_int(*(uint32_t*)(MEMORY_SIZE_ADDRESS));
	//intr_enable();
	put_str("test\n");
	put_str("test------------------------\n");
	//intr_disable();
	//thread_start("k_thread_a", 31, k_thread_a, "argA ");
	//intr_enable();
	//thread_start("k_thread_b", 8, k_thread_b, "argB ");
	//thread_start("consumer_a", 31, k_thread_a, " argA_");
	//thread_start("consumer_b", 31, k_thread_b, " argB_");
	put_int(*(uint32_t*)(0xFFF00400));
	put_str("\naddress test\n");

	

	process_execute(u_prog_a, "user_prog_a");
	console_put_str("8**********\n");
	//process_execute(u_prog_b, "user_prog_b");

	intr_enable();// 打开中断,使时钟中断起作用
console_put_str("9**********\n");
	//int c=1000;
	while(1) {
		//console_put_str("Main ");
		uint32_t temp=0;
		console_put_int(temp++);
		//console_put_str("main))))))))))\n");
	};

	return 0;
}


/* 在线程中运行的函数 */
void k_thread_a(void* arg) {
	char* para = arg;
	while(1) {/*
		enum intr_status old_status = intr_disable();
		if (!ioq_empty(&kbd_buf)) {
			console_put_str(arg);
			char byte = ioq_getchar(&kbd_buf);
			console_put_char(byte);
		}
		intr_set_status(old_status);
		*/
		console_put_str("v_a:0x");
		console_put_int(test_var_a);
	}
}

/* 在线程中运行的函数 */
void k_thread_b(void* arg) {
	char* para = arg;
	while(1) {/*
		enum intr_status old_status = intr_disable();
		if (!ioq_empty(&kbd_buf)) {
			console_put_str(arg);
			char byte = ioq_getchar(&kbd_buf);
			console_put_char(byte);
		}
		intr_set_status(old_status);*/
		console_put_str(" v_b:0x");
		console_put_int(test_var_b);
	}
}

/* 测试用户进程 */
void u_prog_a(void) {
	while(1) {
		console_put_str(" u_prog_a\n");
		test_var_a++;
	}
}

void u_prog_b(void) {
	while(1) {
		console_put_str(" u_prog_b\n");
		test_var_b++;
	}
}
