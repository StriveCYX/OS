#include "thread.h"
#include "stdint.h"
#include "string.h"
#include "global.h"
#include "memory.h"
#include "debug.h"
#include "interrupt.h"
#include "process.h"
//#include "list.h"
#define PG_SIZE 4096

struct task_struct* main_thread;	// 主线程 PCB
struct list thread_ready_list;		// 就绪队列
struct list thread_all_list;		// 所有任务队列
static struct list_elem* thread_tag;// 用于保存队列中的线程结点

extern void switch_to(struct task_struct* cur, struct task_struct* next);

/* 初始化线程栈 thread_stack,
将待执行的函数和参数放到 thread_stack 中相应的位置 */
void thread_create(struct task_struct* pthread, thread_func function, void* func_arg){
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

	/* 确保之前不在队列中 */
	ASSERT(!elem_find(&thread_ready_list, &thread->general_tag));
	/* 加入就绪线程队列 */
	list_append(&thread_ready_list, &thread->general_tag);

	/* 确保之前不在队列中 */
	ASSERT(!elem_find(&thread_all_list, &thread->all_list_tag));
	/* 加入全部线程队列 */
	list_append(&thread_all_list, &thread->all_list_tag);
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
void schedule() {

	ASSERT(intr_get_status() == INTR_OFF);
put_str("1*********\n");
	struct task_struct* cur = running_thread();
	if (cur->status == TASK_RUNNING) {
		// 若此线程只是 cpu 时间片到了,将其加入到就绪队列尾
		ASSERT(!elem_find(&thread_ready_list, &cur->general_tag));
		list_append(&thread_ready_list, &cur->general_tag);
		cur->ticks = cur->priority;
		// 重新将当前线程的 ticks 再重置为其 priority
		cur->status = TASK_READY;
	} else {
		;
		/* 若此线程需要某事件发生后才能继续上 cpu 运行,
		不需要将其加入队列,因为当前线程不在就绪队列中 */
	}
put_str("2*********\n");
	ASSERT(!list_empty(&thread_ready_list));
	thread_tag = NULL;
	// thread_tag 清空
	/* 将 thread_ready_list 队列中的第一个就绪线程弹出,
	准备将其调度上 cpu */
	thread_tag = list_pop(&thread_ready_list);
	struct task_struct* next = elem2entry(struct task_struct, \
	general_tag,  thread_tag);
put_str("3*********\n");
	next->status = TASK_RUNNING;

put_str("10*********\n");
	//激活任务页表等
	process_activate(next);
put_str("11*********\n");
	switch_to(cur, next);
}

/* 当前线程将自己阻塞,标志其状态为 stat. */
void thread_block(enum task_status stat) {
/* stat 取值为 TASK_BLOCKED、TASK_WAITING、TASK_HANGING,
也就是只有这三种状态才不会被调度*/

	ASSERT(((stat == TASK_BLOCKED) || \
			(stat == TASK_WAITING) || \
			(stat == TASK_HANGING)));

	enum intr_status old_status = intr_disable();
	struct task_struct* cur_thread = running_thread();
	cur_thread->status = stat; // 置其状态为 stat

	schedule();		// 将当前线程换下处理器
/* 待当前线程被解除阻塞后才继续运行下面的 intr_set_status */
	intr_set_status(old_status);
}

/* 将线程 pthread 解除阻塞 */
void thread_unblock(struct task_struct* pthread) {
	enum intr_status old_status = intr_disable();
	ASSERT(((pthread->status == TASK_BLOCKED) || (pthread->status == TASK_WAITING) ||(pthread->status == TASK_HANGING)));

	if (pthread->status != TASK_READY) {
		ASSERT(!elem_find(&thread_ready_list, &pthread->general_tag));
		if (elem_find(&thread_ready_list, &pthread->general_tag)) {
			PANIC("thread_unblock: blocked thread in ready_list\n");
		}
		list_push(&thread_ready_list, &pthread->general_tag);
										// 放到队列的最前面,使其尽快得到调度
		pthread->status = TASK_READY;
	}
	intr_set_status(old_status);
}
