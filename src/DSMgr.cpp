#include "DSMgr.h"

DSMgr::DSMgr(){
    num_pages=0;
}

DSMgr::DSMgr(std::string filename){
    OpenFile(filename);
}

DSMgr::~DSMgr(){
    CloseFile();
}

int DSMgr::OpenFile(std::string filename){
    curr_file = fopen(filename.c_str(), "r+");
    if(curr_file==NULL){    //文件不存在，创建文件
        curr_file = fopen(filename.c_str(), "w+");
    }
    fseek(curr_file, 0L, SEEK_END);
    long size = ftell(curr_file);       //获得文件大小
    num_pages = size / PAGE_SIZE;

    return curr_file!=NULL; //返回是否打开文件成功
}

int DSMgr::CloseFile(){
    return fclose(curr_file);
}

FILE * DSMgr::GetFile(){
    return curr_file;
}

int DSMgr::Seek(int offset, int pos){
    return fseek(curr_file, (long)offset, pos);
}

void DSMgr::ReadPage(int page_id, bFrame *frm){
    if (Seek(page_id * PAGE_SIZE, SEEK_SET)) {
        fprintf(stderr, "Error: DSMgr ReadPage cannot find page: %d\n", page_id);
        exit(1);
    }
    fread((void *)frm, 1, PAGE_SIZE, curr_file);
}

void DSMgr::WritePage(int page_id, bFrame *frm){
    if (Seek(page_id * PAGE_SIZE, SEEK_SET)) {
        fprintf(stderr, "Error: DSMgr WritePage cannot find page: %d\n", page_id);
        exit(1);
    }
    fwrite((void *)frm, 1, PAGE_SIZE, curr_file);
}

void DSMgr::IncNumPages(){
    num_pages++;
}

int DSMgr::GetNumPages(){
    return num_pages;
}

int DSMgr::NewPage() {
    char buf[PAGE_SIZE];    //随机初始化
    IncNumPages();
    fseek(curr_file, 0L, SEEK_END);
    fwrite(buf, 1, PAGE_SIZE, curr_file);
    return GetNumPages()-1;
}