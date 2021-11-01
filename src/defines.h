#ifndef DEFINES_H
#define DEFINES_H

#define FRAMESIZE 4096
#define DEFBUFSIZE 1024

typedef struct bFrame
{
    char field[FRAMESIZE];
} bFrame;

//control infomation
typedef struct BCB
{
    int page_id;
    int frame_id;
    int latch;
    int count;
    int dirty;
    struct BCB *next;       //hash table
    struct BCB *free_next;  //free list
    struct BCB *LRU_prev;   //LRU list(double linked list)
    struct BCB *LRU_next;
    char clock_bit;         //clock replacement algrothim
} BCB;

// extern bFrame buf[DEFBUFSIZE]; //store the frames
extern BCB buf_bcb[DEFBUFSIZE]; //store the bcbs

#endif