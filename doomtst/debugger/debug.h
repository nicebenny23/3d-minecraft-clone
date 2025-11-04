#pragma once 

#include <iostream>
#include "../renderer/Window.h"
#include <process.h>
#include "console.h"
#include "../util/dynamicarray.h"// For _beginthread
#include <cstdlib>
#include <csignal>
//good
constexpr bool debuggeneral = true;
constexpr bool debugrender = false;
constexpr bool generateflat = true;
constexpr bool debugnodeath = false;

constexpr bool debug_slow = false;

void ShowAssertMessageBox(void* lpParam);


// Assert function that terminates the main program but keeps the message box open
[[noreturn]] inline void Assert(const char* comment)
{
	throw std::logic_error(comment);
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

	throw std::logic_error(comment);
	// Launch the message box in a completely separate process
	_beginthread(ShowAssertMessageBox, 0, (void*)comment);

	// Immediately terminate the main program
	HANDLE mainThread = OpenThread(THREAD_TERMINATE, FALSE, GetCurrentThreadId());
	TerminateThread(mainThread, 0);
	CloseHandle(mainThread);
}
void clear();


void writetodebug(std::string string);







 // !debug_HPP