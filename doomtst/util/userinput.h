#include "iostream"
#include <Windows.h>
#include "dynamicarray.h"
#include "vector2.h"
#include <glm/glm.hpp>
#ifndef Userinput_HPP
#define Userinput_HPP

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
		extern inputkey esckey;
		extern inputkey mouseright;
	void initiate();

	void updatekey(int code, int pressedorrelesed);
	void updateotherkeys(int button, int pressedorrelesed);
	void endupdate();
	//inputkey Getkey(char keyval)
	
	int convertchartoglfwkey(const char key);

		inputkey getinputkey(const char key);
	



}


#endif // !userinp

