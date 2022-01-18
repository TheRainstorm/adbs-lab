#ifndef DEFINES_H
#define DEFINES_H

#define PAGE_SIZE 4096
#define DEFBUFSIZE 1024     //buf页的数量

typedef struct bFrame
{
    char field[PAGE_SIZE];
} bFrame;

//control infomation
typedef struct BCB
{
    int page_id;
    int frame_id;
    int latch;
    int pincount;           //pincount
    int dirty;
    struct BCB *next;       //hash table
    struct BCB *free_next;  //free list

    //used for LRU repalce algorithm
    struct BCB *LRU_prev;   //LRU list(double linked list)
    struct BCB *LRU_next;

    //used for Clock replace algorithm
    char clock_bit;         //second chance bit
} BCB;

extern bFrame buf[DEFBUFSIZE];  //Buffer
extern BCB buf_bcb[DEFBUFSIZE]; //store the bcbs

#endif