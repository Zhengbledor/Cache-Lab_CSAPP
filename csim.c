/*
    Name:郑志豪
    Time:2021/1/3
    StudentId:U201814781
*/

//************include
#include "cachelab.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

//*************Data Structure
typedef struct
{
    int valid;     //is the line valid
    long long  tag; //line tag
    int LRUnumber; //count in order to displace the line
} Line;            //一个line
typedef struct
{
    Line *line; //array of Line
} Set;
typedef struct
{
    int s;     //the number of Sets is S=2^s
    int E;     //the number of Lines in one set
    int b;     //2^b bytes per cache block
    Set *sets; //array of Sets
} Cache;

//***************function
int get_opt(int argc, char **argv, int *s, int *E, int *b, char *traceFileName, int *isVerbose);
void printHelp();
void initCache(Cache *cache, int s, int E, int b);
extern char *optarg;
extern int optind;
extern int opterr;
extern int optopt;
int getopt(int argc, char *const argv[], const char *optstring);
void getSetTag(long long addr, int s, int b, int *set, long long *tag);
void storeData(Cache *cache, int set, long long tag);
void modifyData(Cache *cache, int set, long long tag);
void loadData(Cache *cache, int set, long long tag);
void readData(Cache *cache, int set, long long tag);
void doEvict(Cache *cache, int set, long long tag);
int findMaxLRUnumber(Set *set, int length);
void isMiss(Cache *cache, int set, long long tag, int *flag);
void freeAll(Cache *cache);

//****************macro
#define FILENAME_LENGTH 100

//***************global variable
int misses;
int hits;
int evictions;

int main(int argc, char **argv)
{
    int s, E, b, isVerbose = 0;
    Cache *cache = (Cache *)malloc(sizeof(Cache));
    char *traceFileName = (char *)malloc(FILENAME_LENGTH * sizeof(char));
    get_opt(argc, argv, &s, &E, &b, traceFileName, &isVerbose);
    initCache(cache, s, E, b);

    FILE *traceFile = fopen(traceFileName, "r");
    char option[10];
    int addr, size;

    while (fscanf(traceFile, "%s %x,%d", option, &addr, &size) != EOF)
    {
        int set;
        long long tag;
        char opt = option[0];
        getSetTag(addr, s, b, &set, &tag);
        if (isVerbose == 1)
            printf("%c %x,%d", opt, addr, size);
        switch (opt)
        {
        case 'I':
            break;
        case 'S':
            storeData(cache, set, tag);
            break;
        case 'M':
            modifyData(cache, set, tag);
            break;
        case 'L':
            loadData(cache, set, tag);
            break;
        default:
            break;
        }
        printf("\n");
    }
    freeAll(cache);
    printSummary(hits, misses, evictions);
    return 0;
}

int get_opt(int argc, char **argv, int *s, int *E, int *b, char *traceFileName, int *isVerbose)
{
    char op;
    while ((op = getopt(argc, argv, "hvs:E:b:t:")) != -1)
    {
        switch (op)
        {
        case 'v':
            *isVerbose = 1;
            break;
        case 's':
            if ((*s = atoi(optarg)) == 0)
            {
                printf("./csim-ref: Missing required command line argument\n");
                printHelp();
            }
            break;
        case 'E':
            if ((*E = atoi(optarg)) == 0)
            {
                printf("./csim-ref: Missing required command line argument\n");
                printHelp();
            }
            break;
        case 'b':
            if ((*b = atoi(optarg)) == 0)
            {
                printf("./csim-ref: Missing required command line argument\n");
                printHelp();
            }
            break;
        case 't':
            strcpy(traceFileName, optarg);
            break;
        case 'h':
        default:
            printHelp();
            break;
        }
    }
    return 1;
}

void printHelp()
{
    printf("Usage: ./csim-ref [-hv] -s <num> -E <num> -b <num> -t <file>\n"
           "Options:\n"
           "  -h         Print this help message.\n"
           "  -v         Optional verbose flag.\n"
           "  -s <num>   Number of set index bits.\n"
           "  -E <num>   Number of lines per set.\n"
           "  -b <num>   Number of block offset bits.\n"
           "  -t <file>  Trace file.\n\n"
           "Examples:\n"
           "    linux>  ./csim-ref -s 4 -E 1 -b 4 -t traces/yi.trace\n"
           "  linux>  ./csim-ref -v -s 8 -E 2 -b 4 -t traces/yi.trace\n");
    exit(0);
}

void initCache(Cache *cache, int s, int E, int b)
{
    cache->s = 2 << s; //has 2^s sets
    cache->E = E;      //one set has E lines;
    if ((cache->sets = (Set *)malloc(cache->s * sizeof(Set))) == NULL)
    {
        printf("Set Memory Error!\n");
        exit(0);
    }
    int i, j;
    for (i = 0; i < cache->s; i++)
    {
        if ((cache->sets[i].line = (Line *)malloc(E * sizeof(Line))) == NULL)
        {
            printf("Line Memory Error!\n");
            exit(0);
        }
        for (j = 0; j < E; j++)
        {
            cache->sets[i].line[j].valid = 0;
            cache->sets[i].line[j].LRUnumber = 0;
        }
    }
}

void getSetTag(long long addr, int s, int b, int *set, long long *tag)
{
    long long tempaddr = addr >> b;
    int mask = (1 << s) - 1;
    *set = (tempaddr & mask);
    mask = s + b;
    *tag = addr >> mask;
}

void storeData(Cache *cache, int set, long long tag)
{ //miss or hit
    readData(cache, set, tag);
}

void modifyData(Cache *cache, int set, long long tag)
{ //miss hit or hit hit
    storeData(cache, set, tag);
    loadData(cache, set, tag);
}

void loadData(Cache *cache, int set, long long tag)
{ //miss or hit
    readData(cache, set, tag);
}

void readData(Cache *cache, int set, long long tag)
{
    int flag;
    isMiss(cache, set, tag, &flag);
    if (flag == -2) //hit
    {
        hits++;
        printf(" hit");
    }
    else //miss
    {
        misses++;
        printf(" miss");
        if (flag >= 0) //find a empty  line
        {
            Line *line = &(cache->sets[set].line[flag]);
            line->LRUnumber = 0;
            line->tag = tag;
            line->valid = 1;
        }
        else
        {
            evictions++;
            printf(" eviction");
            doEvict(cache, set, tag);
        }
    }
}

void doEvict(Cache *cache, int set, long long tag)
{
    Set *sets = &(cache->sets[set]);
    int index = findMaxLRUnumber(sets, cache->E);
    sets->line[index].LRUnumber = 0;
    sets->line[index].tag = tag;
}

int findMaxLRUnumber(Set *set, int length)
{
    int i;
    int max = 0;
    int index;
    for (i = 0; i < length; i++)
    {
        if (set->line[i].LRUnumber > max)
        {
            index = i;
            max = set->line[i].LRUnumber;
        }
    }
    return index;
}

void isMiss(Cache *cache, int set, long long tag, int *flag)
{
    int i;
    int isfind = 0;
    *flag = -1;
    for (i = 0; i < cache->E; i++)
    {
        Line *line = &(cache->sets[set].line[i]);
        if (line->valid == 1)
        {
            line->LRUnumber++;
            if (line->tag == tag) //hit
            {
                line->LRUnumber = 0;
                isfind = 1;
            }
        }
        if (line->valid == 0)
        {
            *flag = i; //find a unused line;
        }
    }
    if (isfind == 1)
        *flag = -2;
}

void freeAll(Cache *cache)
{
    int i;
    for (i = 0; i < cache->s; i++)
    {
        free(cache->sets[i].line);
    }
    free(cache->sets);
}
