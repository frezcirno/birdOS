
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            main.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "stdio.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "fs.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "proto.h"
#include "2048.h"
#include "flappy.h"
#include "memory.h"
#include "glib.h"
#include "bmp.h"

int headercolormode = PEN_LIGHT_BLUE;

/*======================================================================*
                            kernel_main
 *======================================================================*/
PUBLIC int kernel_main()
{
    disp_str("-----\"kernel_main\" begins-----\n");

    struct task *p_task;
    struct proc *p_proc = proc_table;
    char *p_task_stack = task_stack + STACK_SIZE_TOTAL;
    // u16   selector_ldt = SELECTOR_LDT_FIRST;
    u8 privilege;
    u8 rpl;
    int eflags;
    int i, j;
    int prio;
    for (i = 0; i < NR_TASKS + NR_PROCS; i++)
    {
        if (i >= NR_TASKS + NR_NATIVE_PROCS)
        {
            p_proc->p_flags = FREE_SLOT;
            p_proc++;
            p_task++;
            continue;
        }
        if (i < NR_TASKS)
        { /* 任务 */
            p_task = task_table + i;
            privilege = PRIVILEGE_TASK;
            rpl = RPL_TASK;
            eflags = 0x1202; /* IF=1, IOPL=1, bit 2 is always 1   1 0010 0000 0010(2)*/
            prio = 15;       //设定优先级为15
        }
        else
        { /* 用户进程 */
            p_task = user_proc_table + (i - NR_TASKS);
            privilege = PRIVILEGE_USER;
            rpl = RPL_USER;
            eflags = 0x202; /* IF=1, bit 2 is always 1              0010 0000 0010(2)*/
            prio = 5;       //设定优先级为5
        }

        strcpy(p_proc->name, p_task->name); /* 设定进程名称 */
        p_proc->p_parent = NO_TASK;

        if (strcmp(p_task->name, "Init") != 0)
        {
            p_proc->ldts[INDEX_LDT_C] = gdt[SELECTOR_KERNEL_CS >> 3];
            p_proc->ldts[INDEX_LDT_RW] = gdt[SELECTOR_KERNEL_DS >> 3];

            /* change the DPLs */
            p_proc->ldts[INDEX_LDT_C].attr1 = DA_C | privilege << 5;
            p_proc->ldts[INDEX_LDT_RW].attr1 = DA_DRW | privilege << 5;
        }
        else
        { /* INIT process */
            unsigned int k_base;
            unsigned int k_limit;
            int ret = get_kernel_map(&k_base, &k_limit);
            assert(ret == 0);
            init_descriptor(&p_proc->ldts[INDEX_LDT_C],
                            0, /* bytes before the entry point
                      * are useless (wasted) for the
                      * INIT process, doesn't matter
                      */
                            (k_base + k_limit) >> LIMIT_4K_SHIFT,
                            DA_32 | DA_LIMIT_4K | DA_C | privilege << 5);

            init_descriptor(&p_proc->ldts[INDEX_LDT_RW],
                            0, /* bytes before the entry point
                      * are useless (wasted) for the
                      * INIT process, doesn't matter
                      */
                            (k_base + k_limit) >> LIMIT_4K_SHIFT,
                            DA_32 | DA_LIMIT_4K | DA_DRW | privilege << 5);
        }

        p_proc->regs.cs = INDEX_LDT_C << 3 | SA_TIL | rpl;
        p_proc->regs.ds =
            p_proc->regs.es =
                p_proc->regs.fs =
                    p_proc->regs.ss = INDEX_LDT_RW << 3 | SA_TIL | rpl;
        p_proc->regs.gs = (SELECTOR_KERNEL_GS & SA_RPL_MASK) | rpl;

        p_proc->regs.eip = (u32)p_task->initial_eip;
        p_proc->regs.esp = (u32)p_task_stack;
        p_proc->regs.eflags = eflags;

        /* p_proc->nr_tty       = 0; */

        p_proc->p_flags = 0;
        p_proc->p_msg = 0;
        p_proc->p_recvfrom = NO_TASK;
        p_proc->p_sendto = NO_TASK;
        p_proc->has_int_msg = 0;
        p_proc->q_sending = 0;
        p_proc->next_sending = 0;
        p_proc->pid = i;

        for (j = 0; j < NR_FILES; j++)
            p_proc->filp[j] = 0;

        p_proc->ticks = p_proc->priority = prio;

        p_task_stack -= p_task->stacksize;
        p_proc++;
        p_task++;
        // selector_ldt += 1 << 3;
    }

    /* proc_table[NR_TASKS + 0].nr_tty = 0; */
    /* proc_table[NR_TASKS + 1].nr_tty = 1; */
    /* proc_table[NR_TASKS + 2].nr_tty = 1; */

    k_reenter = 0;
    ticks = 0;

    p_proc_ready = proc_table;

    init_clock();
    init_keyboard();

    // mm_init();
    init_video();

    restart();

    while (1)
    {
    }
}

void Init_test()
{
    char tty_name[] = "/dev_tty2";

    char rdbuf[128];

    int fd_stdin = open(tty_name, O_RDWR);
    assert(fd_stdin == 0);
    int fd_stdout = open(tty_name, O_RDWR);
    assert(fd_stdout == 1);

    printf("Init() is running ...\n");

    while (1)
    {
        int r = read(fd_stdin, rdbuf, 70);
        rdbuf[r] = 0;
        printf("Before fork()");
        int pid = fork();
        if (pid != 0)
        { /* parent process */
            printf("parent is running, child pid:%d\n", pid);
            spin("parent");
        }
        else
        { /* child process */
            printf("child is running, pid:%d\n", getpid());
            spin("child");
        }
    }
}

/*****************************************************************************
 *                                get_ticks
 *****************************************************************************/
PUBLIC int get_ticks()
{
    MESSAGE msg;
    reset_msg(&msg);
    msg.type = GET_TICKS;
    send_recv(BOTH, TASK_SYS, &msg);
    return msg.RETVAL;
}

/*****************************************************************************
 *                                convert_to_absolute
 *                      将传入的路径和文件名组合成一个完整的绝对路径
 *****************************************************************************/
PUBLIC void convert_to_absolute(char *dest, char *path, char *file)
{
    int i = 0, j = 0;
    while (path[i] != 0) // 写入路径
    {
        dest[j] = path[i];
        j++;
        i++;
    }
    i = 0;
    while (file[i] != 0) // 写入文件名
    {
        dest[j] = file[i];
        j++;
        i++;
    }
    dest[j] = 0; // 结束符
}

/*======================================================================*
                               TestA
 *======================================================================*/

//1号终端
void TestA()
{
    char tty_name[] = "/dev_tty0";

    char rdbuf[256];
    char cmd[20];
    char filename1[128];
    char filename2[128];

    int fd_stdin = open(tty_name, O_RDWR);
    assert(fd_stdin == 0);
    int fd_stdout = open(tty_name, O_RDWR);
    assert(fd_stdout == 1);

    //  char filename[MAX_FILENAME_LEN+1] = "zsp01";
    // const char bufw[80] = {0};
    //  const int rd_bytes = 3;
    //  char bufr[rd_bytes];
    sl();
    clear();

    char current_dirr[512] = "/"; // 记录当前路径（其实路径字符长度上限为MAX_PATH）

    while (1)
    {
        printf("[root@BirdOS: %s] $ ", current_dirr); // 打印当前路径
        int r = read(fd_stdin, rdbuf, 512);
        rdbuf[r] = 0;

        // 解析命令
        int pos = 0;
        while (rdbuf[pos] != ' ' && rdbuf[pos] != 0) // 读取指令
        {
            cmd[pos] = rdbuf[pos];
            pos++;
        }
        cmd[pos] = 0;
        if (rdbuf[pos] != 0) // 指令还未结束
        {
            pos++;
            int len = pos;
            while (rdbuf[pos] != ' ' && rdbuf[pos] != 0) // 读取第一个文件名
            {
                filename1[pos - len] = rdbuf[pos];
                pos++;
            }
            filename1[pos - len] = 0;
        }
        if (rdbuf[pos] != 0) // 指令还未结束
        {
            pos++;
            int len = pos;
            while (rdbuf[pos] != ' ' && rdbuf[pos] != 0) // 读取第二个文件名
            {
                filename2[pos - len] = rdbuf[pos];
                pos++;
            }
            filename2[pos - len] = 0;
        }
        if (strcmp(cmd, "process") == 0)
        {
            runProcessManage(fd_stdin);
        }
        else if (strcmp(cmd, "ls") == 0)
        {
            ls(current_dirr);
        }
        else if (strcmp(cmd, "touch") == 0) // 创建文件
        {
            CreateFile(current_dirr, filename1);
        }
        else if (strcmp(cmd, "rm") == 0) // 删除文件
        {
            DeleteFile(current_dirr, filename1);
        }
        else if (strcmp(cmd, "cat") == 0) // 打印文件内容
        {
            ReadFile(current_dirr, filename1);
        }
        else if (strcmp(cmd, "vi") == 0) // 写文件
        {
            WriteFile(current_dirr, filename1);
        }
        else if (strcmp(cmd, "mkdir") == 0) // 创建目录
        {
            CreateDir(current_dirr, filename1);
        }
        else if (strcmp(cmd, "cd") == 0)
        {
            GoDir(current_dirr, filename1);
        }
        else if (strcmp(cmd, "help") == 0)
        {
            help();
        }
        else if (strcmp(cmd, "color") == 0)
        {
            colormode = (colormode + 1) % (PEN_DARK_GRAY + 1);
            clear();
        }
        else if (strcmp(cmd, "hcolor") == 0)
        {
            headercolormode = (headercolormode + 1) % (PEN_DARK_GRAY + 1);
            clear();
        }
        else if (strcmp(cmd, "runttt") == 0)
        {
            TTT(fd_stdin, fd_stdout);
            clear();
        }
        else if (strcmp(cmd, "MS") == 0)
        {
            startMineSweeper(fd_stdin, fd_stdout);
            clear();
        }
        else if (strcmp(cmd, "2048") == 0)
        {
            start2048Game(fd_stdin, fd_stdout);
            clear();
        }
        else if (strcmp(cmd, "runPushBox") == 0)
        {
            runPushBox(fd_stdin, fd_stdout);
            clear();
        }
        else if (strcmp(cmd, "clock") == 0)
        {
            clock(fd_stdin, fd_stdout);
            clear();
        }
        else if (strcmp(cmd, "runWZQ") == 0)
        {
            runWZQ(fd_stdin, fd_stdout);
            clear();
        }
        else if (strcmp(cmd, "runSnake") == 0)
        {
            runSnake(fd_stdin, fd_stdout);
            clear();
        }
        else if (strcmp(cmd, "clear") == 0)
        {
            clear();
        }
        else if (strcmp(cmd, "getpid") == 0)
        {
            printf(strcat((char *)getpid(), "\n"));
            printi(getpid());
            printf("\n");
            printi(new_getpid());
            printf("\n");
        }
        else if (strcmp(cmd, "test_fork") == 0)
        {
            int pid = fork();
            if (pid == 0)
            {
            }
            else
            {
                printl("childpid: %d, childname: %s\n", pid, proc_table[pid].name);
            }
        }
        else if (!strcmp(cmd, "cal"))
        {
            if (strlen(rdbuf) > 4)
            {
                calMain(rdbuf + 4);
            }
            else
            {
                char *str = "NULL";
                calMain(str);
            }
        }
        else if (!strcmp(cmd, "math"))
        {
            if (strlen(rdbuf) > 5)
            {
                mathMain(rdbuf + 5);
            }
            else
            {
                char *str = "NULL";
                mathMain(str);
            }
        }
        else if (strcmp(cmd, "flappybird") == 0)
        {
            startflappyBird(fd_stdin, fd_stdout);
        }
        else if (strcmp(cmd, "showlogo") == 0)
        {
            // _showImage("logo");
        }

        else if (strcmp(cmd, "test_ldt") == 0)
        {
            printl("{MM} base_high: %d, bsae_mid: %d, low: %d)\n", proc_table[0].ldts[INDEX_LDT_C].base_high, proc_table[0].ldts[INDEX_LDT_C].base_mid, proc_table[0].ldts[INDEX_LDT_C].base_low);
            printl("{MM} base_high: %d, bsae_mid: %d, low: %d)\n", proc_table[1].ldts[INDEX_LDT_C].base_high, proc_table[1].ldts[INDEX_LDT_C].base_mid, proc_table[1].ldts[INDEX_LDT_C].base_low);
            printl("{MM} base_high: %d, bsae_mid: %d, low: %d)\n", proc_table[2].ldts[INDEX_LDT_C].base_high, proc_table[2].ldts[INDEX_LDT_C].base_mid, proc_table[2].ldts[INDEX_LDT_C].base_low);
            printl("{MM} base_high: %d, bsae_mid: %d, low: %d)\n", proc_table[3].ldts[INDEX_LDT_C].base_high, proc_table[3].ldts[INDEX_LDT_C].base_mid, proc_table[3].ldts[INDEX_LDT_C].base_low);
            printl("{MM} base_high: %d, bsae_mid: %d, low: %d)\n", proc_table[4].ldts[INDEX_LDT_C].base_high, proc_table[4].ldts[INDEX_LDT_C].base_mid, proc_table[4].ldts[INDEX_LDT_C].base_low);
            printl("{MM} base_high: %d, bsae_mid: %d, low: %d)\n", proc_table[5].ldts[INDEX_LDT_C].base_high, proc_table[5].ldts[INDEX_LDT_C].base_mid, proc_table[5].ldts[INDEX_LDT_C].base_low);
            int k_base, k_limit;
            get_kernel_map(&k_base, &k_limit);
            printl("0x%x, 0x%x\n", k_base, k_limit);
            // struct proc* p_proc= proc_table+5;
            // printl("{MM} %d, %d, %d, %d)\n", p_proc->ldts[INDEX_LDT_C], p_proc->ldts[INDEX_LDT_RW], p_proc->ldts[INDEX_LDT_C].attr1, p_proc->ldts[INDEX_LDT_RW].attr1);
            // struct descriptor* ppd = &(proc_table[5].ldts[INDEX_LDT_RW]);
            // printl("{MM} %d,%d,%d,%d,%d,%d)\n", ppd->limit_low, ppd->base_low,  ppd->base_high, ppd->base_mid, ppd->attr1, ppd->limit_high_attr2, ppd->base_high);
            int i;
            for (i = 0; i < NR_TASKS + NR_NATIVE_PROCS; ++i)
            {
                struct descriptor *ppd = &(proc_table[i].ldts[INDEX_LDT_RW]);
                printl("{MM} %s, %x,%x,%x,%x,%x,%x)\n", proc_table[i].name, ppd->limit_low, ppd->base_low, ppd->base_mid, ppd->base_high, ppd->attr1, ppd->limit_high_attr2);
            }
        }
        else if (strcmp(cmd, "saveImage") == 0)
        {
            saveImage(current_dirr, namess, www, hhh);
        }
        else if (strcmp(cmd, "showImage") == 0)
        {
            _showImage(current_dirr, filename1);
        }

        else
            printf("Command not found, please check!\n");
        // printf("rdbuf:      %s\n", rdbuf);
        // printf("cmd:        %s\n", cmd);
        // printf("filename1:  %s\n", filename1);
        // printf("filename2:  %s\n", filename2);
    }
}

/*======================================================================*
                               TestB
 *======================================================================*/
//二号终端
void TestB()
{
    for (;;)
        ;
}

void TestC()
{
    for (;;)
        ;
}

/*****************************************************************************
 *                                panic
 *****************************************************************************/
PUBLIC void panic(const char *fmt, ...)
{
    int i;
    char buf[256];

    /* 4 is the size of fmt in the stack */
    va_list arg = (va_list)((char *)&fmt + 4);

    i = vsprintf(buf, fmt, arg);

    printl("%c !!panic!! %s", MAG_CH_PANIC, buf);

    /* should never arrive here */
    __asm__ __volatile__("ud2");
}

void clear()
{
    fillRect(0, headerHeight * 16, scr_x, scr_y + 40, colormode);
    fillRect(0, 0, scr_x, headerHeight * 16, headercolormode);
    drawText(20, 16, "Bird-OS", PEN_WHITE);
    drawText(800 - 32 * 8 - 20, 16, "Operating System Course Project", PEN_WHITE);
    console_table[current_console].cursor = headerHeight * SCR_WIDTH;
}

//void show()
//{
//  printf("%d  %d  %d  %d",console_table[current_console].con_size, console_table[current_console].crtc_start, console_table[current_console].cursor, console_table[current_console].orig);
//}

void help()
{
    printf("=========================================BirdOS help info===========================================\n");
    printf("Command List                       :\n");
    printf("1. process                         : A process manage,show you all process-info here\n");
    printf("2. filemng                         : Run the file manager\n");
    printf("3. clear                           : Clear the screen\n");
    printf("4. help                            : Show this help message\n");
    printf("5. ls                              : List all files in current directory\n");
    printf("6. touch     [filename]            : Create a new file in current directory\n");
    printf("7. rm        [filename]            : Delete a file in current directory\n");
    printf("8. cat       [filename]            : Print the content of a file in current directory\n");
    printf("9. vi        [filename]            : Write new content at the end of the file\n");
    printf("10. mkdir    [dirname]             : Create a new directory in current directory\n");
    printf("11. cd       [dirname]             : Go to a directory in current directory\n");
    printf("12. runttt                         : Run a small game on this OS\n");
    printf("13. 2048                           : Run 2048 game on this OS\n");
    printf("14. MS                             : Run MineSweeper game on this OS\n");
    printf("15. runSnake                       : Run Greedysnake game on this OS\n");
    printf("16. runWZQ                         : Run a chess game on this OS\n");
    printf("17. clock                          : Run a clock on this OS\n");
    printf("18. runPushBox                     : Run PushBox game on this OS\n");
    printf("====================================================================================================\n");
}

void ProcessManage()
{
    int i;
    printf("=============================================================================\n");
    printf("      PID      |     name      |    priority    |    running?\n");
    //进程号，进程名，优先级，是否是系统进程，是否在运行
    printf("-----------------------------------------------------------------------------\n");
    for (i = 0; i < NR_TASKS + NR_PROCS; ++i) //逐个遍历
    {
        /*if ( proc_table[i].priority == 0) continue;//系统资源跳过*/
        if (proc_table[i].p_flags != FREE_SLOT)
            printf("      %3d           %6s             %2d               %3s\n", proc_table[i].pid, proc_table[i].name, proc_table[i].priority, proc_table[i].p_flags == FREE_SLOT ? "NO" : "YES");
    }
    printf("=============================================================================\n");
}

/*****************************************************************************
 *                                计算器
 *****************************************************************************/

typedef int bool;
typedef int DATA;
#define False 0
#define True 1
#define EMPTY_CH '\0'
#define EMPTY_NUM -999999
#define SIZE 50

/*============ 判断表达式括号是否匹配 ============*/
char bucket_stack[100] = "\0";
int bucket_stack_index = -1;

bool isempty_bucket_stack()
{
    return bucket_stack_index == -1;
}

void bucket_stack_push(char ch)
{
    bucket_stack_index++;
    bucket_stack[bucket_stack_index] = ch;
}

char bucket_stack_pop(void)
{
    if (isempty_bucket_stack())
    {
        return EMPTY_CH;
    }
    char ch = bucket_stack[bucket_stack_index];
    bucket_stack_index--;
    return ch;
}

void bucket_stack_clear(void)
{
    memset(bucket_stack, '\0', sizeof(bucket_stack));
    bucket_stack_index = -1;
}

/*============ 操作数栈 ============*/
int num_stack[100] = {0};
int num_stack_index = -1;

bool isempty_num_stack()
{
    return num_stack_index == -1;
}

void num_stack_push(int num)
{
    num_stack_index++;
    num_stack[num_stack_index] = num;
}

int num_stack_pop(void)
{
    if (isempty_num_stack())
    {
        return EMPTY_NUM;
    }
    int num = num_stack[num_stack_index];
    num_stack[num_stack_index] = 0;
    num_stack_index--;
    return num;
}

void num_stack_clear(void)
{
    memset(num_stack, 0, sizeof(num_stack));
    num_stack_index = -1;
}

/*============ 操作符栈 ============*/
char op_stack[100] = "\0";
int op_stack_index = -1;

bool isempty_op_stack()
{
    return op_stack_index == -1;
}

void op_stack_push(char ch)
{
    op_stack_index++;
    op_stack[op_stack_index] = ch;
}

char op_stack_pop(void)
{
    if (isempty_op_stack())
    {
        return EMPTY_CH;
    }
    char ch = op_stack[op_stack_index];
    op_stack[op_stack_index] = '\0';
    op_stack_index--;
    return ch;
}

char op_stack_top(void)
{
    if (isempty_op_stack())
    {
        return EMPTY_CH;
    }
    return op_stack[op_stack_index];
}

void op_stack_clear(void)
{
    memset(op_stack, '\0', sizeof(op_stack));
    op_stack_index = -1;
}

/*============ 优先级 ============*/
int isp(char ch)
//栈内优先级
{
    switch (ch)
    {
    case '#':
        return 0;
    case '(':
        return 1;
    case '*':
    case '/':
        return 5;
    case '+':
    case '-':
        return 3;
    case ')':
        return 6;
    }
}
int icp(char ch)
//栈外优先级
{
    switch (ch)
    {
    case '#':
        return 0;
    case '(':
        return 6;
    case '*':
    case '/':
        return 4;
    case '+':
    case '-':
        return 2;
    case ')':
        return 1;
    }
}

/*============ 运算符判断 ============*/
bool isOperator(char c)
{
    switch (c)
    {
    case '+':
    case '-':
    case '*':
    case '/':
    case '(':
    case ')':
    case '#':
        return True;
        break;
    default:
        return False;
        break;
    }
}

/*============ 单个数字字符判断 ============*/
bool isDigit(char ch)
{
    return (ch >= '0' && ch <= '9');
}

/*============ 数字判断 ============*/
bool isNum(char *exp)
//数字包括 正数 负数 浮点数 (其中+3要进行特殊处理)
{
    char ch = exp[0];
    if (ch == '+' && strlen(exp) > 1)
    // 如 +3 就是储存 3
    {
        exp = exp + 1; //把+删除
        ch = exp[0];   //更新一下ch
    }

    if (isDigit(ch) || (ch == '-' && strlen(exp) > 1))
    //储存各种数字, 包括正数,负数,浮点数
    {
        return True;
    }

    return False;
}

/*============ 在表达式最后添加 # 标识符 ============*/
void addTail(char *exp)
{
    int i;
    for (i = 0; i < strlen(exp); ++i)
        ;
    exp[i] = ' ';
    exp[i + 1] = '#';
}

/*============ 封装表达式中的项 ============*/
struct Data
{
    int data; //数据本身
    int flag; //0->char, 1->int
};
int _current = 0;

/*============ 获取表达式中的下一项 ============*/
struct Data NextContent(char *exp)
{
    char _next[100] = "\0";
    char ch;
    int index = 0;

    for (int i = _current; i < strlen(exp); ++i)
    {
        ch = exp[i];
        if (ch != ' ')
        {
            _next[index] = ch;
            index++; //因为不同对象以空格隔开,所以只要不是空格就加到_next
        }
        else
        {
            while (exp[i] == ' ')
            {
                i++;
            }
            _current = i; //_current指向下一个位置,结束当前对象的寻找
            break;
        }
    }

    if (isOperator(_next[0]))
    {
        struct Data d;
        d.data = _next[0];
        d.flag = 0;
        return d;
    }
    else
    {
        struct Data d;
        atoi(_next, &d.data);
        d.flag = 1;
        return d;
    }
}

/*============ 根据运算符和两个操作数计算值 ============*/
int Cal(int left, char op, int right)
{
    switch (op)
    {
    case '+':
        return left + right;
        break;
    case '-':
        return left - right;
        break;
    case '*':
        return left * right;
        break;
    case '/':
        return left / right;
        break;
    default:
        return left + right;
        break;
    }
}

/*============ 输出后缀表达式 ============*/
void showBackMode(struct Data result[], int size)
{
    printf("The reverse polish notation is: ");
    for (int i = 0; i < size; ++i)
    {
        if (result[i].flag == 0)
        {
            printf("%c ", result[i].data);
        }
        else
        {
            printf("%d ", result[i].data);
        }
    }
    printf("\n");
}

/*============ 计算后缀表达式的结果 ============*/
int calResult(struct Data result[], int size)
{
    num_stack_clear();
    for (int i = 0; i < size; ++i)
    {
        if (result[i].flag == 1)
        {
            num_stack_push(result[i].data);
        }
        else
        {
            int right = num_stack_pop();
            int left = num_stack_pop();
            num_stack_push(Cal(left, result[i].data, right));
        }
    }
    return num_stack_pop();
}

void mathMain(char *expression)
{
    if (!strcmp(expression, "NULL"))
    {
        printf("Sorry, you should add a math expressioin.\n");
    }
    else
    {
        char str_list[2][10] = {"-beauty", "-rev"};
        int flag[2] = {1, 1};
        for (int i = 0; i < 2; ++i)
        {
            int j = 0;
            while (expression[j] != ' ' && expression[j] != '\0')
            {
                if (expression[j] != str_list[i][j])
                {
                    flag[i] = 0;
                    break;
                }
                ++j;
            }
        }

        if (flag[0])
        {
            if (strlen(expression) > 8)
            {
                char *value = expression + 8;

                if (!check_exp_notion(value))
                {
                    printf("Please check the expression and try again.\n");
                    printf("\n");
                    return;
                }

                bucket_stack_clear();
                if (!check_exp_bucket(value))
                {
                    printf("Please check the expression and try again.\n");
                    printf("\n");
                    return;
                }

                int result = calculate(value, False, True);
                if (result != EMPTY_NUM)
                {
                    printf("The result is %d\n", result);
                }
                else
                {
                    printf("You can input [help] to know more.\n");
                }
            }
            else
            {
                printf("You should add the expression you want to calculate.\n");
                printf("You can input [man cal] to know more.\n");
            }
        }
        else if (flag[1])
        {
            if (strlen(expression) > 5)
            {
                char *value = expression + 5;

                if (!check_exp_notion(value))
                {
                    printf("Please check the expression and try again.\n");
                    printf("\n");
                    return;
                }

                bucket_stack_clear();
                if (!check_exp_bucket(value))
                {
                    printf("Please check the expression and try again.\n");
                    printf("\n");
                    return;
                }

                int result = calculate(value, True, False);
                if (result != EMPTY_NUM)
                {
                    printf("The result is %d\n", result);
                }
                else
                {
                    printf("You can input [help] to know more.\n");
                }
            }
            else
            {
                printf("You should add the expression you want to calculate.\n");
                printf("You can input [man cal] to know more.\n");
            }
        }
        else
        {
            if (strlen(expression) > 0)
            {
                char *value = expression;

                if (!check_exp_notion(value))
                {
                    printf("Please check the expression and try again.\n");
                    printf("\n");
                    return;
                }

                bucket_stack_clear();
                if (!check_exp_bucket(value))
                {
                    printf("Please check the expression and try again.\n");
                    printf("\n");
                    return;
                }

                int result = calculate(value, False, False);
                if (result != EMPTY_NUM)
                {
                    printf("The result is %d\n", result);
                }
                else
                {
                    printf("You can input [help] to know more.\n");
                }
            }
            else
            {
                printf("You should add at least a expression.\n");
                printf("You can input [help] to know more.\n");
            }
        }
    }

    printf("\n");
}

/*============ 顶层计算函数 ============*/
int calculate(char *origin_exp, bool if_showrev, bool if_beauty)
{
    /*============ 表达式美化 ============*/
    char exp[100] = "\0";
    int pos = 0;
    for (int i = 0; i < strlen(origin_exp); ++i)
    {
        if (isOperator(origin_exp[i]))
        {
            exp[pos] = ' ';
            ++pos;
            exp[pos] = origin_exp[i];
            ++pos;
            exp[pos] = ' ';
            ++pos;
        }
        else if (isDigit(origin_exp[i]))
        {
            exp[pos] = origin_exp[i];
            ++pos;
        }
    }

    if (if_beauty)
    {
        printf("After beautify: %s\n", exp);
    }

    /*初始两个栈*/
    num_stack_clear();
    op_stack_clear();
    _current = 0;

    struct Data result[100];
    int index = 0;

    /*在表达式尾部添加结束标识符*/
    addTail(exp);

    op_stack_push('#');
    struct Data elem = NextContent(exp);
    while (!isempty_op_stack())
    {
        char ch = elem.data;

        if (elem.flag == 1)
        { //如果是操作数, 直接读入下一个内容
            result[index] = elem;
            index++;
            elem = NextContent(exp);
        }
        else if (elem.flag == 0)
        { //如果是操作符,判断ch的优先级icp和当前栈顶操作符的优先级isp
            char topch = op_stack_top();
            if (isp(topch) < icp(ch))
            { //当前操作符优先级大,将ch压栈,读入下一个内容
                op_stack_push(ch);
                elem = NextContent(exp);
            }
            else if (isp(topch) > icp(ch))
            { //当前优先级小,推展并输出到结果中
                struct Data buf;
                buf.data = op_stack_pop();
                buf.flag = 0;
                result[index] = buf;
                index++;
            }
            else
            {
                if (op_stack_top() == '(')
                { //如果退出的是左括号则读入下一个内容
                    elem = NextContent(exp);
                }
                op_stack_pop();
            }
        }
    }

    if (if_showrev)
    {
        showBackMode(result, index);
    }

    return calResult(result, index);
}

/*判断表达式括号是否匹配*/
bool check_exp_bucket(char *exp)
{
    char ch = '\0';

    for (int i = 0; i < strlen(exp); ++i)
    {
        if (exp[i] == '(')
        {
            bucket_stack_push('(');
        }
        else if (exp[i] == ')')
        {
            ch = bucket_stack_pop();
            if (ch == EMPTY_CH || ch != '(')
            {
                printf("Buckets in the exprssion you input do not match.\n");
                return False;
            }
        }
    }
    return isempty_bucket_stack();
}

/*判断表达式是否有非法符号*/
bool check_exp_notion(char *exp)
{
    for (int i = 0; i < strlen(exp); ++i)
    {
        if (isDigit(exp[i]) || isOperator(exp[i]) || exp[i] == ' ')
        {
            continue;
        }
        else
        {
            printf("The operator we support: [+-*/()], you have input %c.\n", exp[i]);
            return False;
        }
    }
    return True;
}

/*****************************************************************************
 *                                processManager
 *****************************************************************************/
//进程管理主函数
void runProcessManage(int fd_stdin)
{
    clear();
    char readbuffer[128];
    showProcessWelcome();
    while (1)
    {
        printf("  birdOS ~ process-manager: $ ");

        int end = read(fd_stdin, readbuffer, 70);
        readbuffer[end] = 0;
        int i = 0, j = 0;
        //获得命令指令
        char cmd[20] = {0};
        while (readbuffer[i] != ' ' && readbuffer[i] != 0)
        {
            cmd[i] = readbuffer[i];
            i++;
        }
        i++;
        //获取命令目标
        char target[20] = {0};
        while (readbuffer[i] != ' ' && readbuffer[i] != 0)
        {
            target[j] = readbuffer[i];
            i++;
            j++;
        }
        //结束进程;
        if (strcmp(cmd, "kill") == 0)
        {
            killProcess(target);
            continue;
        }
        //重启进程
        else if (strcmp(cmd, "restart") == 0)
        {
            restartProcess(target);
            continue;
        }
        //弹出提示
        else if (strcmp(readbuffer, "help") == 0)
        {
            clear();
            showProcessWelcome();
        }
        //打印全部进程
        else if (strcmp(readbuffer, "ps") == 0)
        {
            showProcess();
        }
        //退出进程管理
        else if (strcmp(readbuffer, "quit") == 0)
        {
            clear();
            break;
        }
        else if (!strcmp(readbuffer, "clear"))
        {
            clear();
        }
        //错误命令提示
        else
        {
            printf("Sorry, there no such command in the Process Manager.\n");
            printf("You can input [help] to know more.\n");
            printf("\n");
        }
    }
}

//打印所有进程
void showProcess()
{
    int i;
    printf("===============================================================================\n");
    printf("    ProcessID    *    ProcessName    *    ProcessPriority    *    Running?           \n");
    //进程号，进程名，优先级，是否在运行
    printf("-------------------------------------------------------------------------------\n");
    for (i = 0; i < NR_TASKS + NR_PROCS; i++) //逐个遍历
    {
        printf("        %d", proc_table[i].pid);
        printf("                 %5s", proc_table[i].name);
        printf("                   %2d", proc_table[i].priority);
        if (proc_table[i].priority == 0)
        {
            printf("                   no\n");
        }
        else
        {
            printf("                   yes\n");
        }
        //printf("        %d                 %s                   %d                   yes\n", proc_table[i].pid, proc_table[i].name, proc_table[i].priority);
    }
    printf("===============================================================================\n\n");
}

void calMain(char *option)
{
    /*判断附加的选项是何种命令*/
    char str_list[3][10] = {"-month", "-week", "-date"};
    int flag[3] = {1, 1, 1};
    for (int i = 0; i < 3; ++i)
    {
        int j = 0;
        while (option[j] != ' ' && option[j] != '\0')
        {
            if (option[j] != str_list[i][j])
            {
                flag[i] = 0;
                break;
            }
            ++j;
        }
    }

    //char value[20] = "\0";
    int year, month, day;
    char year_str[5] = "\0", month_str[3] = "\0", day_str[3] = "\0";

    if (!strcmp(option, "NULL"))
    {
        printf("Sorry, you should add an option.\n");
    }
    else if (flag[0])
    { //-month
        if (strlen(option) > 7)
        {
            char *value = option + 7;

            int i = 0;
            for (int j = 0; i < strlen(value) && value[i] != '/' && value[i] != ' '; ++i, ++j)
            {
                year_str[i] = value[i];
            }
            ++i;
            for (int j = 0; i < strlen(value) && value[i] != ' '; ++i, ++j)
            {
                month_str[j] = value[i];
            }

            atoi(year_str, &year);
            atoi(month_str, &month);

            if (year > 0 && month > 0 && month < 13)
            {
                display_month(year, month);
            }
            else
            {
                if (year < 0)
                {
                    printf("The [year] you input should greater than 0.\n");
                }
                if (month < 1 || month > 12)
                {
                    printf("The [month] you input should between 1 to 12.\n");
                }
                printf("Please input again.\n");
            }
        }
        else
        {
            printf("Sorry, you should add [Y/M].\n");
            printf("You can input [man cal] to know more.\n");
        }
    }
    else if (flag[1])
    { //-week
        if (strlen(option) > 6)
        {
            char *value = option + 6;

            int i = 0;
            for (int j = 0; i < strlen(value) && value[i] != '/' && value[i] != ' '; ++i, ++j)
            {
                year_str[i] = value[i];
            }
            ++i;
            for (int j = 0; i < strlen(value) && value[i] != '/' && value[i] != ' '; ++i, ++j)
            {
                month_str[j] = value[i];
            }
            ++i;
            for (int j = 0; i < strlen(value) && value[i] != ' '; ++i, ++j)
            {
                day_str[j] = value[i];
            }

            atoi(year_str, &year);
            atoi(month_str, &month);
            atoi(day_str, &day);

            if (year > 0 && month > 0 && month < 13 && day > 0 && day < 32)
            {
                if (Isleap(year) == 1 && month == 2 && day > 29)
                {
                    printf("%d is a leap year, the [day] you input should not greater than 29.\n", year);
                }
                else if (Isleap(year) == 0 && month == 2 && day > 28)
                {
                    printf("%d is a normal year, the [day] you input should not greater than 28.\n", year);
                }
                else
                {
                    display_week(year, month, day);
                }
            }
            else
            {
                if (year < 0)
                {
                    printf("The [year] you input should greater than 0.\n");
                }
                if (month < 1 || month > 12)
                {
                    printf("The [month] you input should between 1 to 12.\n");
                }
                if (day < 1 || day > 31)
                {
                    printf("The day you input should between 1 to 31.\n");
                }
                printf("Please input again.\n");
            }
        }
        else
        {
            printf("Sorry, you should add .\n");
            printf("You can input [man cal] to know more.\n");
        }
    }
    else if (flag[2])
    { //-date
        if (strlen(option) > 6)
        {
            char *value = option + 6;

            int i = 0;
            for (int j = 0; i < strlen(value) && value[i] != '/' && value[i] != ' '; ++i, ++j)
            {
                year_str[i] = value[i];
            }
            ++i;
            for (int j = 0; i < strlen(value) && value[i] != '/' && value[i] != ' '; ++i, ++j)
            {
                month_str[j] = value[i];
            }
            ++i;
            for (int j = 0; i < strlen(value) && value[i] != ' '; ++i, ++j)
            {
                day_str[j] = value[i];
            }

            atoi(year_str, &year);
            atoi(month_str, &month);
            atoi(day_str, &day);

            if (year > 0 && month > 0 && month < 13 && day > 0 && day < 32)
            {
                if (Isleap(year) == 1 && month == 2 && day > 29)
                {
                    printf("%d is a leap year, the [day] you input should not greater than 29.\n", year);
                }
                else if (Isleap(year) == 0 && month == 2 && day > 28)
                {
                    printf("%d is a normal year, the [day] you input should not greater than 28.\n", year);
                }
                else
                {
                    printf("\n%d/%d/%d is day No.%d of the year.\n", year, month, day, Total_day(year, month, day));
                }
            }
            else
            {
                if (year < 0)
                {
                    printf("The [year] you input should greater than 0.\n");
                }
                if (month < 1 || month > 12)
                {
                    printf("The [month] you input should between 1 to 12.\n");
                }
                if (day < 1 || day > 31)
                {
                    printf("The day you input should between 1 to 31.\n");
                }
                printf("Please input again.\n");
            }
        }
        else
        {
            printf("Sorry, you should add .\n");
            printf("You can input [man cal] to know more.\n");
        }
    }
    else
    {
        printf("Sorry, there no such option for cal.\n");
        printf("You can input [man cal] to know more.\n");
    }

    printf("\n");
}

/*****************************************************************************
 *                                calendar
 *****************************************************************************/
int Isleap(int year)
{
    if ((year % 400 == 0) || ((year % 4 == 0) && (year % 100 != 0)))
        return 1;
    else
        return 0;
}

/*判断输入年份二月份的天数
返回值:month的天数*/
int Max_day(int year, int month)
{
    int Day[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (Isleap(year) == 1)
        Day[1] = 29;
    return Day[month - 1];
}

/*计算输入的日期是这一年的多少天*/
int Total_day(int year, int month, int day)
{
    int sum = 0;
    int i = 1;
    for (i = 1; i < month; i++)
        sum = sum + Max_day(year, i);
    sum = sum + day;
    return sum;
}

/*由输入的日期判断当天是星期几
**返回值:count,0～6，分别表示星期日～星期六
*/
int Weekday(int year, int month, int day)
{
    int count;
    count = (year - 1) + (year - 1) / 4 - (year - 1) / 100 + (year - 1) / 400 + Total_day(year, month, day);
    count = count % 7;
    return count;
}

/*显示输入的日期是星期几*/
void display_week(int year, int month, int day)
{
    int count;
    count = Weekday(year, month, day);
    switch (count)
    {
    case 0:
        printf("\n%d-%d-%d is Sunday\n", year, month, day);
        break;
    case 1:
        printf("\n%d-%d-%d is Monday\n", year, month, day);
        break;
    case 2:
        printf("\n%d-%d-%d is Tuesday\n", year, month, day);
        break;
    case 3:
        printf("\n%d-%d-%d is Wednesday\n", year, month, day);
        break;
    case 4:
        printf("\n%d-%d-%d is Thursday\n", year, month, day);
        break;
    case 5:
        printf("\n%d-%d-%d is Friday\n", year, month, day);
        break;
    case 6:
        printf("\n%d-%d-%d is Saturday\n", year, month, day);
        break;
    }
}

/*显示输入的日期的当月日历*/
void display_month(int year, int month)
{
    int i = 0, j = 1;
    int week, max;
    week = Weekday(year, month, 1); //由每月1号确定打印制表符的个数
    max = Max_day(year, month);
    printf("\n                   %d/%d\n", year, month);
    printf("Sun    Mon    Tue    Wed    Thu    Fri    Sat\n");
    for (i = 0; i < week; i++)
        printf("       ");
    for (j = 1; j <= max; j++)
    {
        printf("%d     ", j);
        if (j < 10)
            printf(" ");
        if (i % 7 == 6)
            printf("\n");
        i++;
    }
    printf("\n");
}

int getMag(int n)
{
    int mag = 1;
    for (int i = 0; i < n; i++)
    {
        mag = mag * 10;
    }
    return mag;
}

//计算进程pid
int getPid(char str[])
{
    int length = 0;
    for (; length < MAX_FILENAME_LEN; length++)
    {
        if (str[length] == '\0')
        {
            break;
        }
    }
    int pid = 0;
    for (int i = 0; i < length; i++)
    {
        if (str[i] - '0' > -1 && str[i] - '9' < 1)
        {
            pid = pid + (str[i] + 1 - '1') * getMag(length - 1 - i);
        }
        else
        {
            pid = -1;
            break;
        }
    }
    return pid;
}

//结束进程
void killProcess(char str[])
{
    int pid = getPid(str);

    //健壮性处理以及结束进程
    if (pid >= NR_TASKS + NR_PROCS || pid < 0)
    {
        printf("The pid exceeded the range\n");
    }
    else if (pid < NR_TASKS)
    {
        printf("System tasks cannot be killed.\n");
    }
    else if (proc_table[pid].priority == 0 || proc_table[pid].p_flags == -1)
    {
        printf("Process not found.\n");
    }
    else if (pid == 4 || pid == 6)
    {
        printf("This process cannot be killed.\n");
    }
    else
    {
        proc_table[pid].priority = 0;
        proc_table[pid].p_flags = -1;
        printf("Aim process is killed.\n");
    }

    showProcess();
}

//重启进程
void restartProcess(char str[])
{
    int pid = getPid(str);

    if (pid >= NR_TASKS + NR_PROCS || pid < 0)
    {
        printf("The pid exceeded the range\n");
    }
    else if (proc_table[pid].p_flags != -1)
    {
        printf("This process is already running.\n");
    }
    else
    {
        proc_table[pid].priority = 1;
        proc_table[pid].p_flags = 1;
        printf("Aim process is running.\n");
    }

    showProcess();
}

//打印欢迎界面
void showProcessWelcome()
{
    printf("      ====================================================================\n");
    printf("      #                            Welcome to                            #\n");
    printf("      #                     birdOS ~ Process Manager                     #\n");
    printf("      #                                                                  #\n");
    printf("      #                                                                  #\n");
    printf("      #                                                                  #\n");
    printf("      #             [COMMAND]                 [FUNCTION]                 #\n");
    printf("      #                                                                  #\n");
    printf("      #           $ ps           |     show all process                  #\n");
    printf("      #           $ kill [id]    |     kill a process                    #\n");
    printf("      #           $ restart [id] |     restart a process                 #\n");
    printf("      #           $ quit         |     quit process management system    #\n");
    printf("      #           $ help         |     show command list of this system  #\n");
    printf("      #           $ clear        |     clear the cmd                     #\n");
    printf("      #                                                                  #\n");
    printf("      #                                                                  #\n");
    printf("      #                                                                  #\n");
    printf("      #                                                                  #\n");
    printf("      #                                                                  #\n");
    printf("      #                                                                  #\n");
    printf("      ====================================================================\n");

    printf("\n\n");
}

// //打印所有进程
// void showProcess()
// {
//     int i;
//     printf("===============================================================================\n");
//     printf("    ProcessID    *    ProcessName    *    ProcessPriority    *    Running?           \n");
//     //进程号，进程名，优先级，是否在运行
//     printf("-------------------------------------------------------------------------------\n");
//     for (i = 0; i < NR_TASKS + NR_PROCS; i++) //逐个遍历
//     {
//         printf("        %d", proc_table[i].pid);
//         printf("                 %5s", proc_table[i].name);
//         printf("                   %2d", proc_table[i].priority);
//         if (proc_table[i].priority == 0)
//         {
//             printf("                   no\n");
//         }
//         else
//         {
//             printf("                   yes\n");
//         }
//         //printf("        %d                 %s                   %d                   yes\n", proc_table[i].pid, proc_table[i].name, proc_table[i].priority);
//     }
//     printf("===============================================================================\n\n");
// }

void CreateFile(char *path, char *file)
{
    char absoPath[512];
    convert_to_absolute(absoPath, path, file);

    int fd = open(absoPath, O_CREAT | O_RDWR);

    if (fd == -1)
    {
        printf("Failed to create a new file with name %s\n", file);
        return;
    }

    char buf[1] = {0};
    write(fd, buf, 1);
    printf("File created: %s (fd %d)\n", file, fd);
    close(fd);
}

void DeleteFile(char *path, char *file)
{
    char absoPath[512];
    convert_to_absolute(absoPath, path, file);
    int m = unlink(absoPath);
    if (m == 0)
        printf("%s deleted!\n", file);
    else
        printf("Failed to delete %s!\n", file);
}

void ReadFile(char *path, char *file)
{
    char absoPath[512];
    convert_to_absolute(absoPath, path, file);
    int fd = open(absoPath, O_RDWR);
    if (fd == -1)
    {
        printf("Failed to open %s!\n", file);
        return;
    }

    char buf[4096];
    int n = read(fd, buf, 4096);
    if (n == -1) // 读取文件内容失败
    {
        printf("An error has occured in reading the file!\n");
        close(fd);
        return;
    }

    printf("%s\n", buf);
    close(fd);
}

void WriteFile(char *path, char *file)
{
    char absoPath[512];
    convert_to_absolute(absoPath, path, file);
    int fd = open(absoPath, O_RDWR);
    if (fd == -1)
    {
        printf("Failed to open %s!\n", file);
        return;
    }

    char tty_name[] = "/dev_tty0";
    int fd_stdin = open(tty_name, O_RDWR);
    if (fd_stdin == -1)
    {
        printf("An error has occured in writing the file!\n");
        return;
    }
    char writeBuf[4096]; // 写缓冲区
    int endPos = read(fd_stdin, writeBuf, 4096);
    writeBuf[endPos] = 0;
    write(fd, writeBuf, endPos + 1); // 结束符也应写入
    close(fd);
}

void CreateDir(char *path, char *file)
{
    char absoPath[512];
    convert_to_absolute(absoPath, path, file);
    int fd = open(absoPath, O_RDWR);

    if (fd != -1)
    {
        printf("Failed to create a new directory with name %s\n", file);
        return;
    }
    mkdir(absoPath);
}

void GoDir(char *path, char *file)
{
    int flag = 0; // 判断是进入下一级目录还是返回上一级目录
    char newPath[512] = {0};
    if (file[0] == '.' && file[1] == '.') // cd ..返回上一级目录
    {
        flag = 1;
        int pos_path = 0;
        int pos_new = 0;
        int i = 0;
        char temp[128] = {0}; // 用于存放某一级目录的名称
        while (path[pos_path] != 0)
        {
            if (path[pos_path] == '/')
            {
                pos_path++;
                if (path[pos_path] == 0) // 已到达结尾
                    break;
                else
                {
                    temp[i] = '/';
                    temp[i + 1] = 0;
                    i = 0;
                    while (temp[i] != 0)
                    {
                        newPath[pos_new] = temp[i];
                        temp[i] = 0; // 抹掉
                        pos_new++;
                        i++;
                    }
                    i = 0;
                }
            }
            else
            {
                temp[i] = path[pos_path];
                i++;
                pos_path++;
            }
        }
    }
    char absoPath[512];
    char temp[512];
    int pos = 0;
    while (file[pos] != 0)
    {
        temp[pos] = file[pos];
        pos++;
    }
    temp[pos] = '/';
    temp[pos + 1] = 0;
    if (flag == 1) // 返回上一级目录
    {
        temp[0] = 0;
        convert_to_absolute(absoPath, newPath, temp);
    }
    else // 进入下一级目录
        convert_to_absolute(absoPath, path, temp);
    int fd = open(absoPath, O_RDWR);
    if (fd == -1)
        printf("%s is not a directory!\n", absoPath);
    else
        memcpy(path, absoPath, 512);
}

void _showImage(char *path, char *filename)
{
    clear();
    char absoPath[512];
    convert_to_absolute(absoPath, path, filename);
    unsigned char size[3];
    int fd = open(absoPath, O_RDWR);
    assert(fd != -1);
    read(fd, size, 3);
    int w = size[0] + size[1];
    int h = size[2];
    int rd_bytes = w * h;
    unsigned char Img[rd_bytes];
    int n = read(fd, Img, rd_bytes);
    assert(n == rd_bytes);
    close(fd);
    unsigned char *p = vram;
    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            p[y * 320 + x] = Img[y * w + x];
        }
    }
    console_table[current_console].cursor = ((h / 16) + 1) * 8;
    waitkey();
}

void saveImage(char *path, char *filename, int w, int h)
{
    int fd, n, size;
    if (w > 320)
        w = 320;
    if (h > 200)
        h = 200;
    size = w * h + 3;
    CreateFile(path, filename);

    char absoPath[512];
    convert_to_absolute(absoPath, path, filename);
    fd = open(absoPath, O_RDWR);
    /*write*/
    unsigned char bufw[64000];
    bufw[0] = w % 255;
    bufw[1] = w - bufw[0];
    bufw[2] = h;
    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            bufw[y * w + x + 3] = bmp[y][x];
        }
    }
    bufw[size] = 0;
    n = write(fd, bufw, size + 1);
    assert(n == size + 1);
    printf("File writed. fd: %d\n", fd);
    close(fd);
}
