#pragma once
#include "fileloader.h"
#include <cerrno>
#include <cstring>

long safefile::getsize()
{
    long sizeval;
    movetoend();
    sizeval = ftell(fp);
    go(0);
    return sizeval;
}

void safefile::movetoend()
{
    if (fseek(fp, 0, SEEK_END) != 0) {
        throw std::runtime_error("cannot find end");
    }
    offset = size;
}

int safefile::fscanf(const char* format, ...)
{
    if (fp == nullptr) {
        throw std::runtime_error("file is not defined yet, can't use fscanf");
    }
    va_list args;
    va_start(args, format);
    int result = vfscanf(fp, format, args);
    va_end(args);
    return result;
}

void safefile::fscanf(int expectedargs, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    int result = vfscanf(fp, format, args);
    va_end(args);

    if (expectedargs != result) {
        throw std::runtime_error("Unexpected number of matched arguments in fscanf");
    }
}

safefile::safefile(const char* filepath, mode openmode)
{
    fp = nullptr;
    offset = 0;
    optype = openmode;
    switch (openmode)
    {
    case filewrite:
        fp = fopen(filepath, "wb");
        break;
    case fileread:
        fp = fopen(filepath, "rb");
        break;
    case fileappend:
        fp = fopen(filepath, "ab");
        break;
    default:
        throw std::runtime_error("No valid file access type given");
    }

    if (!fp) {
        fprintf(stderr, "%s\n", strerror(errno));
        debug("Failed to open file");
        throw std::runtime_error(filepath);
    }

    if (canread(optype) || canappend(optype)) {
        size = this->getsize();
    }
    else {
        size = 0;
    }
}

safefile::safefile(char* filepath, mode openmode)
{
    fp = nullptr;
    offset = 0;
    optype = openmode;
    switch (openmode)
    {
    case filewrite:
        fp = fopen(filepath, "wb");
        break;
    case fileread:
        fp = fopen(filepath, "rb");
        break;
    case fileappend:
        fp = fopen(filepath, "ab");
        break;
    default:
        std::cout << fp;
        throw std::runtime_error("No valid file access type given");
    }

    if (!fp) {
        debug("Failed to open file");
        throw std::runtime_error(filepath);
    }

    if (canread(optype)) {
        size = this->getsize();
    }
    else {
        size = 0;
    }
}

void safefile::close()
{
    if (fp != nullptr) {
        fclose(fp);
    }
    else {
        throw std::runtime_error("uninitialized files can't be closed");
    }
}

void safefile::go(unsigned int byteoffset)
{
    if (fseek(fp, byteoffset, SEEK_SET) != 0) {
        throw std::runtime_error("Failed to fseek to offset");
    }
    offset = byteoffset;
}

bool fileexists(const char* name)
{
    FILE* file = fopen(name, "r");
    if (file) {
        fclose(file);
        return true;
    }
    else {
        return false;
    }
}
