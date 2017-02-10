/**
********************************************************************************
LBB: linked bip buffer with variant block size

Copyright (C) 2016 b20yang
---
This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option) any
later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along
with this program. If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/

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


#define START_SZ_IDX 5 
#define STOP_SZ_IDX 15

typedef struct s_test
{
	void*       hdl;
	s_alloc     allocator;
	int         size;
	int         loop;
	int         with_memset;
	long long   results[STOP_SZ_IDX-START_SZ_IDX];
}s_test;

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
static long long meas_perf(s_test* tc)
{
    long long total = 0;
    void* ptrs[128];
    int j = 0;

    for(; j < tc->loop; j++)
    {
        struct timespec start, end;
        int m;

        clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);
        for(m = 0; m < sizeof(ptrs)/sizeof(void*); m++)
            ptrs[m] = tc->allocator.alloc(tc->hdl, tc->size);

		if(tc->with_memset)
		{
        	for(m = 0; m < sizeof(ptrs)/sizeof(void*); m++)
            	memset(ptrs[m], 0xFF, tc->size);
		}
		
        for(m = 0; m < sizeof(ptrs)/sizeof(void*); m++)
            tc->allocator.free(tc->hdl, ptrs[m]);
        
        clock_gettime(CLOCK_THREAD_CPUTIME_ID, &end);
        
        total += diffts(start, end);
    }
    total = total / (tc->loop*sizeof(ptrs)/sizeof(void*));
//    total = total / (size/32);
//    printf("nm %6s, sz %6d, avg: %lld\n", tc->allocator.name, tc->size, total);
	return total;
}

/* ------------------------------------------------------------------------- */
#define COUNT 10000
#define RESULT_FILE "../doc/test_performance_results.txt"
#include<unistd.h>
void write_to_file(s_test *tc)
{
	FILE* f = fopen(RESULT_FILE, "a+");
	if(f != NULL)
	{
		int i;
		for(i = 0; i < sizeof(tc->results)/sizeof(long long)-1;i++)
		{
			fprintf(f, "%lld, ", tc->results[i]);
		}
		fprintf(f, "%lld\n", tc->results[i]);
		fclose(f);
	}
}

int main(void)
{
    int i;

    /* 8MB from hugepage */
    lbb_handle* hdl= lbb_create_to_ext_buf(1024*1024*8, shmaddr);
	if(access(RESULT_FILE, F_OK) == 0)
		unlink(RESULT_FILE);

    /* allocators for test */
	s_alloc glibc = {"glibc", glibc_malloc,  glibc_free};
	s_alloc lbb =   {"lbb",   lbb_alloc,     lbb_free};	

	s_test tc_glibc, tc_lbb;
	tc_lbb.hdl           = hdl;
	tc_lbb.allocator     = lbb;
	tc_lbb.loop          = COUNT;
	tc_lbb.with_memset   = 0;
	
	tc_glibc.hdl         = NULL;
	tc_glibc.allocator   = glibc;
	tc_glibc.loop        = COUNT;	
	tc_glibc.with_memset = 0;	

    /* 32bytes, 64bytes,..., 4096bytes */
    for(i = START_SZ_IDX; i < STOP_SZ_IDX; i++)
    {
		tc_glibc.size = tc_lbb.size = 1<<(i);
        tc_lbb.results[i-START_SZ_IDX]   = meas_perf(&tc_lbb);
        tc_glibc.results[i-START_SZ_IDX] = meas_perf(&tc_glibc);
	
    }
	write_to_file(&tc_lbb);	
	write_to_file(&tc_glibc);			

	s_test tc_glibc_ms = tc_glibc;
	s_test tc_lbb_ms   = tc_lbb;

    tc_lbb_ms.with_memset   = 1;
	tc_glibc_ms.with_memset = 1;
    /* 32bytes, 64bytes,..., 4096bytes */
    for(i = START_SZ_IDX; i < STOP_SZ_IDX; i++)
    {
		tc_glibc_ms.size = tc_lbb_ms.size = 1<<(i);
		
        tc_lbb_ms.results[i-START_SZ_IDX]   = meas_perf(&tc_lbb_ms);
		tc_glibc_ms.results[i-START_SZ_IDX] = meas_perf(&tc_glibc_ms);

    }	
	write_to_file(&tc_lbb_ms);	
	write_to_file(&tc_glibc_ms);			

    /* destroy the lbb */
    lbb_destroy(hdl);

    return 0;
}

