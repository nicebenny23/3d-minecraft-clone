#pragma once
#define _CRT_SECURE_NO_DEPRECATE
#include <fcntl.h>
#include <cstdio>
#include "debug.h"
#include <sys/stat.h>
#include <direct.h>
#ifndef filename_HPP
#define filename_HPP
inline void createdirectory(const char* name) {

	if (_mkdir(name) != 0) {

		Assert("error creating directory");
	}
}
 bool fileexists(const char* name);
	
//DONT USE UNLESS NESSEARY AND CHECKED
inline void deletedirectory(const char* name) {

	if (_rmdir(name) != 0) {

		Assert("error creating directory");
	}
}

enum mode
{
	filewrite,
	fileread,
	fileappend,
};
inline bool canread(mode type) {
	if (type == fileread)
	{
		return true;
	}
	return false;
}
inline bool canwrite(mode type) {
	if (type == filewrite)
	{
		return true;
	}
	return false;
}
inline bool canappend(mode type) {
	if (type == fileappend)
	{
		return true;
	}
	return false;
}
struct safefile
{

	mode optype;
	FILE* fp;
	long size;
	long offset;
	long getsize();
	void movetoend();

	safefile(const char* filepath, mode openmode);
	safefile(char* filepath, mode openmode);
	void close();

	void go(size_t byteoffset);
	template <typename T>
	void write(T* ptr, size_t amt)
	{

		if (canwrite(optype) || canappend(optype))
		{
			size_t writeval = fwrite(ptr, sizeof(T), amt, fp);
			if (writeval != amt)
			{
				Assert("writing failed");
			}
			return;
		}
		else
		{
			Assert("cant write to a file that was not initilized in writing mode");
		}


	}


	template <typename T>
	T* read(size_t amt)
	{

		if (canread(optype))
		{
			T* newarr = (new T[amt]);
			size_t read = fread(newarr, sizeof(T), amt, fp);
			if (read != amt)
			{

				Assert("reading failed");
			}
			return newarr;
		}
		else
		{
			Assert("cant read a file that was not initilized in reading mode");
		}

	}
};
inline void deletefile(const char* name) {

	if (remove(name) != 0)
	{
		Assert("error deleting file");
	}
}
#endif // !file_HPP
