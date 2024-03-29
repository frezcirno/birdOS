/*************************************************************************/ /**
 *****************************************************************************
 * @file   stdio.h
 * @brief  
 * @author Forrest Y. Yu
 * @date   2008
 *****************************************************************************
 *****************************************************************************/

/* the assert macro */
#define ASSERT
#ifdef ASSERT
void assertion_failure(char *exp, char *file, char *base_file, int line);
#define assert(exp) \
        if (exp)    \
                ;   \
        else        \
                assertion_failure(#exp, __FILE__, __BASE_FILE__, __LINE__)
#else
#define assert(exp)
#endif

/* EXTERN */
#define EXTERN extern /* EXTERN is defined as extern except in global.c */

/* string */
#define STR_DEFAULT_LEN 1024

#define O_CREAT 1
#define O_RDWR 2

#define SEEK_SET 1
#define SEEK_CUR 2
#define SEEK_END 3

#define MAX_PATH 128

/*--------*/
/* 库函数 */
/*--------*/

#ifdef ENABLE_DISK_LOG
#define SYSLOG syslog
#endif

/* lib/open.c */
PUBLIC int open(const char *pathname, int flags);

/* lib/close.c */
PUBLIC int close(int fd);

/* lib/read.c */
PUBLIC int read(int fd, void *buf, int count);

/* lib/write.c */
PUBLIC int write(int fd, const void *buf, int count);

/* lib/unlink.c */
PUBLIC int unlink(const char *pathname);

/* lib/getpid.c */
PUBLIC int getpid();

/* lib/syslog.c */
PUBLIC int syslog(const char *fmt, ...);

/* lib/fork.c */
PUBLIC int fork();
/* lib/wait.c */
PUBLIC u32 wait(int *status);
/* lib/sleep.c */
PUBLIC u32 sleep(int seconds);
/* lib/exit.c */
PUBLIC int exit(void);

/* lib/sl.c     */
PUBLIC void sl();

/* game/TTT.c     */
PUBLIC void TTT();

// /* game/2048Game.c     */
//  PUBLIC void     start2048Game              (int fd_stdin, int fd_stdout);

// lib/rand.c
#define RAND_MAX 32768

PUBLIC int rand(void);
PUBLIC void srand(unsigned int seed);
