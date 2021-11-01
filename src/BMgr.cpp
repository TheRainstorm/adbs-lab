#include "BMgr.h"
#include <stdio.h>
#include <string.h>

// bFrame buf[DEFBUFSIZE]; //store the frames
BCB buf_bcb[DEFBUFSIZE]; //store the bcbs

unsigned int _hash(unsigned int x){
    // x = ((x >> 16) ^ x) * 0x45d9f3b;
    // x = ((x >> 16) ^ x) * 0x45d9f3b;
    // x = (x >> 16) ^ x;
    return x;
}

BMgr::BMgr(){
    //init hash table
    memset(ptof, NULL, DEFBUFSIZE*sizeof(BCB *)); //init hash table to be empty

    init_bcb();

    init_free_list();   //add all frames to free list

    init_LRU();

    //init statistic
    access_total = 0;
    hit = 0;
    write = 0;
}

int BMgr::accessPage(int page_id, int type){
    access_total++;
    //hit?
    BCB * bcb_ptr = hash_search(page_id);
    if(bcb_ptr!=NULL){  //hit!
        //Update dirty
        if(type==1){
            /**write to buffer
             */
            bcb_ptr->dirty = 1; //set dirty
            write++;
        }
        update_LRU(bcb_ptr, 0);
        hit++;
        return bcb_ptr->frame_id;
    }

    //free frame?
    bcb_ptr = get_free();
    int from_free = !(bcb_ptr==NULL);
    if(bcb_ptr==NULL){
        //select victim
        bcb_ptr = select_victim();
        //check dirty
        if(bcb_ptr->dirty){
            /*
            call Disk manager to write back page
            writeback(bcb_ptr->page_id, bcb[bcb_ptr->frame_id])
            */
           bcb_ptr->dirty=0;
        }
        hash_delete(bcb_ptr);   //delete original page_id to frame_id hash map
    }

    if(type==1){
        /**write to buffer
         */
        bcb_ptr->dirty = 1; //set dirty
        write++;
    }else{
        /* 
        call Disk manager to read page, and write to buffer
        read(page_id, bcb[bcb_ptr->frame_id])
        */
       ;
    }
    bcb_ptr->page_id = page_id;

    update_LRU(bcb_ptr, from_free);
    hash_insert(bcb_ptr);

    return bcb_ptr->frame_id;
}

void BMgr::init_bcb(){
    for(int i=0; i<DEFBUFSIZE; i++){
        buf_bcb[i].page_id = -1;
        buf_bcb[i].frame_id = i;
        buf_bcb[i].latch = 0;
        buf_bcb[i].count = 0;
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

BCB *BMgr::select_victim(){
    return LRU_head->LRU_prev;
}

void BMgr::init_LRU(){
    LRU_head = new BCB;    //init LRU list to be empty. empty head simplify discussion
    LRU_head->LRU_next = LRU_head->LRU_prev = LRU_head;
    LRU_head->frame_id = -1;
}

void BMgr::update_LRU(BCB *bcb_ptr, int from_free){
    if(from_free==1){
        lru_link_insert_head(bcb_ptr); //insert bcb_ptr to the head
    }else{
        lru_link_delete(bcb_ptr);
        lru_link_insert_head(bcb_ptr); //insert bcb_ptr to the head
    }
}

void BMgr::lru_link_delete(BCB *bcb_ptr){
    bcb_ptr->LRU_next->LRU_prev = bcb_ptr->LRU_prev;
    bcb_ptr->LRU_prev->LRU_next = bcb_ptr->LRU_next;
}

void BMgr::lru_link_insert_head(BCB *bcb_ptr){
    bcb_ptr->LRU_prev = LRU_head;
    bcb_ptr->LRU_next = LRU_head->LRU_next;
    LRU_head->LRU_next->LRU_prev = bcb_ptr;
    LRU_head->LRU_next = bcb_ptr;
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

void BMgr::print_statistical_data(){
    printf("total: %d\n", access_total);
    printf("hit rate: %f\n", (float)hit/access_total);
    printf("write rate: %f\n", (float)write/access_total);
}