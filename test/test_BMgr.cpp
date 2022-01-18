#include "BMgr.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <iostream>
#include "cmdline.h"

int main(int argc, char *argv[]){
    cmdline::parser cmd;
    cmd.add("lru", 'l', "use LRU replace algorithm");
    cmd.add("clock", 'c', "use clock replace algorithm");
    cmd.add("random", 'r', "use random replace algorithm");
    cmd.add<std::string>("tracefile", 'f', "trace file name", true, "../data-5w-50w-zipf.txt");
    cmd.parse_check(argc, argv);

    std::string trace_file = cmd.get<std::string>("tracefile");
    // const char *trace_file = "../data-5-12-for-4-LRU.txt";
    FILE *fp;
    if(!(fp = fopen(trace_file.c_str(), "r"))){
        fprintf(stderr, "ERROR: open %s filed\n", trace_file.c_str());
        exit(1);
    }

    BMgr *bmgr;
    if(cmd.exist("lru")){
        bmgr = new BMgr("data.dbf", ReplaceAlg::LRU);
    }else if(cmd.exist("clock")){
        bmgr = new BMgr("data.dbf", ReplaceAlg::Clock);
    }else if(cmd.exist("random")){
        bmgr = new BMgr("data.dbf", ReplaceAlg::Random);
    }else{
        fprintf(stderr, "ERROR: not existed replace algorithm\n");
        exit(1);
    }

    int type, page_id;
    int cnt = 0;

    //创建50000个页的文件
    for(int i=0; i<50000; i++){
        bmgr->FixNewPage(&page_id);     //创建新页，需要写入文件
        bmgr->UnfixPage(page_id);       //删除后，页只存在文件中，不存在于buffer中
    }

    long t = -clock();
    fseek(fp, 0, SEEK_SET);
    while((fscanf(fp, "%d,%d", &type, &page_id))==2){
        bmgr->FixPage(page_id, type);   //访问页
        // printf("cnt: %d\n", cnt++);
    }
    t += clock();
    bmgr->print_statistical_data();
    printf("time elapsed=%.6f\n", (double)t/CLOCKS_PER_SEC);

    fclose(fp);
    return 0;
}