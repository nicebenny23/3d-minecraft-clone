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

safefile::safefile(std::string filepath, mode openmode)
{
    fp = nullptr;
    offset = 0;
    optype = openmode;
    switch (openmode)
    {
    case filewrite:
        fp = fopen(filepath.c_str(), "wb");
        break;
    case fileread:
        fp = fopen(filepath.c_str(), "rb");
        break;
    case fileappend:
        fp = fopen(filepath.c_str(), "ab");
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
		fp = nullptr;
	}
    
}

void safefile::go(unsigned int byteoffset)
{
    if (fseek(fp, byteoffset, SEEK_SET) != 0) {
        throw std::runtime_error("Failed to fseek to offset");
    }
    offset = byteoffset;
}


bool fileexists(std::string name)
{
    FILE* file = fopen(name.c_str(), "r");
    if (file) {
        fclose(file);
        return true;
    }
    else {
        return false;
    }
}
