#include "type.h"
#include "stdio.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "hd.h"
#include "fs.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "proto.h"
#include "memory.h"

#define PDE_ADDR 0x200000 // PDE放置位置 大小 固定0x1000 必须按4KB对齐
// 内核在0x100000
#define PTE_ADDR 0x400000 // PTE放置位置 大小 最大0x200000 必须按4KB对齐
// (希望页表没事)
#define MEMMAN_ADDR 0x2000 // 内存分配表放置位置 大小 固定 0x7FE0

unsigned int *pde = (unsigned int *)PDE_ADDR;
unsigned int *pte = (unsigned int *)PTE_ADDR;
MEMMAN *memman = (MEMMAN *)MEMMAN_ADDR;

unsigned char *MemChkBuf;

unsigned int mm_init()
{
    memman->frees = 0;    // 可用信息数目
    memman->maxfrees = 0; // 用于观察可用状况：frees的最大值
    memman->lostsize = 0; // 释放失败的内存的大小总和
    memman->losts = 0;    // 释放失败次数

    // unsigned int total = 0;
    // unsigned int highest = 0;
    // unsigned int valid = 0;

    struct boot_params bp;
    get_boot_params(&bp);

    // unsigned int *mem_buf_count = (unsigned int *)MemChkBuf;
    // unsigned char *mem_buf = MemChkBuf + 4;

    // unsigned int ards_count = *mem_buf_count;
    // ARDStruct ards;
    // for (unsigned int i = 0; i < ards_count; i++)
    // {
    //     memcpy(&ards, mem_buf, sizeof(ARDStruct));
    //     mem_buf += sizeof(ARDStruct);
    //     total += ards.LengthLow;
    //     if (ards.Type == AddressRangeMemory)
    //     {
    //         valid += ards.LengthLow;
    //         if (ards.BaseAddrLow + ards.LengthLow > highest)
    //             highest = ards.BaseAddrLow + ards.LengthLow;
    //     }
    // }
    // setup_paging(highest);

    mm_free((unsigned char *)0x600000, bp.mem_size - 0x600000);

    return bp.mem_size;
}

unsigned int mm_total()
{
    unsigned int i, t = 0;
    for (i = 0; i < memman->frees; i++)
    {
        t += memman->free[i].size;
    }
    return t;
}

unsigned char *mm_alloc(unsigned int size)
{
    for (int i = 0; i < memman->frees; i++)
    {
        if (memman->free[i].size >= size)
        {
            /* 找到了足够大的内存 */
            unsigned char *a = memman->free[i].addr;
            memman->free[i].addr += size;
            memman->free[i].size -= size;
            if (memman->free[i].size == 0)
            {
                /* 如果free[i]变成了0，就减掉一条可用信息 */
                memman->frees--;
                for (; i < memman->frees; i++)
                {
                    memman->free[i] = memman->free[i + 1]; /* 代入结构体 */
                }
            }
            return a;
        }
    }
    return NULL; /* 没有可用空间 */
}

int mm_free(void *addr, unsigned int size)
{
    int i, j;
    /* 为便于归纳内存，将free[]按照addr的顺序排列 */
    /* 所以，先决定应该放在哪里 */
    for (i = 0; i < memman->frees; i++)
    {
        if (memman->free[i].addr > (unsigned char *)addr)
        {
            break;
        }
    }
    /* free[i - 1].addr < addr < free[i].addr */
    if (i > 0)
    {
        /* 前面有可用内存 */
        if (memman->free[i - 1].addr + memman->free[i - 1].size == addr)
        {
            /* 可以与前面的可用内存归纳到一起 */
            memman->free[i - 1].size += size;
            if (i < memman->frees)
            {
                /* 后面也有 */
                if (addr + size == memman->free[i].addr)
                {
                    /* 也可以与后面的可用内存归纳到一起 */
                    memman->free[i - 1].size += memman->free[i].size;
                    /* memman->free[i]删除 */
                    /* free[i]变成0后归纳到前面去 */
                    memman->frees--;
                    for (; i < memman->frees; i++)
                    {
                        memman->free[i] = memman->free[i + 1]; /* 结构体赋值 */
                    }
                }
            }
            return 0; /* 成功完成 */
        }
    }
    /* 不能与前面的可用空间归纳到一起 */
    if (i < memman->frees)
    {
        /* 后面还有 */
        if (addr + size == memman->free[i].addr)
        {
            /* 可以与后面的内容归纳到一起 */
            memman->free[i].addr = addr;
            memman->free[i].size += size;
            return 0; /* 成功完成 */
        }
    }
    /* 既不能与前面归纳到一起，也不能与后面归纳到一起 */
    if (memman->frees < MEMMAN_FREES)
    {
        /* free[i]之后的，向后移动，腾出一点可用空间 */
        for (j = memman->frees; j > i; j--)
        {
            memman->free[j] = memman->free[j - 1];
        }
        memman->frees++;
        if (memman->maxfrees < memman->frees)
        {
            memman->maxfrees = memman->frees; /* 更新最大值 */
        }
        memman->free[i].addr = addr;
        memman->free[i].size = size;
        return 0; /* 成功完成 */
    }
    /* 不能往后移动 */
    memman->losts++;
    memman->lostsize += size;
    return -1; /* 失败 */
}

unsigned char *mm_alloc_4k(unsigned int size)
{
    size = (size + 0xfff) & 0xfffff000;
    return mm_alloc(size);
}

int mm_free_4k(void *addr, unsigned int size)
{
    size = (size + 0xfff) & 0xfffff000;
    return mm_free(addr, size);
}

void setup_paging(unsigned int memsize)
{
    unsigned int pde_count = memsize >> 22;
    if (memsize & 0x3FFFFF)
        pde_count++;
    unsigned int pte_count = pde_count * 1024;

    for (int i = 0; i < pde_count; i++)
    {
        pde[i] = (PTE_ADDR + (i << 12)) | PG_P | PG_RW; // 每个PTE占 4KB
    }

    for (int i = 0; i < pte_count; i++)
    {
        pte[i] = ((i << 12) | PG_P | PG_RW);
    }

    store_cr3(PDE_ADDR);
    unsigned int cr0 = load_cr0();
    store_cr0(cr0 | 0x80000000);
}