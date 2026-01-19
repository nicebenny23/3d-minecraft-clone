#include "fileloader.h"
#pragma once
namespace stn {
	class FileLineReader {
		stn::file_handle& file;
	public:
		FileLineReader(stn::file_handle& f) : file(f) {
			file.seek(SeekOrigin::Begin, 0);
		}
		struct iterator {
			stn::file_handle* file;
			std::string line;

			iterator& operator++() {
				char buf[512];
				if (fgets(buf, sizeof(buf), file->fp)) {
					line = buf;
				}
				else {
					file = nullptr;
				}
				return *this;
			}

			const std::string& operator*() const {
				return line;
			}
			bool operator!=(const iterator& other) const {
				return file != other.file;
			}
		};

		iterator begin() {
			return ++iterator{ &file, {} };
		}
		iterator end() {
			return iterator{ nullptr, {} };
		}
	};

}