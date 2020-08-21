
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            proto.h
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#define in_byte(port)                                   \
        ({                                              \
                unsigned char _v;                       \
                __asm__("inb %%dx,%%al"                 \
                        : "=a"(_v)                      \
                        : "d"((unsigned short)(port))); \
                _v;                                     \
        })

#define out_byte(port, val)                                    \
        __asm__("outb %%al, %%dx" ::"a"((unsigned char)(val)), \
                "d"((unsigned short)(port)))

#define in16(port)                                      \
        ({                                              \
                unsigned short _v;                      \
                __asm__("inw %%dx, %%ax"                \
                        : "=a"(_v)                      \
                        : "d"((unsigned short)(port))); \
                _v;                                     \
        })

#define out16(port, val)                                        \
        __asm__("outw %%ax, %%dx" ::"a"((unsigned short)(val)), \
                "d"((unsigned short)(port)))

#define in32(port)                                      \
        ({                                              \
                unsigned int _v;                        \
                __asm__("inl %%dx, %%eax"               \
                        : "=a"(_v)                      \
                        : "d"((unsigned short)(port))); \
                _v;                                     \
        })

#define out32(port, val)                                       \
        __asm__("outl %%eax, %%dx" ::"a"((unsigned int)(val)), \
                "d"((unsigned short)(port)))

PUBLIC void disp_str(char *info);
PUBLIC void disp_color_str(char *info, int color);
PUBLIC void disable_irq(int irq);
PUBLIC void enable_irq(int irq);

#define enable_int() __asm__("sti")

#define disable_int() __asm__("cli")

#define port_read(port, buf, size) \
        __asm__("cld;rep insw" ::"d"(port), "D"(buf), "c"((size) / 2))

#define port_write(port, buf, size) \
        __asm__("cld;rep outsw" ::"d"(port), "S"(buf), "c"((size) / 2))

PUBLIC void glitter(int row, int col);

/* protect.c */
PUBLIC void init_prot();
PUBLIC u32 seg2phys(u16 seg);
PUBLIC void init_descriptor(struct descriptor *p_desc,
                            u32 base, u32 limit, u16 attribute);

/* klib.c */
PUBLIC void delay(int time);
PUBLIC void disp_int(int input);
PUBLIC char *itoa(char *str, int num);
PUBLIC int atoi(const char *str, int *pRet);
/* kernel.asm */
PUBLIC void restart();

/* main.c */
PUBLIC int get_ticks();
PUBLIC void Init_test();
PUBLIC void TestA();
PUBLIC void TestB();
PUBLIC void TestC();
PUBLIC void panic(const char *fmt, ...);

/* i8259.c */
PUBLIC void init_8259A();
PUBLIC void put_irq_handler(int irq, irq_handler handler);
PUBLIC void spurious_irq(int irq);

/* clock.c */
PUBLIC void clock_handler(int irq);
PUBLIC void init_clock();
PUBLIC void milli_delay(int milli_sec);

/* kernel/hd.c */
PUBLIC void task_hd();
PUBLIC void hd_handler(int irq);

/* keyboard.c */
PUBLIC void init_keyboard();
PUBLIC void keyboard_read(TTY *p_tty);

/* tty.c */
PUBLIC void task_tty();
PUBLIC void in_process(TTY *p_tty, u32 key);
PUBLIC void dump_tty_buf(); /* for debug only */

/* systask.c */
PUBLIC void task_sys();

/* mm/main.c */
PUBLIC void task_mm();
PUBLIC int alloc_mem(int pid, int memsize);
PUBLIC int free_mem(int pid);

/* mm/func.c */
PUBLIC int mm_fork();
PUBLIC void mm_exit(int status);
PUBLIC void mm_wait();

/* fs/main.c */
PUBLIC void task_fs();
PUBLIC int rw_sector(int io_type, int dev, u64 pos,
                     int bytes, int proc_nr, void *buf);
PUBLIC struct inode *get_inode(int dev, int num);
PUBLIC void put_inode(struct inode *pinode);
PUBLIC void sync_inode(struct inode *p);
PUBLIC struct super_block *get_super_block(int dev);

/* fs/open.c */
PUBLIC int do_open();
PUBLIC int do_close();

/* fs/read_write.c */
PUBLIC int do_rdwt();

/* fs/link.c */
PUBLIC int do_unlink();

/* fs/misc.c */
PUBLIC int do_stat();
PUBLIC int strip_path(char *filename, const char *pathname,
                      struct inode **ppinode);
PUBLIC int search_file(char *path);

/* fs/disklog.c */
PUBLIC int do_disklog();
PUBLIC int disklog(char *logstr); /* for debug */
PUBLIC void dump_fd_graph(const char *fmt, ...);

/* console.c */
PUBLIC void out_char(CONSOLE *p_con, char ch);
PUBLIC void scroll_screen(CONSOLE *p_con, int direction);
PUBLIC void select_console(int nr_console);
PUBLIC void init_screen(TTY *p_tty);
PUBLIC int is_current_console(CONSOLE *p_con);

/* printf.c */
PUBLIC int printf(const char *fmt, ...);
PUBLIC int printl(const char *fmt, ...);

/* vsprintf.c */
PUBLIC int vsprintf(char *buf, const char *fmt, va_list args);
PUBLIC int sprintf(char *buf, const char *fmt, ...);

/* proc.c */
PUBLIC void schedule();
PUBLIC void *va2la(int pid, void *va);
PUBLIC int ldt_seg_linear(struct proc *p, int idx);
PUBLIC void reset_msg(MESSAGE *p);
PUBLIC void dump_msg(const char *title, MESSAGE *m);
PUBLIC void dump_proc(struct proc *p);
PUBLIC int send_recv(int function, int src_dest, MESSAGE *msg);
PUBLIC void inform_int(int task_nr);

/* lib/misc.c */
PUBLIC void spin(char *func_name);

/* 以下是系统调用相关 */

/* 系统调用 - 系统级 */
/* proc.c */
PUBLIC int sys_sendrec(int function, int src_dest, MESSAGE *m, struct proc *p);
PUBLIC int sys_printx(int _unused1, int _unused2, char *s, struct proc *p_proc);

/* syscall.asm */
PUBLIC void sys_call(); /* int_handler */

/* 系统调用 - 用户级 */
PUBLIC int sendrec(int function, int src_dest, MESSAGE *p_msg);
PUBLIC int printx(char *str);

#define load_eflags()                 \
        ({                            \
                unsigned int res;     \
                __asm__("pushfl\n\t"  \
                        "popl %0"     \
                        : "=m"(res)); \
                res;                  \
        })

#define store_eflags(e)       \
        __asm__("push %0\n\t" \
                "popfl" ::"m"(e))

#define load_cr0()                          \
        ({                                  \
                unsigned int res;           \
                __asm__("movl %%cr0, %%eax" \
                        : "=a"(res));       \
                res;                        \
        })

#define store_cr0(e) __asm__("movl %0, %%cr0" ::"r"(e))

#define load_cr1()                          \
        ({                                  \
                unsigned int res;           \
                __asm__("movl %%cr1, %%eax" \
                        : "=a"(res));       \
                res;                        \
        })

#define store_cr1(e) __asm__("movl %0, %%cr1" ::"r"(e))

#define load_cr2()                          \
        ({                                  \
                unsigned int res;           \
                __asm__("movl %%cr2, %%eax" \
                        : "=a"(res));       \
                res;                        \
        })

#define store_cr2(e) __asm__("movl %0, %%cr2" ::"r"(e))

#define load_cr3()                       \
        ({                               \
                unsigned int res;        \
                __asm__("movl %%cr3, %0" \
                        : "=m"(res));    \
                res;                     \
        })

#define store_cr3(e) __asm__("movl %0, %%cr3" ::"r"(e))

#undef NULL
#define NULL ((void *)0)

// kernel/main.c
void ProcessManage();
void CreateFile(char *path, char *file);
void DeleteFile(char *path, char *file);
void ReadFile(char *path, char *file);
void WriteFile(char *path, char *file);
void CreateDir(char *path, char *file);
void GoDir(char *path, char *file);
void _showImage(char *path, char *filename);
void saveImage(char *path, char *filename, int w, int h);
void clear();
void help();

// lib/getpid.c
PUBLIC int getpid();

// lib/ls.c
PUBLIC int ls(char *pathName); // 传入当前目录，发送当前目录下的文件名

// lib/printf.c
PUBLIC int printi(u32 num, ...);

// lib/mkdir.c
PUBLIC int mkdir(char* path);