#include "cachelab.h"
#include "stdio.h"
#include "getopt.h"
#include "stdlib.h"
#include "string.h"

typedef struct cache_line{
    int valid_bit;
    unsigned tag;
    int time_stamp;
}cache_line;

cache_line** cache;
FILE* fp;
int s;                          // number of set index bits,2^s is the number of sets
int E;                          // number of lines per set
int b;                          // number of block bits,2^b is the block size
int S;                          // number of cache sets
int B;                          // block size
char name_of_valgrind_trace[100];
int hit_cnt;
int miss_cnt;
int eviction_cnt;
int display_trace_info_flag;

void print_usage_info()
{
    printf("Usage: ./csim-ref [-hv] -s <num> -E <num> -b <num> -t <file>\n"
           "Options:\n"
           "  -h         Print this help message.\n"
           "  -v         Optional verbose flag.\n"
           "  -s <num>   Number of set index bits.\n"
           "  -E <num>   Number of lines per set.\n"
           "  -b <num>   Number of block offset bits.\n"
           "  -t <file>  Trace file.\n"
           "\n"
           "Examples:\n"
           "  linux>  ./csim-ref -s 4 -E 1 -b 4 -t traces/yi.trace\n"
           "  linux>  ./csim-ref -v -s 8 -E 2 -b 4 -t traces/yi.trace");
}

void init_cache()
{
    hit_cnt=0;
    miss_cnt=0;
    eviction_cnt=0;
    cache=(cache_line**)malloc(S*sizeof(cache_line*));
    for(int i=0;i<S;i++)
    {
        cache[i]=(cache_line*)malloc(E*sizeof(cache_line));
        for(int j=0;j<E;j++)
        {
            cache[i][j].valid_bit=0;
            cache[i][j].tag=0;
            cache[i][j].time_stamp=0;
        }
    }
}

void update_time_stamp()
{
    for(int i=0;i<S;i++)
    {
        for(int j=0;j<E;j++)
        {
            if(cache[i][j].valid_bit)
                cache[i][j].time_stamp++;
        }
    }
}

// return the index of available block index
int find_available_block(unsigned address)
{
    int max_time=-1;
    int find_index=0;
    unsigned set_index=(address>>b) & ((1<<s)-1);
    for(int i=0;i<E;i++)
    {
        if(!cache[set_index][i].valid_bit)
            return i;
        if(cache[set_index][i].time_stamp>max_time) {
            max_time = cache[set_index][i].time_stamp;
            find_index = i;
        }
    }
    return find_index;
}

// return the index of expected address
// return -1 stands for miss
int find_data(unsigned address)
{
    unsigned load_address=address;
    unsigned set_index=(load_address>>b) & ((1<<s)-1);
    unsigned tag=load_address>>(s+b);
    for(int j=0;j<E;j++)
    {
        if(cache[set_index][j].tag==tag && cache[set_index][j].valid_bit)            // hit
        {
            return j;
        }
    }
    return -1;
}


// load instruction
// 1.find
// 2.1 if hit,return
// 2.2 if miss,find available block
// 2.2.1 if find avaiable block,hit,return
// 2.2.2 if no available block,eviction,return
void load_data(unsigned address,int size)
{
    unsigned set_index=(address>>b) & ((1<<s)-1);
    unsigned tag=address>>(s+b);

    int find_index=find_data(address);
    if(find_index!=-1)                                      // hit
    {
        hit_cnt++;
        cache[set_index][find_index].time_stamp=0;
        if(display_trace_info_flag)
            printf("L %x,%d hit \n",address,size);
        return ;
    }
    miss_cnt++;                                             // donot hit
    int available_block_index= find_available_block(address);
    if(!cache[set_index][available_block_index].valid_bit)  // is free block
    {
        cache[set_index][available_block_index].valid_bit=1;
        cache[set_index][available_block_index].time_stamp=0;
        cache[set_index][available_block_index].tag=tag;
        if(display_trace_info_flag)
            printf("L %x,%d miss \n",address,size);
        return ;
    }
    else                                                    // evictim
    {
        eviction_cnt++;
        cache[set_index][available_block_index].valid_bit=1;
        cache[set_index][available_block_index].time_stamp=0;
        cache[set_index][available_block_index].tag=tag;
        if(display_trace_info_flag)
            printf("L %x,%d miss eviction \n",address,size);
        return ;
    }
}


// store instruction
void store_data(unsigned address,int size)
{
    unsigned set_index=(address>>b) & ((1<<s)-1);
    unsigned tag=address>>(s+b);

    int find_index=find_data(address);
    if(find_index!=-1)                                      // hit
    {
        hit_cnt++;
        cache[set_index][find_index].time_stamp=0;
        if(display_trace_info_flag)
            printf("S %x,%d hit\n",address,size);
        return ;
    }
    miss_cnt++;                                             // donot hit
    int available_block_index= find_available_block(address);
    if(!cache[set_index][available_block_index].valid_bit)  // is free block
    {
        cache[set_index][available_block_index].valid_bit=1;
        cache[set_index][available_block_index].time_stamp=0;
        cache[set_index][available_block_index].tag=tag;
        if(display_trace_info_flag)
            printf("S %x,%d miss\n",address,size);
        return ;
    }
    else                                                    // evictim
    {
        eviction_cnt++;
        cache[set_index][available_block_index].valid_bit=1;
        cache[set_index][available_block_index].time_stamp=0;
        cache[set_index][available_block_index].tag=tag;
        if(display_trace_info_flag)
            printf("S %x,%d miss eviction\n",address,size);
        return ;
    }
}


void modify_data(unsigned address,int size)
{
    if(display_trace_info_flag)
        printf("M %x,%d ",address,size);
    unsigned set_index=(address>>b) & ((1<<s)-1);
    unsigned tag=address>>(s+b);
    int find_index;

    // load
    find_index=find_data(address);
    if(find_index!=-1)                                              // hit
    {
        hit_cnt++;
        cache[set_index][find_index].time_stamp=0;
        if(display_trace_info_flag)
            printf("hit ");
    }
    else
    {
        miss_cnt++;                                                 // donot hit
        int available_block_index = find_available_block(address);
        if (!cache[set_index][available_block_index].valid_bit)     // is free block
        {
            cache[set_index][available_block_index].valid_bit = 1;
            cache[set_index][available_block_index].time_stamp = 0;
            cache[set_index][available_block_index].tag = tag;
            if (display_trace_info_flag)
                printf("miss ");
        }
        else                                                        // evictim
        {
            eviction_cnt++;
            cache[set_index][available_block_index].valid_bit = 1;
            cache[set_index][available_block_index].time_stamp = 0;
            cache[set_index][available_block_index].tag = tag;
            if (display_trace_info_flag)
                printf("miss eviction ");

        }
    }


    // strore
    find_index=find_data(address);
    if(find_index!=-1)                                              // hit
    {
        hit_cnt++;
        cache[set_index][find_index].time_stamp=0;
        if(display_trace_info_flag)
            printf("hit ");
    }
    else
    {
        miss_cnt++;                                                 // donot hit
        int available_block_index = find_available_block(address);
        if (!cache[set_index][available_block_index].valid_bit)     // is free block
        {
            cache[set_index][available_block_index].valid_bit = 1;
            cache[set_index][available_block_index].time_stamp = 0;
            cache[set_index][available_block_index].tag = tag;
            if (display_trace_info_flag)
                printf("miss");
        }
        else                                                        // evictim
        {
            eviction_cnt++;
            cache[set_index][available_block_index].valid_bit = 1;
            cache[set_index][available_block_index].time_stamp = 0;
            cache[set_index][available_block_index].tag = tag;
            if (display_trace_info_flag)
                printf("miss eviction");

        }
    }
    if(display_trace_info_flag)
        printf("\n");

}


void run()
{
    char identifier;
    unsigned address;
    int	size;
    while(fscanf(fp," %c %x,%d",&identifier,&address,&size)>0)
    {
        switch(identifier)
        {
            case 'L':           // data load
                load_data(address,size);
                break;
            case 'S':           // data store
                store_data(address,size);
                break;
            case 'M':           // data modify
                modify_data(address,size);
                break;
            default:
                break;
        }
        update_time_stamp();
    }
    fclose(fp);
}




int main(int argc,char** argv)
{
    int opt;
    while((opt=getopt(argc,argv,"hvs:E:b:t:"))!=-1)
    {
        switch(opt)
        {
            case 'h':
                print_usage_info();
                break;
            case 'v':
                display_trace_info_flag=1;
                break;
            case 's':
                s=atoi(optarg);
                break;
            case 'E':
                E=atoi(optarg);
                break;
            case 'b':
                b=atoi(optarg);
                break;
            case 't':
                strcpy(name_of_valgrind_trace,optarg);
                break;
            default:
                print_usage_info();
                break;
        }
    }

    S=1<<s;
    B=1<<b;
    if((fp=fopen(name_of_valgrind_trace,"r"))==NULL)
    {
        printf("cannt open %s\n",name_of_valgrind_trace);
        return 0;
    }

    init_cache();
    run();
    printSummary(hit_cnt, miss_cnt, eviction_cnt);
    return 0;
}
