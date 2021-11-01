#ifndef BMGR_H
#define BMGR_H

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
// extern BCB buf_bcb[DEFBUFSIZE]; //store the bcbs

class BMgr
{
public:
    BMgr(); //init

    //interface
    int accessPage(int page_id, int type);  //type: 0 read, 1 write

    //internal
    void init_bcb();
    
    void init_free_list();
    BCB *get_free();

    BCB *hash_search(int page_id);
    void hash_insert(BCB *bcb_ptr);
    void hash_delete(BCB *bcb_ptr);

    BCB *select_victim();
    //LRU
    void init_LRU();
    void update_LRU(BCB *bcb_ptr, int from_free);
    void lru_link_delete(BCB *bcb_ptr);
    void lru_link_insert_head(BCB *bcb_ptr);
    
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
    BCB *LRU_head;
    BCB *free_list;
public:
    int access_total;
    int hit;
    int write;
};

#endif