#include "BMgr.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(){
    const char *trace_file = "../data-5w-500000-zipf.txt";
    // const char *trace_file = "../data-5-12-for-4-LRU.txt";
    FILE *fp;
    if(!(fp = fopen(trace_file, "r"))){
        printf("open %s filed\n", trace_file);
        exit(1);
    }

    int type, page_id;
    int cnt = 0;

    long long t;
    t = -clock();
    BMgr bmgr(0);
    fseek(fp, 0, SEEK_SET);
    while((fscanf(fp, "%d,%d", &type, &page_id))==2){
        bmgr.accessPage(page_id, type);
        // printf("cnt: %d\n", cnt++);
    }
    t += clock();
    bmgr.print_statistical_data();
    printf("CLOCKS_PER_SEC=%d\n", CLOCKS_PER_SEC);
    printf("time elapsed=%lld, %f\n",t, (float)t/CLOCKS_PER_SEC);

    t = -clock();
    BMgr bmgr2(1);
    fseek(fp, 0, SEEK_SET);
    while((fscanf(fp, "%d,%d", &type, &page_id))==2){
        bmgr2.accessPage(page_id, type);
    }
    t += clock();
    bmgr2.print_statistical_data();
    printf("time elapsed=%lld, %f\n",t, (float)t/CLOCKS_PER_SEC);

    t = -clock();
    BMgr bmgr3(2);
    fseek(fp, 0, SEEK_SET);
    while((fscanf(fp, "%d,%d", &type, &page_id))==2){
        bmgr3.accessPage(page_id, type);
    }
    t += clock();
    bmgr3.print_statistical_data();
    printf("time elapsed=%lld, %f\n",t, (float)t/CLOCKS_PER_SEC);
    
    fclose(fp);
    return 0;
}