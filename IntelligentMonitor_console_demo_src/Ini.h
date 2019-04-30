#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <cstring>

class ReadIni
{
public:
    ReadIni(char *file_name);

    /*
    ini配置文件读取函数
    等号左右两边不要使用空格
    每行不要以空格开头
    */
    char *GetIniKeyString(char *title, char *key);
    int GetIniKeyInt(char *title, char *key);

private:
    char m_file_name[2048];
};
