#ifndef CONSOLE
#define CONSOLE

#include "stdint.h"
/* 初始化终端 */
void console_init();

/* 获取终端 */
void console_acquire();

/* 释放终端 */
void console_release();

/* 终端中输出字符串 */
void console_put_str(char* str);

/* 终端中输出字符 */
void console_put_char(uint8_t char_asci);

/* 终端中输出十六进制整数 */
void console_put_int(uint32_t num);
#endif

