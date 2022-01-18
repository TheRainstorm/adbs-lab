#ifndef BMGR_H
#define BMGR_H

#include <iostream>
#include "defines.h"
#include "DSMgr.h"
#include "replace_alg.h"

class BMgr
{
public:
    BMgr(std::string filename, int alg);                      //alg选择替换算法
    ~BMgr();

// Interface functions
    int FixPage(int page_id, int type); //type: 0 read, 1 write
    int FixNewPage(int *page_id_ptr);   //调用DSMgr创建一个新的页，返回frame_id，和page_id(通过指针返回)
    int UnfixPage(int page_id);
    int NumFreeFrames();

// Internal Functions
    BCB *SelectVictim();                //根据替换算法选择一个替换帧，BCB中包含需要的frame_id
    // int Hash(int page_id);
    // void RemoveBCB(BCB *ptr, int page_id);
    // void RemoveLRUEle(int frid);
    // void SetDirty(int frame_id);
    // void UnsetDirty(int frame_id);
    // void WriteDirtys();
    // PrintFrame(int frame_id);
    
// My internal
    void clear_buffer();                //将bufer中为dirty的frame全部写回磁盘

    void init_bcb();
    //空闲frame链表操作
    void init_free_list();              //开始所有frame都是空闲的，初始化每个frame都指向下一个frame
    BCB *get_free();                    //从free list中找到一个空闲的frame，没找到返回NULL

    BCB *hash_search(int page_id);      //在buffer中寻找page_id对应的frame是否存在，不存在返回NULL
    void hash_insert(BCB *bcb_ptr);     
    void hash_delete(BCB *bcb_ptr);     //从hash表中，删除bcb_ptr->page_id存在的映射

    //statistic helper
    void init_statistical_data();
    void print_statistical_data();
private:
    // Hash Table
    // int ftop[DEFBUFSIZE];
    BCB *ptof[DEFBUFSIZE];
    BCB *free_list;

    const struct replace_alg *replace_alg;  //使用C的结构体来表示类
    DSMgr *dsmgr;                       //data storage manger实例
public:
    //statistic data
    int access_total;       //读写的总次数
    int hit;                //读写buffer命中次数
    int write;              //读写中，写的次数
    int read_io, write_io;  //磁盘io次数（如读不命中，写dirty替换）
};

#endif