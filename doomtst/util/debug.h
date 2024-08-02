#include <iostream>
#include <string>
#ifndef debug_HPP
#define debug_HPP
//good
constexpr bool debuggeneral = true;
constexpr bool debugrender = false;
constexpr bool generateflat = false;
constexpr bool debugnodeath = false;

inline void Assert(char* msg) {

	std::cout << '\n' << msg;
	std::abort();
}
inline void preAssert(char* msg) {

	std::cout << '\n' << msg;

}inline void preAssert(const char* msg) {

	std::cout << '\n' << msg;

}
inline void Assert(const char* msg) {

	std::cout << '\n' << msg;
    	std::abort();
}




void reset();


void writetodebug(std::string string);







#endif // !debug_HPP