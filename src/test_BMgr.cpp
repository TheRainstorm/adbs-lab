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

    BMgr bmgr;
    int type, page_id;
    int cnt=0;
    while((fscanf(fp, "%d,%d", &type, &page_id))==2){
        bmgr.accessPage(page_id, type);
        // printf("cnt: %d\n", cnt);
        cnt++;
    }
    bmgr.print_statistical_data();

    return 0;
}