#include "iostream"
#include <Windows.h>
#include "dynamicarray.h"
#include "vector2.h"
#include <glm/glm.hpp>
#pragma once 
#define esckey GLFW_KEY_ESCAPE
#define shiftkey GLFW_KEY_LEFT_SHIFT

#define Mouse_leftindex GLFW_KEY_LAST+ GLFW_MOUSE_BUTTON_LEFT
#define Mouse_rightindex GLFW_KEY_LAST + GLFW_MOUSE_BUTTON_RIGHT
#define Extra_keys 2
namespace userinput {

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
	struct  InputManager
	{
		Cont::array<inputkey, true> keylist;


		v2::Vector2 mouseposdt;
		v2::Vector2 mousepos;
		v2::Vector2 normedmousepos;
		inputkey mouseleft();

		inputkey mouseright();

		InputManager();

		void updatekey(int code, int pressedorrelesed);
		void endupdate();

		int convertchartoglfwkey(const int key);

		inputkey getKey(const int key);

	};


}


 // !userinp

