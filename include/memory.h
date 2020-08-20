#pragma once

// memtest.c
#define EFLAGS_AC_BIT     0x00040000
#define CR0_CACHE_DISABLE 0x60000000

// memory.c
#define MEMMAN_FREES 4090 /* 大约是32KB*/

typedef struct s_FREEINFO
{
    unsigned char *addr;
    unsigned int size;
} FREEINFO;

typedef struct s_MEMMAN
{
    int frees, maxfrees, lostsize, losts;
    FREEINFO free[MEMMAN_FREES];
} MEMMAN;

unsigned int mm_init();
unsigned int mm_total();
unsigned char *mm_alloc(unsigned int size);
int mm_free(void *addr, unsigned int size);
unsigned char *mm_alloc_4k(unsigned int size);
int mm_free_4k(void *addr, unsigned int size);

typedef struct s_ard
{
    // Address Range Descriptor Structure
    unsigned int BaseAddrLow;
    unsigned int BaseAddrHigh;
    unsigned int LengthLow;
    unsigned int LengthHigh;
    unsigned int Type;
} ARDStruct;

#define AddressRangeMemory 1 // OS可用
// #define AddressRangeReserved 2

extern unsigned char *MemChkBuf; // from boot1

// 分页机制
#define PG_P   0x1   //   ; 页存在属性位
#define PG_RW  0x2   //   ; R/W 属性位值, 0表示只读, 1表示读写
#define PG_US  0x4   //   ; U/S 属性位值, 0表示系统级, 1表示用户级
#define PG_PWT 0x8   //   ; 写直达
#define PG_PCD 0x10  //   ; 禁用缓存
#define PG_A   0x20  //   ; 可访问
#define PG_PS  0x80  //   ; 页大小(默认4k)
#define PG_G   0x100 //   ;

// PTE
#define PG_D   0x40 //
#define PG_PAT 0x80 //

void setup_paging(unsigned int memsize);