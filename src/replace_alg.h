#ifndef REPLACE_ALG_H
#define REPLACE_ALG_H
#include "defines.h"

struct replace_alg {
    const char *name;                               //e.g LRU, Clock
    void (*init)();                                 //init the required variable in BCB
    void (*update)(struct BCB *bcb_ptr, int from_free);    //update after a access to a page
    struct BCB *(*select_victim)();                        //select the victim page
};

extern const struct replace_alg LRU_replace_alg;
extern const struct replace_alg Clock_replace_alg;

#endif