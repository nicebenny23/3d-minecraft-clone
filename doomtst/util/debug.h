#include <iostream>
#include <string>
#ifndef debug_HPP
#define debug_HPP
//good
inline void Assert(char* msg) {

	std::cout << '/n'<<msg;
	std::abort();
}
inline void Assert(const char* msg) {

	std::cout << '/n' << msg;
	std::abort();
}



constexpr bool debuggeneral = true;
constexpr bool debugrender = true;
	void reset();

	
	void writetodebug(std::string string);







#endif // !debug_HPP
