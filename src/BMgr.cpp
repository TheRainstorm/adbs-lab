#include "BMgr.h"
#include <string.h>

bFrame buf[DEFBUFSIZE];     //store the frames
BCB buf_bcb[DEFBUFSIZE];    //store the bcbs

unsigned int _hash(unsigned int x){
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x;
}

BMgr::BMgr(std::string filename, int alg){
    dsmgr = new DSMgr(filename);

    init_bcb();         //init bcb array
    init_free_list();   //add all frames to free list
    memset(ptof, 0, DEFBUFSIZE*sizeof(BCB *)); //init hash table to be empty
    
    //choose replace algorithm
    switch (alg){
        case LRU:
            replace_alg = &LRU_replace_alg;
            break;
        case Clock:
            replace_alg = &Clock_replace_alg;
            break;
        case Random:
            replace_alg = &Random_replace_alg;
            break;
        default:
            exit(1);
    }
    replace_alg->init();
    printf("Used Replace Alg: %s\n", replace_alg->name);

    init_statistical_data(); //init statistic
}

BMgr::~BMgr(){
    delete dsmgr;
    clear_buffer();
}

int BMgr::FixPage(int page_id, int type){
    access_total++; //statistic

    //hit?
    BCB * bcb_ptr = hash_search(page_id);
    if(bcb_ptr!=NULL){  //hit!
        hit++;
        if(type==1){    //write
            /**
             * write to buffer
             */
            bcb_ptr->dirty = 1; //set dirty
            write++;
        }
        replace_alg->update(bcb_ptr, 0);
        return bcb_ptr->frame_id;
    }

    /*no hit
    1. check if there is free frame
        1. if no
            1. **select victim frame**
            2. writeback (when dirty)
    2. now we get a free frame(from free or victim)
        1. if request is write
            1. write to free frame
        2. if read
            1. call memory manger to read page from disk
            2. write to free frame
    3. update LRU(or other replace algrithm data struct)
    4. map page_id to frame(hash insert)
    */
    bcb_ptr = SelectVictim();   //check free and select victim

    if(type==1){
        /** 
         * write to buffer
         */
        bcb_ptr->dirty = 1; //set dirty
        write++;
    }else{
        /** 
         * call Disk manager to read page, and write to buffer
         * read(page_id, bcb[bcb_ptr->frame_id])
        */
        dsmgr->ReadPage(page_id, &buf[bcb_ptr->frame_id]);
        read_io++;
    }
    bcb_ptr->page_id = page_id;

    hash_insert(bcb_ptr);

    return bcb_ptr->frame_id;
}

int BMgr::FixNewPage(int *page_id_ptr){
    BCB *bcb_ptr = SelectVictim();
    int page_id = dsmgr->NewPage();
    *page_id_ptr = page_id;

    bcb_ptr->page_id = page_id;
    hash_insert(bcb_ptr);
    bcb_ptr->pincount++;

    return bcb_ptr->frame_id;
}

int BMgr::UnfixPage(int page_id) {
    BCB * bcb_ptr = hash_search(page_id);
    if(bcb_ptr==NULL){
        fprintf(stderr, "ERROR: UnfixPage %d failed\n", page_id);
        exit(1);
    }
    if(bcb_ptr->pincount==0){
        hash_delete(bcb_ptr);
    }else{
        bcb_ptr->pincount--;
    }
    return bcb_ptr->frame_id;
}

int BMgr::NumFreeFrames() {
    BCB * ptr = free_list;
    int cnt = 0;
    while(ptr!=NULL){
        cnt ++;
        ptr = ptr->free_next;
    }
    return cnt;
}


void BMgr::clear_buffer(){
    for(int i=0; i<DEFBUFSIZE; i++){
        if(buf_bcb[i].dirty==1){
            /** 
             * call Disk manager to write back page
             * writeback(bcb_ptr->page_id, bcb[bcb_ptr->frame_id])
            */
           write_io++;
        }
    }
}

BCB *BMgr::SelectVictim(){
    //free frame?
    BCB *bcb_ptr;
    bcb_ptr = get_free();
    int is_free_frame = !(bcb_ptr==NULL);
    if(bcb_ptr==NULL){  //no free frame
        bcb_ptr = replace_alg->select_victim();     //select victim
        if(bcb_ptr->dirty){                         //check dirty
            /** 
             * call Disk manager to write back page
             * writeback(bcb_ptr->page_id, bcb[bcb_ptr->frame_id])
            */
           dsmgr->WritePage(bcb_ptr->page_id, &buf[bcb_ptr->frame_id]);
           write_io++;
           bcb_ptr->dirty=0;
        }
        hash_delete(bcb_ptr);   //delete original page_id to frame_id hash map
    }
    replace_alg->update(bcb_ptr, is_free_frame);

    return bcb_ptr;
}

void BMgr::init_bcb(){
    for(int i=0; i<DEFBUFSIZE; i++){
        buf_bcb[i].page_id = -1;
        buf_bcb[i].frame_id = i;
        buf_bcb[i].latch = 0;
        buf_bcb[i].pincount = 0;
        buf_bcb[i].dirty = 0;

        buf_bcb[i].next = NULL;
        buf_bcb[i].free_next = NULL;
        buf_bcb[i].LRU_prev = NULL;
        buf_bcb[i].LRU_next = NULL;
    }
}

void BMgr::init_free_list(){
    for(int i=0; i<DEFBUFSIZE-1; i++){
        buf_bcb[i].free_next = &buf_bcb[i+1];
    }
    buf_bcb[DEFBUFSIZE-1].next=NULL;
    free_list = &buf_bcb[0];
}

BCB *BMgr::get_free(){
    BCB * bcb_ptr = free_list;
    if(free_list==NULL){
        return NULL;
    }else{
        free_list = free_list->free_next;
        return bcb_ptr;
    }
}

BCB *BMgr::hash_search(int page_id){
    int index = _hash(page_id)%DEFBUFSIZE;
    BCB * bcb_ptr = ptof[index];
    while(bcb_ptr!=NULL){   //chain size not limited
        if(bcb_ptr->page_id==page_id){
            return bcb_ptr;
        }
        bcb_ptr = bcb_ptr->next;
    }
    return NULL;
}

void BMgr::hash_insert(BCB *bcb_ptr){
    int index = _hash(bcb_ptr->page_id)%DEFBUFSIZE;
    BCB *bcb_hash_ptr = ptof[index];
    //fix bug: bcb_ptr->next forget to set NULL when if condition is true
    //      which cause infinite loop when search
    // if(bcb_hash_ptr==NULL){
    //     ptof[index] = bcb_ptr;
    // }else{
    //     bcb_ptr->next = bcb_hash_ptr;
    //     ptof[index] = bcb_ptr;
    // }
    bcb_ptr->next = bcb_hash_ptr;
    ptof[index] = bcb_ptr;
}

void BMgr::hash_delete(BCB *bcb_ptr){
    int index = _hash(bcb_ptr->page_id)%DEFBUFSIZE;
    BCB *bcb_hash_ptr = ptof[index];
    if(bcb_hash_ptr==bcb_ptr){
        // ptof[index] = NULL;          //fix bug: first hit doesn't mean only one element
        ptof[index] = bcb_ptr->next;    
    }else{
        while(bcb_hash_ptr->next!=NULL){
            if(bcb_hash_ptr->next==bcb_ptr){
                bcb_hash_ptr->next = bcb_ptr->next;
                break;
            }
            bcb_hash_ptr=bcb_hash_ptr->next;
        }
    }
}

void BMgr::init_statistical_data(){
    access_total = 0;
    hit = 0;
    write = 0;
    write_io = read_io = 0;
}

void BMgr::print_statistical_data(){
    printf("total: %d\n", access_total);
    printf("hit rate: %.3f%%\n", (float)hit/access_total*100);
    printf("write rate: %.3f%%\n", (float)write/access_total*100);
    printf("read_io: %d\n", read_io);
    printf("write_io: %d\n", write_io);
    printf("total_io: %d\n", read_io+write_io);
}