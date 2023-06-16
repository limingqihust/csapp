/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 * 
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

team_t team = {
    /* Team name */
    "ateam",
    /* First member's full name */
    "Harry Bovik",
    /* First member's email address */
    "bovik@cs.cmu.edu",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""
};

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)


#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))


#define WSIZE               4                       // 字长 4个字节 / 头部或脚部的字节数
#define DSIZE               8                       // 双字长 8个字节
#define CHUNKSIZE           (1<<12)                 // 一次堆扩展的大小

#define MAX(x,y)            ((x)>(y)?(x):(y))

#define PACK(size,alloc)    ((size) | (alloc))      // 将块大小和分配位合为一个字

#define GET(p)              (*(unsigned int*)(p))   // 读地址p处的字   
#define PUT(p,val)          (*(unsigned int*)(p)=(val))

#define GET_SIZE(p)         (GET(p) & ~0x7)         // 读地址p处存储的大小字段
#define GET_ALLOC(p)        (GET(p) & 0x1)          // 读地址p处存储的分配字段

#define HDRP(bp)            ((char*)(bp)-WSIZE)     // 由块的指针计算头部的地址
#define FTRP(bp)            ((char*)(bp)+GET_SIZE(HDRP(bp))-DSIZE)      // 由块的指针计算尾部的地址

#define NEXT_BLKP(bp)       ((char*)(bp)+GET_SIZE(((char*)(bp)-WSIZE))) // 由当前块的地址计算下一个块的地址
#define PREV_BLKP(bp)       ((char*)(bp)-GET_SIZE(((char*)(bp)-DSIZE))) // 由当前块的地址计算上一个块的地址


static char* heap_listp;
static char* next_fit_pos;
static void* extend_heap(size_t words);
static void* coalesce(void* bp);
static void* find_first_fit(size_t asize);
static void* find_next_fit(size_t asize);
static void place(void* bp,size_t asize);
static void print_info();


int mm_init(void)
{
    // 创建空的heap
    if((heap_listp=mem_sbrk(4*WSIZE))==(void*)-1)
        return -1;
    PUT(heap_listp,0);                      // 填充字
    PUT(heap_listp+(1*WSIZE),PACK(DSIZE,1));// 序言块的头部
    PUT(heap_listp+(2*WSIZE),PACK(DSIZE,1));// 序言块的尾部
    PUT(heap_listp+(3*WSIZE),PACK(0,1));    // 结尾块
    heap_listp+=(2*WSIZE);
    
    next_fit_pos=heap_listp;
    if(extend_heap(CHUNKSIZE/WSIZE)==NULL)
        return -1;
    return 0;
}

void *mm_malloc(size_t size)
{
    size_t asize;
    size_t extendsize;
    char* bp;
    if(size==0)
        return NULL;
    // 扩展 对齐
    if(size<=DSIZE)
        asize=2*DSIZE;
    else
        asize=DSIZE*((size+(DSIZE)+(DSIZE-1))/DSIZE);
    // 寻找一个可用的块
    if((bp=find_next_fit(asize))!=NULL)
    {
        place(bp,asize);
        return bp;
    }
    // 没有找到
    extendsize=MAX(asize,CHUNKSIZE);
    if((bp=extend_heap(extendsize/WSIZE))==NULL)
        return NULL;
    place(bp,asize);
    return bp;

}

void mm_free(void *bp)
{
    size_t size=GET_SIZE(HDRP(bp));
    PUT(HDRP(bp),PACK(size,0));
    PUT(FTRP(bp),PACK(size,0));
    coalesce(bp);
}

void *mm_realloc(void *bp, size_t size)
{
    if(bp==NULL)
    {
        return mm_malloc(size);
    }
    else if(size==0)
    {
        mm_free(bp);
        return NULL;
    }
    else
    {
        size_t new_size;
        if(size<=DSIZE)
            new_size=2*DSIZE;
        else
            new_size=DSIZE*((size+(DSIZE)+(DSIZE-1))/DSIZE);
        size_t old_size=GET_SIZE(HDRP(bp));
        if(old_size>=new_size)           // 缩小原block
        {
            place(bp,new_size);
            return bp;
        }
        else                            // 扩大原block
        {
            char* new_bp=mm_malloc(new_size);
            strncpy(new_bp,bp,old_size-2*WSIZE);
            mm_free(bp);
            return new_bp;
        }
    }
    
}

/**
 * 合并
*/
void* coalesce(void* bp)
{
    size_t prev_alloc=GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc=GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size=GET_SIZE(HDRP(bp));
    if(prev_alloc && next_alloc)
    {
        return bp;
    }
    else if(prev_alloc && !next_alloc)  // 向后合并
    {
        if(next_fit_pos==NEXT_BLKP(bp))
        {
            next_fit_pos=bp;
        }
        size+=GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(bp),PACK(size,0));
        PUT(FTRP(bp),PACK(size,0));
    }
    else if(!prev_alloc && next_alloc)  // 向前合并
    {
        if(next_fit_pos==bp)
        {
            next_fit_pos=PREV_BLKP(bp);
        }
        size+=GET_SIZE(HDRP(PREV_BLKP(bp)));
        PUT(FTRP(bp),PACK(size,0));
        PUT(HDRP(PREV_BLKP(bp)),PACK(size,0));
        bp=PREV_BLKP(bp);
    }
    else                                // 向前和向后合并
    {
        if(next_fit_pos==bp || next_fit_pos==NEXT_BLKP(bp))
        {
            next_fit_pos=PREV_BLKP(bp);
        }
        size+=GET_SIZE(HDRP(PREV_BLKP(bp)))+GET_SIZE(FTRP(NEXT_BLKP(bp)));
        PUT(HDRP(PREV_BLKP(bp)),PACK(size,0));
        PUT(FTRP(NEXT_BLKP(bp)),PACK(size,0));
        bp=PREV_BLKP(bp);
    }
    return bp;
}

/**
 * 将heap扩展words个字
*/
void* extend_heap(size_t words)  
{
    char* bp;
    size_t size;
    size=(words%2) ? (words+1)*WSIZE : words*WSIZE; // 8字节对齐
    if((long)(bp=mem_sbrk(size))==-1)
        return NULL;
    PUT(HDRP(bp),PACK(size,0));
    PUT(FTRP(bp),PACK(size,0));
    PUT(HDRP(NEXT_BLKP(bp)),PACK(0,1));

    return coalesce(bp);
}

/**
 * 寻找可用的块 采用首次适配策略
*/
void* find_first_fit(size_t asize)
{
    for(char* bp=heap_listp;GET_SIZE(HDRP(bp))>0;bp=NEXT_BLKP(bp))
    {
        if(GET_SIZE(HDRP(bp))>=asize && !GET_ALLOC(HDRP(bp)))
        {   
            return bp;
        }
    }
    return NULL;
}

static void* find_next_fit(size_t asize)
{
    char* bp=next_fit_pos;
    do
    {
        if(GET_SIZE(HDRP(bp))>=asize && !GET_ALLOC(HDRP(bp)))
        {
            next_fit_pos=bp;
            return bp;
        }
        bp=NEXT_BLKP(bp);
        if(GET_SIZE(HDRP(bp))==0)
        {
            bp=heap_listp;
        }
    } while (bp!=next_fit_pos);
    
    return NULL;

}


/**
 * 将请求块放置 
 * 当剩余部分>=最小块的大小 进行分割
 * asize包括了请求块的头部和尾部
*/
void place(void* bp,size_t asize)
{
    size_t old_size=GET_SIZE(HDRP(bp));
    size_t new_size=old_size-asize;
    if(new_size>=2*DSIZE)                  // 需要进行分割
    {
        PUT(HDRP(bp),PACK(asize,1));
        PUT(FTRP(bp),PACK(asize,1));
        char* new_bp=NEXT_BLKP(bp);
        PUT(HDRP(new_bp),PACK(new_size,0));
        PUT(FTRP(new_bp),PACK(new_size,0));
    }
    else                                    // 不需要进行分割
    {
        PUT(HDRP(bp),PACK(old_size,1));
        PUT(FTRP(bp),PACK(old_size,1));
    }
}


/**
 * 输出所有内存块的分配情况
*/
void print_info()
{
    printf("\n");
    printf("start_addr\t");
    printf("block_size\t");
    printf("payload\t\t");
    printf("alloc\t");
    printf("header_addr\t");
    printf("header\t");
    printf("tail_addr\t");
    printf("tail\t");
    printf("\n");
    for(char* bp=heap_listp;GET_SIZE(HDRP(bp))>0;bp=NEXT_BLKP(bp))
    {
        printf("%p\t%x\t\t%x\t\t%x\t%p\t%x\t%p\t%x\n",bp,GET_SIZE(HDRP(bp)),GET_SIZE(HDRP(bp))-2*WSIZE,GET(HDRP(bp)) & 1,HDRP(bp),GET(HDRP(bp)),FTRP(bp),GET(FTRP(bp)));
    }
}



