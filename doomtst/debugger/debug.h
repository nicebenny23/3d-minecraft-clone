#include <iostream>
#include <windows.h>
#include <process.h>  // For _beginthread
#include <cstdlib>
#include <csignal>
#ifndef debug_HPP
#define debug_HPP
//good
constexpr bool debuggeneral = true;
constexpr bool debugrender = false;
constexpr bool generateflat = false;
constexpr bool debugnodeath = false;

#define debug(msg) std::cout << 'w' << (msg )<< '\n';

#define warn(msg) std::cout << 'y' <<( msg )<< '\n';
#define alert(msg) std::cout << 'r' << (msg )<< '\n';
void ShowAssertMessageBox(void* lpParam);


// Assert function that terminates the main program but keeps the message box open
inline void Assert(const char* comment)
{
	// Launch the message box in a completely separate process
	_beginthread(ShowAssertMessageBox, 0, (void*)comment);
	
	// Immediately terminate the main program
	HANDLE mainThread = OpenThread(THREAD_TERMINATE, FALSE, GetCurrentThreadId());
	TerminateThread(mainThread, 0);
	CloseHandle(mainThread);
}
void reset();


void writetodebug(std::string string);







#endif // !debug_HPP