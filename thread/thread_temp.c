#include "thread.h"
#include "stdint.h"
#include "string.h"
#include "global.h"
#include "memory.h"
#include "interrupt.h"
#include "debug.h"
#include "print.h"
#define PG_SIZE 4096

struct task_struct* main_thread;	// 主线程 PCB
struct list thread_ready_list;		// 就绪队列
struct list thread_all_list;		// 所有任务队列
static struct list_elem* thread_tag;// 用于保存队列中的线程结点

extern void switch_to(struct task_struct* cur, struct task_struct* next);

/* 初始化线程栈 thread_stack,
将待执行的函数和参数放到 thread_stack 中相应的位置 */
void thread_create(struct task_struct* pthread, thread_func function, 						void* func_arg){
	/* 先预留中断使用栈的空间,可见 thread.h 中定义的结构 */
	pthread->self_kstack -= sizeof(struct intr_stack);
	/* 再留出线程栈空间,可见 thread.h 中定义 */
	pthread->self_kstack -= sizeof(struct thread_stack);
	struct thread_stack* kthread_stack = (struct thread_stack*)pthread->self_kstack;
	kthread_stack->eip = kernel_thread;

	kthread_stack->function = function;
	kthread_stack->func_arg = func_arg;
	kthread_stack->ebp = kthread_stack->ebx = \
	kthread_stack->esi = kthread_stack->edi = 0;
}

/* 获取当前线程 pcb 指针 */
struct task_struct* running_thread() {
	uint32_t esp;
	asm ("mov %%esp, %0" : "=g" (esp));
	/* 取 esp 整数部分,即 pcb 起始地址 */
	return (struct task_struct*)(esp & 0xfffff000);
}

/* 由 kernel_thread 去执行 function(func_arg) */
static void kernel_thread(thread_func* function, void* func_arg) {
/* 执行 function 前要开中断,避免后面的时钟中断被屏蔽,而无法调度其他线程 */
	intr_enable();
	function(func_arg);
}

/* 初始化线程基本信息 */
void init_thread(struct task_struct* pthread, char* name, int prio) {
	memset(pthread, 0, sizeof(*pthread));
	strcpy(pthread->name, name);
	if (pthread == main_thread) {
	/* 由于把 main 函数也封装成一个线程,
	并且它一直是运行的,故将其直接设为 TASK_RUNNING */
		pthread->status = TASK_RUNNING;
	} else {
		pthread->status = TASK_READY;
	}

	/* self_kstack 是线程自己在内核态下使用的栈顶地址 */

	pthread->self_kstack = (uint32_t*)((uint32_t)pthread + PG_SIZE);
	pthread->priority = prio;
	pthread->ticks = prio;
	pthread->elapsed_ticks = 0;
	pthread->pgdir = NULL;
	pthread->stack_magic = 0x19870916;
// 自定义的魔数
}

/* 初始化线程环境 */
void thread_init(void) {
	put_str("thread_init start\n");
	list_init(&thread_ready_list);
	list_init(&thread_all_list);
	/* 将当前 main 函数创建为线程 */
	make_main_thread();
	put_str("thread_init done\n");
}

/* 创建一优先级为 prio 的线程,线程名为 name,
线程所执行的函数是 function(func_arg) */


struct task_struct* thread_start(char* name, \
								int prio, \
								thread_func function, \
								void* func_arg) {
	/* pcb 都位于内核空间,包括用户进程的 pcb 也是在内核空间 */
	struct task_struct* thread = get_kernel_pages(1);
	init_thread(thread, name, prio);
	thread_create(thread, function, func_arg);

	put_str("test thread_start\n");
	asm volatile ("movl %0, %%esp; \
	pop %%ebp; pop %%ebx; pop %%edi; pop %%esi; \
	ret": : "g" (thread->self_kstack) : "memory");
	put_str("test thread_start\n");
	return thread;
}

/* 将 kernel 中的 main 函数完善为主线程 */
static void make_main_thread(void) {
/* 因为 main 线程早已运行,
* 咱们在 loader.S 中进入内核时的 mov esp,0xc009f000,
* 就是为其预留 pcb 的,因此 pcb 地址为 0xc009e000,
* 不需要通过 get_kernel_page 另分配一页*/
	main_thread = running_thread();
	init_thread(main_thread, "main", 31);
	/* main 函数是当前线程,当前线程不在 thread_ready_list 中,
	* 所以只将其加在 thread_all_list 中 */
	ASSERT(!elem_find(&thread_all_list, &main_thread->all_list_tag));
	list_append(&thread_all_list, &main_thread->all_list_tag);
}

/* 实现任务调度 */
void schedule(void) {

	ASSERT(intr_get_status() == INTR_OFF);

	struct task_struct* cur = running_thread();
	if (cur->status == TASK_RUNNING) {
		// 若此线程只是 cpu 时间片到了,将其加入到就绪队列尾
		ASSERT(!elem_find(&thread_ready_list, &cur->general_tag));
		list_append(&thread_ready_list, &cur->general_tag);
		cur->ticks = cur->priority;
		// 重新将当前线程的 ticks 再重置为其 priority
		cur->status = TASK_READY;
	} else {
		/* 若此线程需要某事件发生后才能继续上 cpu 运行,
		不需要将其加入队列,因为当前线程不在就绪队列中 */
	}

	ASSERT(!list_empty(&thread_ready_list));
	thread_tag = NULL;
	// thread_tag 清空
	/* 将 thread_ready_list 队列中的第一个就绪线程弹出,
	准备将其调度上 cpu */
	thread_tag = list_pop(&thread_ready_list);
	struct task_struct* next = elem2entry(struct task_struct, \
											general_tag, \ 
											thread_tag);
	next->status = TASK_RUNNING;
	switch_to(cur, next);
}
