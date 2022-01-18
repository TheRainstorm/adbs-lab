#ifndef DSMGR_H
#define DSMGR_H

#include <iostream>
#include "defines.h"

//directory based file format

class DSMgr
{
public:
    DSMgr();
    DSMgr(std::string filename);
    ~DSMgr();
    int OpenFile(std::string filename);
    int CloseFile();
    FILE *GetFile();
    int Seek(int offset, int pos);                  //将curr_file偏移到指定位置，用于ReadPage和WritePage
    void ReadPage(int page_id, bFrame *frm);        //从文件中读取page_id的页到frame中
    void WritePage(int page_id, bFrame *frm);       //将frame写入到文件中page_id对应的页
    void IncNumPages();         //页计数器加1
    int GetNumPages();          //获得页计数器
    void SetUse(int index, int use_bit);
    int GetUse(int index);

    int NewPage();

private:
    FILE *curr_file;
    int num_pages;
};
#endif