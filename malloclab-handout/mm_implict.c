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


#define WSIZE               4                       // �ֳ� 4���ֽ� / ͷ����Ų����ֽ���
#define DSIZE               8                       // ˫�ֳ� 8���ֽ�
#define CHUNKSIZE           (1<<12)                 // һ�ζ���չ�Ĵ�С

#define MAX(x,y)            ((x)>(y)?(x):(y))

#define PACK(size,alloc)    ((size) | (alloc))      // �����С�ͷ���λ��Ϊһ����

#define GET(p)              (*(unsigned int*)(p))   // ����ַp������   
#define PUT(p,val)          (*(unsigned int*)(p)=(val))

#define GET_SIZE(p)         (GET(p) & ~0x7)         // ����ַp���洢�Ĵ�С�ֶ�
#define GET_ALLOC(p)        (GET(p) & 0x1)          // ����ַp���洢�ķ����ֶ�

#define HDRP(bp)            ((char*)(bp)-WSIZE)     // �ɿ��ָ�����ͷ���ĵ�ַ
#define FTRP(bp)            ((char*)(bp)+GET_SIZE(HDRP(bp))-DSIZE)      // �ɿ��ָ�����β���ĵ�ַ

#define NEXT_BLKP(bp)       ((char*)(bp)+GET_SIZE(((char*)(bp)-WSIZE))) // �ɵ�ǰ��ĵ�ַ������һ����ĵ�ַ
#define PREV_BLKP(bp)       ((char*)(bp)-GET_SIZE(((char*)(bp)-DSIZE))) // �ɵ�ǰ��ĵ�ַ������һ����ĵ�ַ


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
    // �����յ�heap
    if((heap_listp=mem_sbrk(4*WSIZE))==(void*)-1)
        return -1;
    PUT(heap_listp,0);                      // �����
    PUT(heap_listp+(1*WSIZE),PACK(DSIZE,1));// ���Կ��ͷ��
    PUT(heap_listp+(2*WSIZE),PACK(DSIZE,1));// ���Կ��β��
    PUT(heap_listp+(3*WSIZE),PACK(0,1));    // ��β��
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
    // ��չ ����
    if(size<=DSIZE)
        asize=2*DSIZE;
    else
        asize=DSIZE*((size+(DSIZE)+(DSIZE-1))/DSIZE);
    // Ѱ��һ�����õĿ�
    if((bp=find_next_fit(asize))!=NULL)
    {
        place(bp,asize);
        return bp;
    }
    // û���ҵ�
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
        if(old_size>=new_size)           // ��Сԭblock
        {
            place(bp,new_size);
            return bp;
        }
        else                            // ����ԭblock
        {
            char* new_bp=mm_malloc(new_size);
            strncpy(new_bp,bp,old_size-2*WSIZE);
            mm_free(bp);
            return new_bp;
        }
    }
    
}

/**
 * �ϲ�
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
    else if(prev_alloc && !next_alloc)  // ���ϲ�
    {
        if(next_fit_pos==NEXT_BLKP(bp))
        {
            next_fit_pos=bp;
        }
        size+=GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(bp),PACK(size,0));
        PUT(FTRP(bp),PACK(size,0));
    }
    else if(!prev_alloc && next_alloc)  // ��ǰ�ϲ�
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
    else                                // ��ǰ�����ϲ�
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
 * ��heap��չwords����
*/
void* extend_heap(size_t words)  
{
    char* bp;
    size_t size;
    size=(words%2) ? (words+1)*WSIZE : words*WSIZE; // 8�ֽڶ���
    if((long)(bp=mem_sbrk(size))==-1)
        return NULL;
    PUT(HDRP(bp),PACK(size,0));
    PUT(FTRP(bp),PACK(size,0));
    PUT(HDRP(NEXT_BLKP(bp)),PACK(0,1));

    return coalesce(bp);
}

/**
 * Ѱ�ҿ��õĿ� �����״��������
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
 * ���������� 
 * ��ʣ�ಿ��>=��С��Ĵ�С ���зָ�
 * asize������������ͷ����β��
*/
void place(void* bp,size_t asize)
{
    size_t old_size=GET_SIZE(HDRP(bp));
    size_t new_size=old_size-asize;
    if(new_size>=2*DSIZE)                  // ��Ҫ���зָ�
    {
        PUT(HDRP(bp),PACK(asize,1));
        PUT(FTRP(bp),PACK(asize,1));
        char* new_bp=NEXT_BLKP(bp);
        PUT(HDRP(new_bp),PACK(new_size,0));
        PUT(FTRP(new_bp),PACK(new_size,0));
    }
    else                                    // ����Ҫ���зָ�
    {
        PUT(HDRP(bp),PACK(old_size,1));
        PUT(FTRP(bp),PACK(old_size,1));
    }
}


/**
 * ��������ڴ��ķ������
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



