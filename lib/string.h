#include "stdint.h"

/* 将 dst_起始的 size 个字节置为 value */
void memset(void* dst_, uint8_t value, uint32_t size);


/* 将 src_起始的 size 个字节复制到 dst_ */
void memcpy(void* dst_, const void* src_, uint32_t size);

/* 连续比较以地址 a_和地址 b_开头的 size 个字节,若相等则返回 0,
若 a_大于 b_,返回+1,否则返回−1 */
int memcmp(const void* a_, const void* b_, uint32_t size);

/* 将字符串从 src_复制到 dst_ */
char* strcpy(char* dst_, const char* src_);

/* 返回字符串长度 */
uint32_t strlen(const char* str);


/* 比较两个字符串,若 a_中的字符大于 b_中的字符返回 1,
相等时返回 0,否则返回−1. */
int8_t strcmp (const char* a, const char* b);

/* 从左到右查找字符串 str 中首次出现字符 ch 的地址*/
char* strchr(const char* str, const uint8_t ch);

/* 从后往前查找字符串 str 中首次出现字符 ch 的地址*/
char* strrchr(const char* str, const uint8_t ch);

/* 将字符串 src_拼接到 dst_后,返回拼接的串地址 */
char* strcat(char* dst_, const char* src_);

/* 在字符串 str 中查找字符 ch 出现的次数 */
uint32_t strchrs(const char* str, uint8_t ch);
