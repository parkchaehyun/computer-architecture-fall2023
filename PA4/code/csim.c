/*
 * csim.c - A cache simulator that can replay traces from Valgrind
 *     and output statistics such as number of hits, misses, and
 *     evictions.  The replacement policy is LRU.
 *
 * Implementation and assumptions:
 *  1. Each load/store can cause at most one cache miss. (I examined the trace,
 *  the largest request I saw was for 8 bytes).
 *  2. Instruction loads (I) are ignored, since we are interested in evaluating
 *  trans.c in terms of its data cache performance.
 *  3. data modify (M) is treated as a load followed by a store to the same
 *  address. Hence, an M operation can result in two cache hits, or a miss and a
 *  hit plus an possible eviction.
 *
 * The function printSummary() is given to print output.
 * Please use this function to print the number of hits, misses and evictions.
 * This is crucial for the driver to evaluate your work.
 */
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include "cachelab.h"

//#define DEBUG_ON
#define ADDRESS_LENGTH 64

extern int arrCount[TYPECOUNT];
extern int verbosity; /* print trace if set */
extern int cachePolicy; // 0 : lru, 1 : fifo

/* Type: Memory address */
typedef unsigned long long int mem_addr_t;

/* Type: Cache line
   LRU is a counter used to implement LRU replacement policy  */
typedef struct cache_line {
    char valid;
    mem_addr_t tag;
    unsigned long long int lru;
    char dirty;
} cache_line_t;

typedef cache_line_t* cache_set_t;
typedef cache_set_t* cache_t;

/* Globals set by command line args */
//int verbosity = 0; /* print trace if set */
int s = 0; /* set index bits */
int s2 = 0; /* set index bits for L2 */
int b = 0; /* block offset bits */
int E = 0; /* associativity */
char* trace_file = NULL;

/* Derived from command line args */
int S; /* number of sets */
int B; /* block size (bytes) */
int S2; /* number of sets in L2 */
int E2; /* associativity of L2 */

/* Counters used to record cache statistics */
unsigned long long int lru_counter = 1;
unsigned long long int l2_lru_counter = 1;

int* l1_fifo = 0;
int* l2_fifo = 0;

/* The cache we are simulating */
cache_t cache, cache_l2;
mem_addr_t set_index_mask, set_index_mask_l2;

/*
 * initCache - Allocate memory, write 0's for valid and tag and LRU
 * also computes the set_index_mask
 */
void initCache()
{
    int i,j;
    cache = (cache_set_t*) malloc(sizeof(cache_set_t) * S);
    cache_l2 = (cache_set_t*)malloc(sizeof(cache_set_t) * S2);
    for (i=0; i<S; i++){
        cache[i]=(cache_line_t*) malloc(sizeof(cache_line_t) * E);
        for (j=0; j<E; j++){
            cache[i][j].valid = 0;
            cache[i][j].tag = 0;
            cache[i][j].lru = 0;
            cache[i][j].dirty = 0;
        }
    }

    for(i = 0; i < S2; i++){
        cache_l2[i] = (cache_line_t*) malloc(sizeof(cache_line_t) * E2);
        for(j = 0; j < E2; j++){
            cache_l2[i][j].valid = 0;
            cache_l2[i][j].tag = 0;
            cache_l2[i][j].lru = 0;
            cache_l2[i][j].dirty = 0;
        }
    }

    /* Computes set index mask */
    set_index_mask = (mem_addr_t) (pow(2, s) - 1);
    set_index_mask_l2 = (mem_addr_t) (pow(2, s2) - 1);

    l1_fifo = (int*)malloc(sizeof(int) * S);
    l2_fifo = (int*)malloc(sizeof(int) * S2);
    for (i=0; i<S; i++){
        l1_fifo[i] = 0;
    }
    for (i=0; i<S2; i++){
        l2_fifo[i] = 0;
    }
}



/*
 * freeCache - free allocated memory
 */
void freeCache()
{
    int i;
    for (i=0; i<S; i++){
        free(cache[i]);
    }
    free(cache);

    for(i = 0; i < S2; i++){
        free(cache_l2[i]);
    }
    free(cache_l2);

    free(l1_fifo);
    free(l2_fifo);
}


/*
 * accessData - Access data at memory address addr.
 *   If it is already in cache, increast hit_count
 *   If it is not in cache, bring it in cache, increase miss count.
 *   Also increase eviction_count if a line is evicted.
 *   Process writes if write == 1, process reads if write == 0
 */
void accessData(mem_addr_t addr, int write) {
    mem_addr_t set_index, tag; //,block_offset;
    mem_addr_t set_index_l2, tag_l2; //,block_offset_l2;
    int i;
    int hit = 0;
    int hit_l2 = 0;

    /* Get set_index and tag */
    set_index = (addr >> b) & set_index_mask;
    tag = addr >> (s + b);

    set_index_l2 = (addr >> b) & set_index_mask_l2;
    tag_l2 = addr >> (s2 + b);

    /* Check if it hits in L1 */
    for (i = 0; i < E; i++) {
        if (cache[set_index][i].valid == 1 && tag==cache[set_index][i].tag) {
            hit = 1;
            break;
        }
    }
    
    if (hit) {
        /* If it hits in L1 */
        Verbose(L1_Hit);
        cache[set_index][i].lru = lru_counter;
        lru_counter++;
        mem_addr_t write_back_set_index = ((cache[set_index][i].tag & 1) << s | set_index);
                int j;
                for (j = 0; j < E2; j++) {
                    if (cache_l2[write_back_set_index][j].valid == 1 && (cache_l2[write_back_set_index][j].tag << 1 | ((write_back_set_index >> s) & 1)) == cache[set_index][i].tag) {
                        break;
                    }
                }
                cache_l2[write_back_set_index][j].lru = l2_lru_counter;
        l2_lru_counter++;
        if (write) {
            cache[set_index][i].dirty = 1;
        }
    } else {
        /* If it misses in L1 */
        Verbose(L1_Miss);

        /* check if there is an empty block in L1 */
        int empty_index = -1;
        for (i = 0; i < E; i++) {
            if (cache[set_index][i].valid == 0) {
                empty_index = i;
                break;
            }
        }
        /* If there is no empty block in L1 */
        if (empty_index == -1) {
            Verbose(L1_Eviction);
            /* evict a block from L1 */
            int min = INT_MAX;
            if (cachePolicy == 0) {
                for (i = 0; i < E; i++) {
                    if (cache[set_index][i].lru < min) {
                        min = cache[set_index][i].lru;
                        empty_index = i;
                    }
                }
            } else {
                empty_index = l1_fifo[set_index];
            }
            /* If the evicted block is dirty, write back to L2 */
            if (cache[set_index][empty_index].dirty) {
                /* find the corresponding block in L2 */
                mem_addr_t write_back_set_index = ((cache[set_index][empty_index].tag & 1) << s | set_index);
                int j;
                for (j = 0; j < E2; j++) {
                    if (cache_l2[write_back_set_index][j].valid == 1 && (cache_l2[write_back_set_index][j].tag << 1 | ((write_back_set_index >> s) & 1)) == cache[set_index][empty_index].tag) {
                        break;
                    }
                }

                cache_l2[write_back_set_index][j].dirty = 1;
                cache_l2[write_back_set_index][j].lru = l2_lru_counter;
                l2_lru_counter++;
                cache[set_index][empty_index].dirty = 0;

                Verbose(L2_Write);
            }
        }

        cache[set_index][empty_index].valid = 1;
        cache[set_index][empty_index].tag = tag;
        cache[set_index][empty_index].lru = lru_counter;
        lru_counter++;
        l1_fifo[set_index]++;
        if (l1_fifo[set_index] == E) {
            l1_fifo[set_index] = 0;
        }
        if (write) {
            cache[set_index][empty_index].dirty = 1;
        }

        /* Check if it hits in L2 */
        for (i = 0; i < E2; i++) {
            if (cache_l2[set_index_l2][i].valid == 1 && tag_l2==cache_l2[set_index_l2][i].tag) {
                hit_l2 = 1;
                break;
            }
        }

        /* If it hits in L2 */
        if (hit_l2) {
            Verbose(L2_Hit);
            cache_l2[set_index_l2][i].lru = l2_lru_counter;
            l2_lru_counter++;
        } else { /* L2 miss */
            Verbose(L2_Miss);
            /* check if there is an empty block in L2 */
            int empty_index_l2 = -1;
            for (i = 0; i < E2; i++) {
                if (cache_l2[set_index_l2][i].valid == 0) {
                    empty_index_l2 = i;
                    break;
                }
            }

            /* If there is no empty block in L2, evict */
            if (empty_index_l2 == -1) {
                Verbose(L2_Eviction);
                /* evict a block from L2 */
                int min_l2 = INT_MAX;
                if (cachePolicy == 0) {
                    for (i = 0; i < E2; i++) {
                        if (cache_l2[set_index_l2][i].lru < min_l2) {
                            min_l2 = cache_l2[set_index_l2][i].lru;
                            empty_index_l2 = i;
                        }
                    }
                } else {
                    empty_index_l2 = l2_fifo[set_index_l2];
                }

                /* invalidate the corresponding block (if any) in L1 */
                int j;
                mem_addr_t L1_tag = (cache_l2[set_index_l2][empty_index_l2].tag << 1 | ((set_index_l2 >> s) & 1));
                for (j = 0; j < E; j++) {
                    if (cache[set_index][j].valid == 1 && cache[set_index][j].tag == L1_tag) {
                        cache[set_index][j].valid = 0;
                        cache_l2[set_index_l2][empty_index_l2].valid = 0;
                        Verbose(L1_Eviction);
                        if(cache[set_index][j].dirty == 1){
                            cache[set_index][j].dirty = 0;
                            cache_l2[set_index_l2][empty_index_l2].dirty = 0;
                            Verbose(Mem_Write);
                        }
                        break;
                    }
                }

                /* If the evicted block is dirty, write back to memory */
                if (cache_l2[set_index_l2][empty_index_l2].dirty) {
                    Verbose(Mem_Write);
                    cache_l2[set_index_l2][empty_index_l2].dirty = 0;
                }
            }

            cache_l2[set_index_l2][empty_index_l2].valid = 1;
            cache_l2[set_index_l2][empty_index_l2].tag = tag_l2;
            cache_l2[set_index_l2][empty_index_l2].lru = l2_lru_counter;
            l2_fifo[set_index_l2]++;
            if (l2_fifo[set_index_l2] == E2) {
                l2_fifo[set_index_l2] = 0;
            }
            l2_lru_counter++;
        }
    }
}

/*
 * replayTrace - replays the given trace file against the cache
 */
void replayTrace(char* trace_fn)
{
    char buf[1000];
    mem_addr_t addr=0;
    unsigned int len=0;
    FILE* trace_fp = fopen(trace_fn, "r");

    if(!trace_fp){
        fprintf(stderr, "%s: %s\n", trace_fn, strerror(errno));
        exit(1);
    }

    while( fgets(buf, 1000, trace_fp) != NULL) {
        if(buf[1]=='S' || buf[1]=='L' || buf[1]=='M') {
            sscanf(buf+3, "%llx,%u", &addr, &len);
      
            if(verbosity)
                printf("%c 0x%llx,%u ", buf[1], addr, len);

        /* If the instruction contains R */
        if(buf[1] == 'L' || buf[1] == 'M'){
                accessData(addr, 0);
        }

            /* If the instruction contains W */
            if(buf[1]=='M' || buf[1] == 'S')
                accessData(addr, 1);
            
            if (verbosity)
                printf("\n");
        }
    }

    fclose(trace_fp);
}

/*
 * printUsage - Print usage info
 */
void printUsage(char* argv[])
{
    printf("Usage: %s [-hv] -s <num> -E <num> -b <num> -p <num> -t <file>\n", argv[0]);
    printf("Options:\n");
    printf("  -h         Print this help message.\n");
    printf("  -v         Optional verbose flag.\n");
    printf("  -s <num>   Number of set index bits.\n");
    printf("  -E <num>   Number of lines per set.\n");
    printf("  -b <num>   Number of block offset bits.\n");
    printf("  -p <num>   Select Cache Policy, 0 : lru, 1 : fifo.\n");
    printf("  -t <file>  Trace file.\n");
    printf("\nExamples:\n");
    printf("  linux>  %s -s 4 -E 1 -b 4 -t traces/yi.trace\n", argv[0]);
    printf("  linux>  %s -v -s 8 -E 2 -b 4 -t traces/yi.trace\n", argv[0]);
    exit(0);
}

/*
 * main - Main routine
 */
int main(int argc, char* argv[])
{
    char c;
    verbosity = 0;
    cachePolicy = 0;
    while( (c=getopt(argc,argv,"s:E:b:t:p:vh")) != -1){
        switch(c){
        case 's':
            s = atoi(optarg);
            break;
        case 'E':
            E = atoi(optarg);
            break;
        case 'b':
            b = atoi(optarg);
            break;
        case 't':
            trace_file = optarg;
            break;
        case 'v':
            verbosity = 1;
            break;
        case 'p':
            cachePolicy = atoi(optarg);
        break;
        case 'h':
            printUsage(argv);
            exit(0);
        default:
            printUsage(argv);
            exit(1);
        }
    }

    /* Make sure that all required command line args were specified */
    if (s == 0 || E == 0 || b == 0 || trace_file == NULL || !(cachePolicy == 0 || cachePolicy == 1)) {
        printf("%s: Missing required command line argument\n", argv[0]);
        printUsage(argv);
        exit(1);
    }

    s2 = s + 1;

    /* Compute S, E and B from command line args */
    S = (unsigned int) pow(2, s);
    B = (unsigned int) pow(2, b);

    /* Compute S2, E2, and B2 from command line args */
    S2 = (unsigned int) pow(2, s2);
    E2 = (unsigned int) E * 4;
 
    /* Initialize cache */
    initCache();
    
    initCount();

#ifdef DEBUG_ON
    printf("DEBUG: S:%u E:%u B:%u trace:%s\n", S, E, B, trace_file);
    printf("DEBUG: set_index_mask: %llu\n", set_index_mask);
#endif
 
    replayTrace(trace_file);

    /* Free allocated memory */
    freeCache();

    printf("level\t\thits\t\tmisses\t\tevictions\t\t\n");
    printf("L1\t\t%d\t\t%d\t\t%d\t\t\n", arrCount[L1_Hit], arrCount[L1_Miss], arrCount[L1_Eviction]);
    printf("L2\t\t%d\t\t%d\t\t%d\t\t\n", arrCount[L2_Hit], arrCount[L2_Miss], arrCount[L2_Eviction]);
    printf("writes to L2: %d, writes to memory: %d\n", arrCount[L2_Write], arrCount[Mem_Write]);
    
    printSummary();

    return 0;
}
