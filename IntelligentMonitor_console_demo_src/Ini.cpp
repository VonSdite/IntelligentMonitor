#include "Ini.h"

ReadIni::ReadIni(char * file_name)
{
    strcpy(m_file_name, file_name);
}

char * ReadIni::GetIniKeyString(char * title, char * key)
{
    FILE *fp;
    int  flag = 0;
    char str_section[128], *wTmp;
    static char str_line[2048];
    char msg[2048];

    sprintf(str_section, "[%s]", title);
    if (NULL == (fp = fopen(m_file_name, "r")))
    {
        sprintf(msg, "打开文件失败: %s", m_file_name);
        exit(EXIT_FAILURE);
    }

    while (NULL != fgets(str_line, 2048, fp))
    {
        if (0 == strncmp("//", str_line, 2) || '#' == str_line[0])
            continue;

        wTmp = strchr(str_line, '=');
        if ((NULL != wTmp) && (1 == flag))
        {
            if (0 == strncmp(key, str_line, wTmp - str_line))
            {
                str_line[strlen(str_line) - 1] = '\0';
                fclose(fp);
                return wTmp + 1;
            }
        }
        else if (0 == strncmp(str_section, str_line, strlen(str_line) - 1))
            flag = 1;
        else
            flag = 0;
    }
    fclose(fp);
    sprintf(msg, "找不到 [%s][%s]的配置 ", title, key);
    exit(EXIT_FAILURE);
}

int ReadIni::GetIniKeyInt(char * title, char * key)
{
    return atoi(GetIniKeyString(title, key));
}
