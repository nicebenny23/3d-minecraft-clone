#include "iostream"
#include <Windows.h>
#include "dynamicarray.h"
#include "vector2.h"
#include <glm/glm.hpp>
#ifndef Userinput_HPP
#define Userinput_HPP
#define esckey GLFW_KEY_ESCAPE
#define shiftkey GLFW_KEY_LEFT_SHIFT
namespace userinput {

	extern glm::vec2 mouseposdt;
	extern glm::vec2 mousepos;
	extern v2::Vector2 normedmousepos;
		struct inputkey {
		bool held;
		bool released;
		bool pressed;
		bool xorheld(){

			return held && !released;
		}
     		inputkey() {

			held = false;
			pressed = false;
			released = false;
	     	}
	};
		extern inputkey mouseleft;
	
		extern inputkey mouseright;
	void initiate();

	void updatekey(int code, int pressedorrelesed);
	void updateotherkeys(int button, int pressedorrelesed);
	void endupdate();
	//inputkey Getkey(char keyval)
	
	int convertchartoglfwkey(const int key);

		inputkey getinputkey(const int key);
	



}


#endif // !userinp

