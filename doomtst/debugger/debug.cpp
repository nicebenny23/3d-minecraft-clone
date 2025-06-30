#pragma once
#include "debug.h"

#pragma warning(disable: 4996)
void ShowAssertMessageBox(void* lpParam)
{
    const char* comment = (const char*)lpParam;

    MessageBoxA(NULL, comment, "Assertion Failed", MB_ICONERROR | MB_OK | MB_SYSTEMMODAL);

    // Keep this thread alive to prevent automatic closure
    while (true)
    {
        Sleep(100);
    }
}
//will not compile without
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
