#ifndef BMGR_H
#define BMGR_H
#include "defines.h"
#include "replace_alg.h"

class BMgr
{
public:
    BMgr(int alg); //init
    ~BMgr();

    //interface
    int accessPage(int page_id, int type);  //type: 0 read, 1 write
    void clear_buffer();

    //internal
    void init_bcb();
    
    void init_free_list();
    BCB *get_free();

    BCB *hash_search(int page_id);
    void hash_insert(BCB *bcb_ptr);
    void hash_delete(BCB *bcb_ptr);

    //help
    void print_statistical_data();

    // Interface functions
    // int FixPage(int page_id, int prot);
    // void NewPage FixNewPage();
    // int UnfixPage(int page_id);
    // int NumFreeFrames();
    // // Internal Functions
    // int SelectVictim();
    // int Hash(int page_id);
    // void RemoveBCB(BCB *ptr, int page_id);
    // void RemoveLRUEle(int frid);
    // void SetDirty(int frame_id);
    // void UnsetDirty(int frame_id);
    // void WriteDirtys();
    // PrintFrame(int frame_id);

private:
    // Hash Table
    // int ftop[DEFBUFSIZE];
    BCB *ptof[DEFBUFSIZE];
    BCB *free_list;

    const struct replace_alg *replace_alg;
public:
    int access_total;
    int hit;
    int write;
    int read_io, write_io;
};

#endif