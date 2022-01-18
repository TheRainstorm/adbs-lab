#include "replace_alg.h"
#include "defines.h"

//global variable
int Clock_current;

static void init(){
    Clock_current=0;
    for(int i=0; i<DEFBUFSIZE; i++){
        buf_bcb[i].clock_bit=0;
    }
}

static void update(BCB *bcb_ptr, int is_free_frame){
    bcb_ptr->clock_bit=1;    //1 means have been accessed recently
}

static BCB *select_victim(){
    int cnt=0;
    BCB * bcb_ptr;
    while(1){
        bcb_ptr = &buf_bcb[Clock_current];
        if(bcb_ptr->clock_bit==0 || cnt++>=10){
            Clock_current = (Clock_current+1)%DEFBUFSIZE;
            return bcb_ptr;
        }
        bcb_ptr->clock_bit = 0;
        Clock_current = (Clock_current+1)%DEFBUFSIZE;
    }
}

const struct replace_alg Clock_replace_alg = {
    .name = "Clock",
    .init = init,
    .update = update,
    .select_victim = select_victim,
};
