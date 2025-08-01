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

inline fs::path createUniqueNamedFolder(const fs::path& basePath, const std::string& baseName) {
	if (!fs::exists(basePath)) {
		fs::create_directories(basePath);
	}

	int folderNumber = 1;
	fs::path folderPath;

	do {
		folderPath = basePath / (baseName+std::to_string(folderNumber));
		folderNumber++;
	} while (fs::exists(folderPath));

	fs::create_directories(folderPath);
	return folderPath;
}
inline void createFolder(const fs::path& basePath, const std::string& baseName) {
	if (!fs::exists(basePath)) {
		fs::create_directories(basePath);
	}

	fs::create_directories(basePath/baseName);

}
inline fs::path getWindowsHomeDir() {
	const char* homeDrive = std::getenv("HOMEDRIVE");
	const char* homePath = std::getenv("HOMEPATH");

	if (homeDrive && homePath) {
		return fs::path(std::string(homeDrive) + std::string(homePath));
	}

	// Fallback: try USERPROFILE env var (usually full home path)
	const char* userProfile = std::getenv("USERPROFILE");
	if (userProfile) {
		return fs::path(userProfile);
	}

	throw std::logic_error("Could not find home directory");
	
}

bool fileexists(std::string name);


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
	template<typename... Args>
	void fscanf(size_t expectedargs, const char* format, Args&&... args) {
			int result = std::fscanf(fp, format, std::forward<Args>(args)...);
			if (result != static_cast<int>(expectedargs)) {
				throw std::runtime_error("Unexpected number of matched arguments in fscanf");
			}
		
	}
	safefile(const char* filepath, mode openmode);
	safefile(std::string filepath, mode openmode);
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
