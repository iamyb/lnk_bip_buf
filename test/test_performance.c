#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <time.h>
#include <string.h>
#include "lnk_bip_buf_internal.h"

/* huge page */
/* ------------------------------------------------------------------------- */
#ifndef SHM_HUGETLB
#define SHM_HUGETLB 04000
#endif

#define LENGTH (256UL*1024*1024)

#define dprintf(x)  printf(x)

/* Only ia64 requires this */
#ifdef __ia64__
#define SHMAT_FLAGS (SHM_RND)
#else
#define ADDR (void *)(0x0UL)
#define SHMAT_FLAGS (0)
#endif

static int   shmid;
static char* shmaddr;
static void __attribute__((constructor))create_hugepage(void)
{
    if ((shmid = shmget(2, LENGTH,
            SHM_HUGETLB | IPC_CREAT | SHM_R | SHM_W)) < 0) {
        perror("shmget");
        exit(1);
    }

    shmaddr = shmat(shmid, NULL, SHMAT_FLAGS);
    if (shmaddr == (char *)-1) {
        perror("Shared memory attach failure\n");
        shmctl(shmid, IPC_RMID, NULL);
        exit(2);
    }
}

static void __attribute__((destructor))destroy_hugepage(void)
{
    if (shmdt((const void *)shmaddr) != 0) {
        perror("Detach failure");
        shmctl(shmid, IPC_RMID, NULL);
        exit(3);
    }

    shmctl(shmid, IPC_RMID, NULL);
}


/* A wrapper for glibc malloc to have same definition with lbb allocator*/
/* ------------------------------------------------------------------------- */
static void* glibc_malloc(void* hdl, int block_size)
{
    (void)hdl;
    return malloc(block_size);
}

static void glibc_free(void* hdl, void* ptr)
{
    (void)hdl;
    return free(ptr);
}

/* ------------------------------------------------------------------------- */
typedef void* (*f_alloc)(void*, int);
typedef void  (*f_free)(void*, void*);

typedef struct s_alloc
{
    char*   name;
    f_alloc alloc;
    f_free  free;
}s_alloc;

/* ------------------------------------------------------------------------- */
static long long diffts(struct timespec start, struct timespec end)
{
    struct timespec temp;
    if ((end.tv_nsec-start.tv_nsec)<0) {
        temp.tv_sec = end.tv_sec-start.tv_sec-1;
        temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
    } else {
        temp.tv_sec = end.tv_sec-start.tv_sec;
        temp.tv_nsec = end.tv_nsec-start.tv_nsec;
    }
    return temp.tv_sec*1000000000+temp.tv_nsec;
}

/* ------------------------------------------------------------------------- */
static void meas_perf(void* hdl, s_alloc* allocator, int size, int loop)
{
    long long total = 0;
    void* ptrs[128];
    int j = 0;

    for(; j < loop; j++)
    {
        struct timespec start, end;
        int m;

        clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);
        for(m = 0; m < sizeof(ptrs)/sizeof(void*); m++)
            ptrs[m] = allocator->alloc(hdl, size);
        for(m = 0; m < sizeof(ptrs)/sizeof(void*); m++)
            memset(ptrs[m], 0xFF, size);
        for(m = 0; m < sizeof(ptrs)/sizeof(void*); m++)
            allocator->free(hdl, ptrs[m]);
        
        clock_gettime(CLOCK_THREAD_CPUTIME_ID, &end);
        
        total += diffts(start, end);
    }
    total = total / (loop*sizeof(ptrs)/sizeof(void*));
//    total = total / (size/32);
    printf("nm %6s, sz %6d, avg: %lld\n", allocator->name, size, total);
}

/* ------------------------------------------------------------------------- */
#define COUNT 10000
int main(void)
{
    int i;

    /* 8MB from hugepage */
    lbb_handle* hdl= lbb_create_to_ext_buf(1024*1024*8, shmaddr);

    /* allocators for test */
    s_alloc lbb =   {"lbb",   lbb_alloc,     lbb_free    };
    s_alloc glibc = {"glibc", glibc_malloc,  glibc_free  };

    /* 32bytes, 64bytes,..., 4096bytes */
    for(i = 5; i < 15; i++)
    {
        meas_perf(hdl,  &lbb,  1<<(i), COUNT);
        meas_perf(NULL, &glibc,1<<(i), COUNT);
    }

    /* destroy the lbb */
    lbb_destroy(hdl);

    return 0;
}

