#include "syscall.h" 
/* 无参数的系统调用 */ 
#define _syscall0(NUMBER) ({                                      \
   int retval;                                                    \
   asm volatile (                                                 \
   "int $0x80"                                                    \
   : "=a" (retval)                                                \
   : "a" (NUMBER)                                                 \
    : "memory"                                                    \
  ) ;                                                             \
                                                                  \
    retval;                                                       \
}) 

/* 一个参数的系统调用 */
#define _syscall1(NUMBER, ARG0) ({			                        \
   int retval;					                                       \
   asm volatile (					                                    \
   "pushl %[arg0]; pushl %[number]; int $0x80; addl $8, %%esp"    \
   : "=a" (retval)					                                 \
   : [number] "i" (NUMBER), [arg0] "g" (ARG0)		               \
   : "memory"						                                    \
   );							                                          \
   retval;						                                       \
})

 /* 三个参数的系统调用 */ 
 
#define _syscall3(NUMBER, ARG1, ARG2, ARG3) ({                    \
     int retval;                                                  \
     asm volatile (                                               \
        "int $0x80"                                               \
        : "=a" (retval)                                           \
        : "a" (NUMBER), "b" (ARG1), "c" (ARG2), "d" (ARG3)        \
        : "memory"                                                \
     ) ;                                                          \
     retval;                                                      \
}) 

/* 打印字符串str */ 
uint32_t write(char* str) { 
   return _syscall1(SYS_WRITE, str); 
}
