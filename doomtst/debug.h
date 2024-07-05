#include <iostream>
#ifndef debug_HPP
#define debug_HPP

inline void Assert(char* msg) {

	std::cout << '/n'<<msg;
	std::abort();
}
inline void Assert(const char* msg) {

	std::cout << '/n' << msg;
	std::abort();
}

#endif // !debug_HPP
