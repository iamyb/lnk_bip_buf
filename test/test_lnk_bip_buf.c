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

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "lnk_bip_buf_internal.h"

#if defined (LBB_TEST)
/* Test Codes below ----------------------------------------- */
/* ---------------------------------------------------------- */
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
#if 1
#define LBB_ASSERT(lbb,a,b,c) do{                               \
    if(a!=b)                                               \
    {                                                      \
        int last = lbb_get_tail(lbb);\
        int cur = lbb_get_head(lbb);\
        int free = lbb_get_free_space(lbb);\
        int used = lbb_get_used_space(lbb);\
        int tailor = lbb_get_tailor_words(lbb);\
        printf("-----------------------------------\n");   \
        printf("error, %d, %d, line: %d \n",a,b,c);        \
        printf("-----------------------------------\n");   \
        printf("lbb->last: %d lbb->cur: %d \n", last, cur); \
        printf("lbb->free: %d lbb->used: %d \n", free, used);\
        printf("lbb->tailor: %d \n", tailor);                       \
        printf("-----------------------------------\n");   \
        exit(-1);                                          \
    }                                                      \
}while(0);
#endif 
#define LBB_BUF_SZ 1024

#if 1
static void test_case_1()
{    
    lbb_header* lbb = lbb_create(LBB_BUF_SZ);

    /* step1: alloc one block */
    u32   sz0 = 100;
    void* p1 = lbb_alloc(lbb, sz0);
    u32   left = LBB_B2W_ALIGN(LBB_BUF_SZ)-LBB_BLK_SIZE(sz0);
    
    LBB_ASSERT(lbb,lbb_get_free_space(lbb), left, __LINE__); 
    LBB_ASSERT(lbb,lbb_get_head(lbb), LBB_BLK_SIZE(sz0),__LINE__);
    LBB_ASSERT(lbb,lbb_get_used_space(lbb), LBB_BLK_SIZE(sz0),__LINE__);

    /* step2: free one block */
    lbb_free(lbb, p1);
    left = left + LBB_BLK_SIZE(sz0);
    LBB_ASSERT(lbb,lbb_get_free_space(lbb), left, __LINE__); 

    lbb_destroy(lbb);
    printf("Test Case 1 passed\n");
}

#endif 
#if 1
/* ---------------------------------------------------------- */
static void test_case_2()
{
    lbb_header* lbb = lbb_create(LBB_BUF_SZ); // 256 WORD

    void* arrp[100];
    void* arrp1[100];

    int i = 0, count = 0;

    memset(arrp, 0, sizeof(arrp));
    memset(arrp1, 0, sizeof(arrp));

    /* alloc until no free memory */
    int sz0 = 20;
    for(; i < 50; i++) 
    {
        arrp[i] = lbb_alloc(lbb, sz0); // 20/4 + 2 = 7
        if(arrp[i] == NULL) break;
    }
    int left = LBB_B2W_ALIGN(LBB_BUF_SZ)-i*LBB_BLK_SIZE(sz0);
    int free = lbb_get_free_space(lbb);
    LBB_ASSERT(lbb,free, left, __LINE__);

    /* free 10 blocks */
    for(i = 0; i < 10; i++)
    {
        lbb_free(lbb, arrp[i]);
        arrp[i] = NULL;
    }
    left += i*LBB_BLK_SIZE(sz0);
    free = lbb_get_free_space(lbb);
    LBB_ASSERT(lbb,free, left, __LINE__);

    /* alloc again */
    for(i=0; i < 50; i++) 
    {
        arrp1[i] = lbb_alloc(lbb, sz0); // 20/4 + 2 = 7
        if(arrp1[i] == NULL) break;
    }
    left -= i*LBB_BLK_SIZE(sz0);
    free = lbb_get_free_space(lbb);
    LBB_ASSERT(lbb,free, left, __LINE__);

    /* free the first part */
    for(i=0; i<50; i++)
    {
        if(arrp[i] != NULL)
        {
            lbb_free(lbb, arrp[i]);
            count++;
        }
    }
    left += count*LBB_BLK_SIZE(sz0);
    free = lbb_get_free_space(lbb);
    LBB_ASSERT(lbb,free, left, __LINE__);
    
    /* free the second part */
    for(i=0, count=0; i<50; i++)
    {
        if(arrp1[i] != 0)
        {
            lbb_free(lbb, arrp1[i]);
            count++;
        }
    }
    left += count*LBB_BLK_SIZE(sz0);
    free = lbb_get_free_space(lbb);
    LBB_ASSERT(lbb,free, left, __LINE__);

    lbb_destroy(lbb);
    printf("Test Case 2 passed\n");
}

/* ---------------------------------------------------------- */
static void test_case_3()
{
    #define FREE_SAFE 1024
    void* ap[100];
    int i;
    int alloced = 0;
    lbb_header* lbb = lbb_create(FREE_SAFE); // 256 WORD

    int sz[] = {51, 33, 102, 97};
    for(i=0; i < sizeof(sz)/sizeof(int); i++)
    {
        alloced += ((sz[i]+3)/sizeof(u32)+LBB_BH_WORDS);
        ap[i] = lbb_alloc(lbb, sz[i]);
    }

    int left = ((FREE_SAFE+3)/sizeof(u32)) - alloced;
    int free = lbb_get_free_space(lbb);
    LBB_ASSERT(lbb,free, left, __LINE__);

    /* free the second block first */
    lbb_free(lbb, ap[1]);
    free = lbb_get_free_space(lbb);
    LBB_ASSERT(lbb,free, left, __LINE__);

    /* free the first block */
    lbb_free(lbb, ap[0]);
    left = left + (sz[0]+3)/sizeof(u32) + LBB_BH_WORDS;
    left = left + (sz[1]+3)/sizeof(u32) + LBB_BH_WORDS;
    free = lbb_get_free_space(lbb);
    LBB_ASSERT(lbb,free, left, __LINE__);

    lbb_destroy(lbb);
    printf("Test Case 3 passed\n");
}

#include<time.h>
#include<stdlib.h>


/* ---------------------------------------------------------- */
void test_case_4()
{
    srand(time(NULL));
    int sz = rand()%(LBB_BUF_SZ-LBB_BH_WORDS*sizeof(u32));

    lbb_header* lbb = lbb_create(LBB_BUF_SZ); 
    LBB_ASSERT(lbb,lbb!=NULL, 1, __LINE__);

    void* p = lbb_alloc(lbb, sz);
    LBB_ASSERT(lbb,p!=NULL, 1, __LINE__);
    int left = LBB_B2W_ALIGN(LBB_BUF_SZ) - LBB_BLK_SIZE(sz);
    int free = lbb_get_free_space(lbb);
    int tailor = lbb_get_tailor_words(lbb);
    LBB_ASSERT(lbb,(free+tailor), left, __LINE__);

    lbb_free(lbb, p);
    left += LBB_BLK_SIZE(sz);
    free = lbb_get_free_space(lbb);
    LBB_ASSERT(lbb,free, left, __LINE__);
    
    lbb_destroy(lbb);
    printf("Test Case 4 passed\n");
}

/* ---------------------------------------------------------- */
static void test_case_5()
{
    int i = 0;
    lbb_header* lbb = lbb_create(LBB_BUF_SZ); 
    LBB_ASSERT(lbb,lbb!=NULL, 1, __LINE__);

    srand(time(NULL));
    for(i=0; i < 100000; i++)
    {
        int sz = (rand()%(LBB_BUF_SZ-LBB_BH_WORDS*sizeof(u32)-4))+1;

        void* p = lbb_alloc(lbb, sz);
        LBB_ASSERT(lbb,p!=NULL, 1, __LINE__);
        int left = LBB_B2W_ALIGN(LBB_BUF_SZ) - LBB_BLK_SIZE(sz);
        int free = lbb_get_free_space(lbb);
        int tailor = lbb_get_tailor_words(lbb);
        LBB_ASSERT(lbb,(free+tailor), left, __LINE__);
        
        lbb_free(lbb, p);
        left += LBB_BLK_SIZE(sz);
        free = lbb_get_free_space(lbb);
        LBB_ASSERT(lbb,free, left, __LINE__);
    }
    lbb_destroy(lbb);
    printf("Test Case 5 passed\n");
}

/* ---------------------------------------------------------- */
static void test_case_6()
{
    int i = 0, j=0;
    int left = LBB_B2W_ALIGN(LBB_BUF_SZ);
    lbb_header* lbb = lbb_create(LBB_BUF_SZ);
    LBB_ASSERT(lbb,lbb != NULL, 1, __LINE__);
    void* p[100];
    memset(p, 0, sizeof(p));

    srand(time(NULL));
    for(j=0; j < 100; j++)
    {
        left = LBB_B2W_ALIGN(LBB_BUF_SZ);
        for(i=0; i < sizeof(p)/sizeof(void*);i++)
        {
            int sz = (rand()%(LBB_BUF_SZ/16-LBB_BH_WORDS*sizeof(u32)-4))+1;
            p[i] = lbb_alloc(lbb, sz);
            if(p[i] == NULL) break;
            else
            {
                left = left - LBB_BLK_SIZE(sz);
                int tailor = lbb_get_tailor_words(lbb);
                int free = lbb_get_free_space(lbb);
                LBB_ASSERT(lbb,free+tailor, left, __LINE__);
            }
        }

        lbb_free(lbb, p[i-1]);
        lbb_blk_hd* bh =(lbb_blk_hd*)((u32*)p[i-1]-LBB_BH_WORDS);
        p[i-1] = NULL;

        for(i=0; i < sizeof(p)/sizeof(void*);i++)
        {
            lbb_free(lbb, p[i]);
            p[i] = NULL;
        }
        int free = lbb_get_free_space(lbb);
        int total = lbb_get_total_space(lbb);
        LBB_ASSERT(lbb,total, free, __LINE__);
    }

    lbb_destroy(lbb);
    printf("Test Case 6 passed\n");
    
}

static __inline__ unsigned long long rdtsc(void)
{
    unsigned long long int x;
    __asm__ volatile (".byte 0x0f, 0x31" : "=A" (x));
    return x;
}

static void test_case_7(void)
{
    int loop = 10000000;

    int* sz_p = (int*) malloc(loop*sizeof(int));
    int i = 0;
    lbb_header* lbb = lbb_create(LBB_BUF_SZ); 
    LBB_ASSERT(lbb,lbb!=NULL, 1, __LINE__);

    srand(time(NULL));
    for(i = 0; i < loop; i++)
    {
        sz_p[i] = (rand()%(LBB_BUF_SZ-LBB_BH_WORDS*sizeof(u32)-4))+1;
    }

    long long t0 = rdtsc();
    /* lbb_buffer */
#if 0
    for(i = 0; i < loop; i++)
    {
        void* p = lbb_alloc(lbb, sz_p[i]);
        lbb_free(lbb, p);
    }
#endif 
    long long t1 = rdtsc();
#if 1
    /* stdlib malloc */
    for(i = 0; i < loop; i++)
    {
        void* p = malloc(sz_p[i]);
        free(p);
    }
#endif 
    long long t2 = rdtsc();
    printf("t1-t0: %lld\n", t1-t0);
    printf("t2-t1: %lld\n", t2-t1);

    free(sz_p);
    lbb_destroy(lbb);
    printf("Test Case 7 passed\n");
}

static void test_all(void)
{
    test_case_1();
    test_case_2();
    test_case_3();
    test_case_4();
    test_case_5();
#if 1
    test_case_6();
#endif 
}
#endif
/* ---------------------------------------------------------- */
int main(int argc, char* argv[])
{
#if 1
    if(argc == 2)
    {
        // "./a.out 1" 
        // "./a.out 2"
        // "./a.out 3"
        int num = argv[1][0]-'0';

        switch(num)
        {
            case 1:
                test_case_1();
                break;
            case 2:
                test_case_2();
                break;
            case 3: 
                test_case_3(); 
                break;
            case 6: 
                test_case_6(); 
                break;
            case 7:
                test_case_7(); //performance measurement
                break;
            default:
                printf("invalid case number\n");
        }
        return 0;
    }
    test_all();
#endif
    return 0;
}
#endif
