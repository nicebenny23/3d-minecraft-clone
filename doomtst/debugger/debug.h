#include <iostream>
#include "../renderer/Window.h"
#include <process.h>
#include "console.h"
#include "../util/dynamicarray.h"// For _beginthread
#include <cstdlib>
#include <csignal>
#pragma once 
//good
constexpr bool debuggeneral = true;
constexpr bool debugrender = false;
constexpr bool generateflat = false;
constexpr bool debugnodeath = false;
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
}inline void Assert(bool tst,const char* comment)
{
	if (tst)
	{
		return;
	}
	// Launch the message box in a completely separate process
	_beginthread(ShowAssertMessageBox, 0, (void*)comment);

	// Immediately terminate the main program
	HANDLE mainThread = OpenThread(THREAD_TERMINATE, FALSE, GetCurrentThreadId());
	TerminateThread(mainThread, 0);
	CloseHandle(mainThread);
}
void reset();


void writetodebug(std::string string);







 // !debug_HPP