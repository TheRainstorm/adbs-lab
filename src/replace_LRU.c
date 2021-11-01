#include "replace_alg.h"
#include "defines.h"
#include <stdlib.h>

BCB *LRU_head;  //

static inline void lru_link_delete(BCB *bcb_ptr){
    bcb_ptr->LRU_next->LRU_prev = bcb_ptr->LRU_prev;
    bcb_ptr->LRU_prev->LRU_next = bcb_ptr->LRU_next;
}

static inline void lru_link_insert_head(BCB *bcb_ptr){
    bcb_ptr->LRU_prev = LRU_head;
    bcb_ptr->LRU_next = LRU_head->LRU_next;
    LRU_head->LRU_next->LRU_prev = bcb_ptr;
    LRU_head->LRU_next = bcb_ptr;
}

void init_LRU(){
    LRU_head = (BCB *)malloc(sizeof(BCB));    //init LRU list to be empty. empty head simplify discussion
    LRU_head->LRU_next = LRU_head->LRU_prev = LRU_head;
    LRU_head->frame_id = -1;
}

void update_LRU(BCB *bcb_ptr, int from_free){
    if(from_free==1){
        lru_link_insert_head(bcb_ptr); //insert bcb_ptr to the head
    }else{
        lru_link_delete(bcb_ptr);
        lru_link_insert_head(bcb_ptr); //insert bcb_ptr to the head
    }
}

BCB *select_victim_LRU(){
    return LRU_head->LRU_prev;
}

const struct replace_alg LRU_replace_alg = {
    .name = "LRU",
    .init = init_LRU,
    .update = update_LRU,
    .select_victim = select_victim_LRU,
};