#include "iostream"
#include <Windows.h>
#include "dynamicarray.h"
#include "vector2.h"
#include <glm/glm.hpp>
#ifndef Userinput_HPP
#define Userinput_HPP
#define esckey GLFW_KEY_ESCAPE
#define shiftkey GLFW_KEY_LEFT_SHIFT

#define Mouse_leftindex GLFW_KEY_LAST+ GLFW_MOUSE_BUTTON_LEFT
#define Mouse_rightindex GLFW_KEY_LAST + GLFW_MOUSE_BUTTON_RIGHT
#define Extra_keys 2
namespace userinput {
	
	extern v2::Vector2 mouseposdt;
	extern v2::Vector2 mousepos;
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

		void update(int action);
	};
		inputkey mouseleft();
	
		inputkey mouseright();
	void initiate();

	void updatekey(int code, int pressedorrelesed);
	void endupdate();
	
	int convertchartoglfwkey(const int key);

		inputkey getinputkey(const int key);
	



}


#endif // !userinp

