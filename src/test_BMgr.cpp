#include "BMgr.h"
#include <stdio.h>
#include <stdlib.h>

int main(){
    const char *trace_file = "../data-5w-500000-zipf.txt";
    FILE *fp;
    if(!(fp = fopen(trace_file, "r"))){
        printf("open %s filed\n", trace_file);
        exit(1);
    }

    int type, page_id;

    BMgr bmgr(0);
    fseek(fp, 0, SEEK_SET);
    while((fscanf(fp, "%d,%d", &type, &page_id))==2){
        bmgr.accessPage(page_id, type);
    }
    bmgr.print_statistical_data();

    BMgr bmgr2(1);
    fseek(fp, 0, SEEK_SET);
    while((fscanf(fp, "%d,%d", &type, &page_id))==2){
        bmgr2.accessPage(page_id, type);
    }
    bmgr2.print_statistical_data();

    return 0;
}