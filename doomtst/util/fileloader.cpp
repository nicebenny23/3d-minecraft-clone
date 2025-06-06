#include "fileloader.h"


long safefile::getsize()
{


    long sizeval;


    movetoend();

    // Get current position of the file pointer (which is the file size)
    sizeval = ftell(fp);
    go(0);
    // Close the file

    return sizeval;
}

void safefile::movetoend()
{

    if (fseek(fp, 0, SEEK_END) != 0) {


        Assert("cannot find end");
    }
    offset = size;
}

int safefile::fscanf(const char* format, ...)
{
    if (fp == nullptr) {

        Assert("file is not defined yet cant use fscanf");
    }
    va_list args;
    va_start(args, format);
    //fscanf(meshfile.fp, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
// Call the original fscanf function and pass the file and format
    int result = vfscanf(fp, format, args);

    va_end(args);
    return result;
}

void safefile::fscanf(int expectedargs, const char* format, ...)
{
    va_list args;
    va_start(args, format);

    int result = vfscanf(fp, format, args); // Use the class's FILE* member
    va_end(args);

    if (expectedargs != result) {
        Assert("Unexpected number of matched arguments in fscanf");
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
        Assert("No valid file acess type given");
        break;
    }

    if (!fp) {
        fprintf(stderr, strerror(errno));
        debug("Failed to open file");


        Assert(filepath);

    }
    if (canread(optype) || canappend(optype))
    {
        size = this->getsize();
    }
    else
    {
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
        Assert("No valid file acess type given");
        break;
    }

    if (!fp) {
        debug("Failed to open file");
        Assert(filepath);

    }
    if (canread(optype))
    {
        size = this->getsize();
    }
    else
    {
        size = 0;
    }

}


void  safefile::close()
{
    if (fp != nullptr)
    {
        fclose(fp);
    }
    else
    {
        Assert("unitilized files can't be closed");
    }
}

void safefile::go(unsigned int byteoffset)
{

    if (fseek(fp, byteoffset, SEEK_SET) != 0) {
        
        Assert("Failed to fseek to offset");
        fclose(fp);

    }

    offset = byteoffset;
}

 bool fileexists(const char* name)
{
    FILE* file;
    file = fopen(name, "r");
    if (file) {

        fclose(file);
        return true;
    }
    else {
        return false;
    }
}
