#pragma once
#ifndef window_HPP
#define window_HPP
#include <glad/glad.h>
#include <GLFW/glfw3.h>
namespace window {



	void setcursor(bool mode);

void		createwindow(int scrwidth, int scrheight);
void swapbuffer();


void processInput();

bool shouldclose();
	
	
}


#endif // !window
