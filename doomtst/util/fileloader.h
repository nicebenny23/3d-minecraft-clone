#pragma once
#define _CRT_SECURE_NO_DEPRECATE

#include <cstdio>
#include <cstdarg>
#include <string>
#include <iostream>
#include <filesystem>
#include <direct.h>
#include <stdexcept>

#include "../debugger/debug.h"

namespace fs = std::filesystem;

inline void deleteFilesInFolder(const std::string& folderPath) {
	for (const auto& entry : fs::directory_iterator(folderPath)) {
		if (entry.is_regular_file()) {
			fs::remove(entry.path());
		}
	}
}

inline void createdirectory(const char* name) {
	if (_mkdir(name) != 0) {
		throw std::runtime_error("error creating directory");
	}
}

bool fileexists(const char* name);

// DONT USE UNLESS NECESSARY AND CHECKED
inline void deletedirectory(const char* name) {
	if (_rmdir(name) != 0) {
		throw std::runtime_error("error deleting directory");
	}
}

enum mode {
	filewrite,
	fileread,
	fileappend,
};

inline bool canread(mode type) {
	return (type == fileread);
}

inline bool canwrite(mode type) {
	return (type == filewrite);
}

inline bool canappend(mode type) {
	return (type == fileappend);
}

struct safefile {
	mode optype;
	FILE* fp;
	long size;
	long offset;

	long getsize();
	void movetoend();
	int fscanf(const char* format, ...);
	void fscanf(int expectedargs, const char* format, ...);

	safefile(const char* filepath, mode openmode);
	safefile(char* filepath, mode openmode);
	void close();

	void go(unsigned int byteoffset);

	template <typename T>
	void write(T* ptr, size_t amt)
	{
		if (canwrite(optype) || canappend(optype))
		{
			size_t writeval = fwrite(ptr, sizeof(T), amt, fp);
			if (writeval != amt)
			{
				std::cout << "only " << writeval << " out of " << amt << " elems were written";
				throw std::runtime_error("writing failed");
			}
			return;
		}
		else
		{
			throw std::runtime_error("can't write to a file that was not initialized in writing mode");
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
				debug("reading failed");
				debug("attempted to read a file but only");
				std::cout << read << " out of " << amt << " elements were read";
				throw std::runtime_error("reading failed");
			}
			return newarr;
		}
		else
		{
			throw std::runtime_error("can't read a file that was not initialized in reading mode");
		}
	}
};

inline void deletefile(const char* name) {
	if (remove(name) != 0)
	{
		throw std::runtime_error("error deleting file");
	}
}
