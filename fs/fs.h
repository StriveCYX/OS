#ifndef __fs_fs_H
#define __fs_fs_H

#include "global.h"

#define MAX_FILES_PER_PART 4096 // 每个分区所支持最大创建的文件数
#define BITS_PER_SECTOR 4096    // 每扇区的位数，就是512B*8bit/B=4096bit
#define SECTOR_SIZE 512         // 扇区字节大小
#define BLOCK_SIZE SECTOR_SIZE  // 块字节大小

extern struct partition* cur_part;  // 默认情况下操作的是哪个分区

/* 文件类型 */
enum file_types
{
    FT_UNKNOWN,  // 不支持的文件类型
    FT_REGULAR,  // 普通文件
    FT_DIRECTORY // 目录
};

/* 在分区链表中找到名为 part_name 的分区，并将其指针赋值给 cur_part */
static bool mount_partition(struct list_elem *pelem, int arg);

/* 格式化分区，也就是初始化分区的元信息，创建文件系统 */
static void partition_format(struct disk *hd, struct partition *part);

/* 在磁盘上搜索文件系统，若没有则格式化分区创建文件系统 */
void filesys_init();

/* 将最上层路径名称解析出来 */
static char *path_parse(char *pathname, char *name_store);

/* 返回路径深度，比如/a/b/c，深度为 3 */
int32_t path_depth_cnt(char *pathname);
#endif