#include "debug.h"

#pragma warning(disable: 4996)//will not compile without
void reset()
{

    FILE* fp = fopen("debug.txt", "w");
    if (fp == NULL)
    {
        Assert("debug.txt is null");
    }
    fclose(fp);
}

void writetodebug(std::string string)
{
    if (debuggeneral)
    {


        FILE* fp = fopen("debug.txt", "a+");
        if (fp == NULL)
        {
            Assert("debug.txt is null");
        }
        for (int i = 0; i < string.length(); i++)
        {
            fputc(int(string[i]), fp);
        }
        fclose(fp);
    }
}
