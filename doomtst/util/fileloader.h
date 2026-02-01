#pragma once
#define _CRT_SECURE_NO_DEPRECATE

#include <cstdio>
#include <cstdarg>
#include <string>
#include <iostream>
#include "data_format.h"
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
		folderPath = basePath / (baseName + std::to_string(folderNumber));
		folderNumber++;
	} while (fs::exists(folderPath));

	fs::create_directories(folderPath);
	return folderPath;
}
inline void createFolder(const fs::path& basePath, const std::string& baseName) {
	if (!fs::exists(basePath)) {
		fs::create_directories(basePath);
	}

	fs::create_directories(basePath / baseName);

}
inline fs::path getWindowsHomeDir() {
	const char* homeDrive = std::getenv("HOMEDRIVE");
	const char* homePath = std::getenv("HOMEPATH");

	if (homeDrive && homePath) {
		return fs::path(std::string(homeDrive) + std::string(homePath));
	}
	const char* userProfile = std::getenv("USERPROFILE");
	if (userProfile) {
		return fs::path(userProfile);
	}

	throw std::logic_error("Could not find home directory");

}
inline void deletefile(const char* name) {
	if (remove(name) != 0) {
		throw std::runtime_error("error deleting file");
	}
}

inline bool fileexists(std::string name) {

	FILE* file = fopen(name.c_str(), "r");
	if (file) {
		fclose(file);
		return true;
	}
	else {
		return false;
	}
}

namespace stn {
	enum class FileMode {
		Write,
		Read,
		ReadWrite,
		WriteBinary,
		ReadBinary,
		ReadWriteBinary

	};

	inline bool is_readable(FileMode mode) {
		switch (mode) {
		case FileMode::Write:
		case FileMode::WriteBinary:
		return false;
		default:
		return true;
		}
	}

	inline bool is_writeable(FileMode mode) {
		switch (mode) {
		case FileMode::Read:
		case FileMode::ReadBinary:
		return false;
		default:
		return true;
		}
	}
	inline bool is_binary(FileMode mode) {
		switch (mode) {
		case FileMode::ReadBinary:
		case FileMode::WriteBinary:
		case FileMode::ReadWriteBinary:
		return true;
		default:
		return false;
		}
	}
	inline const char* file_opener(FileMode mode) {
		switch (mode) {
		case FileMode::Read:        return "r";
		case FileMode::Write:       return "w";
		case FileMode::ReadWrite:   return "w+";
		case FileMode::ReadBinary:       return "rb";
		case FileMode::WriteBinary:      return "wb";
		case FileMode::ReadWriteBinary:  return "w+b";
		}
	}
	enum class SeekOrigin {
		Begin,
		Current,
		End
	};

	struct file_handle {
		FileMode file_options;
		FILE* fp;
		long offset;
		bool readable() const{
			return is_readable(file_options);
		}
		bool writeable() const {
			return is_writeable(file_options);
		}
		bool binary() const {
			return is_binary(file_options);
		}
		size_t size() {
			seek_end();
			auto pos = std::ftell(fp);
			if (pos < 0) {
				stn::throw_logic_error("ftell failed with {}", pos);
			}
			seek(SeekOrigin::Begin, 0);
			return static_cast<std::uint64_t>(pos);
		}
		void seek(SeekOrigin origin, long offset) {
			int loc_type = 0;
			switch (origin) {
			case SeekOrigin::Begin:   loc_type = SEEK_SET; break;
			case SeekOrigin::Current: loc_type= SEEK_CUR; break;
			case SeekOrigin::End:     loc_type = SEEK_END; break;
			}

			if (fseek(fp, offset, loc_type) != 0) {
				throw std::runtime_error("Failed to seek in file");
			}
		}
		void seek_end() {
			seek(SeekOrigin::End, 0);
		}
		template<typename... Args>
		void fscanf(const char* format, Args&&... args) {
			int result = std::fscanf(fp, format, std::forward<Args>(args)...);
			if (result != static_cast<int>(sizeof...(Args))) {
				throw std::runtime_error("Unexpected number of matched arguments in fscanf");
			}

		}
		file_handle(std::string filepath, FileMode open_mode) :file_options(open_mode), fp(fopen(filepath.c_str(), file_opener(open_mode))), offset(0) {
			if (!fp) {
				throw std::runtime_error(filepath);
			}
		}
		file_handle(std::filesystem::path file_path, FileMode open_mode) :file_handle(file_path.string(), open_mode) {

		}

		file_handle(const file_handle&) = delete;
		file_handle& operator=(const file_handle&) = delete;
		file_handle(file_handle&& other) noexcept
			: file_options(other.file_options), fp(other.fp), offset(other.offset) {
			other.fp = nullptr;
		}
		void close() {
			if (fp != nullptr) {
				fclose(fp);
				fp = nullptr;
			}
		}
		~file_handle() {
			close();
		}

		template <typename T>
		void write(stn::span<const T> span) {
			if (writeable()) {
				size_t amount_written = fwrite(span.data(), sizeof(T), span.length(), fp);
				if (amount_written != span.length()) {
					stn::throw_logic_error("attempted to write to a file but only {} out of {} elements were written", amount_written, span.length());
				}
			}
			else {
				throw std::runtime_error("can't write to a file that was not initialized in writing mode");
			}
		}


		template <typename T>
		stn::array<T> read(size_t count) {
			if (!readable()) {
				throw std::runtime_error("can't read a file that was not initialized in reading mode");
			}
			stn::array<T> newarr = stn::array<T>(count);
			size_t read = fread(newarr.data(), sizeof(T), count, fp);
			if (read != count) {
				stn::throw_logic_error("attempted to read a file but only {} out of {} elements were read", read, count);
			}
			return newarr;
		}
		void write_bytes(stn::span<const std::byte> span) {
			if (readable()) {
				size_t amount_written = fwrite(span.data(), sizeof(std::byte), span.length(), fp);
				if (amount_written != span.length()) {
					stn::throw_logic_error("attempted to write to a file but only {} out of {} elements were written", amount_written, span.length());
				}
			}
			else {
				throw std::runtime_error("can't write to a file that was not initialized in writing mode");
			}
		}
		stn::array<std::byte> read_bytes(size_t count) {
			if (!readable()) {
				throw std::runtime_error("can't read a file that was not initialized in reading mode");
			}
			stn::array<std::byte> newarr = stn::array<std::byte>(count);
			size_t read = fread(newarr.data(), sizeof(std::byte), count, fp);
			if (read != count) {
				stn::throw_logic_error("attempted to read a file but only {} out of {} elements were read", read, count);
			}
			return newarr;
		}
		std::string read_text() {
			if (binary()) {
				stn::throw_logic_error("cannot read text from a binary file");
			}
			size_t file_size = size();
			seek(SeekOrigin::Begin, 0);
			std::string result;
			int ch;
			while ((ch = std::fgetc(fp)) != EOF) {
				result.push_back(static_cast<char>(ch));
			}
			return result;
		}
	};

}