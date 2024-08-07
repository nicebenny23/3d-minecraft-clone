#include "userinput.h"
#include <GLFW/glfw3.h>
namespace userinput {
	glm::vec2 mousepos;
	glm::vec2 mouseposdt;
	inputkey mouseleft;
	inputkey mouseright;
	inputkey esckey;
	v2::Vector2 normedmousepos;
	dynamicarray::array<inputkey> keylist;

	void endupdate(){
		mouseposdt = glm::vec2(0, 0);
		for (int i = 0; i < keylist.capacity; i++)
		{
			keylist[i].pressed = false;
			keylist[i].released = false;
		}
		mouseleft.pressed = false;
		mouseleft.released = false;
		mouseright.pressed = false;
		mouseright.released = false;
		esckey.released = false;
		esckey.pressed = false;
	}

	void initiate()
	{
		mouseleft = inputkey();
		mouseright = inputkey();
		mousepos = glm::vec2(0, 0);
		mouseposdt = glm::vec2(0, 0);
		esckey = inputkey();
		keylist = dynamicarray::array<inputkey>(GLFW_KEY_LAST);
		for (int i = 0; i < keylist.capacity; i++)
		{
			keylist[i]=inputkey();
			
		}
	}
	void updatekey(int code, int pressedorrelesed) {

		
		if (pressedorrelesed == GLFW_PRESS)
		{

			keylist[code].pressed = true;
			keylist[code].held = true;

		}
		if (pressedorrelesed == GLFW_RELEASE)
		{
			keylist[code].pressed = false;
			keylist[code].held= false;
			keylist[code].released = true;
		}
		
	}

	void updateotherkeys(int button, int pressedorrelesed)
	{
		if (button == 256)
		{
			if (pressedorrelesed == GLFW_PRESS)
			{

				esckey.pressed = true;
				esckey.held = true;

			}
			if (pressedorrelesed == GLFW_RELEASE)
			{
				esckey.pressed = false;
				esckey.held = false;
				esckey.released = true;
			}
		}
		if (button==GLFW_MOUSE_BUTTON_RIGHT)
		{
			if (pressedorrelesed == GLFW_PRESS)
			{

				mouseright.pressed = true;
				mouseright.held = true;

			}
			if (pressedorrelesed == GLFW_RELEASE)
			{
				mouseright.pressed = false;
				mouseright.held = false;
				mouseright.released = true;
			}
		}if (button == GLFW_MOUSE_BUTTON_LEFT)
		{
			if (pressedorrelesed == GLFW_PRESS)
			{

				mouseleft.pressed = true;
				mouseleft.held = true;

			}
			if (pressedorrelesed == GLFW_RELEASE)
			{
				mouseleft.pressed = false;
				mouseleft.held = false;
				mouseleft.released = true;
			}
		}
	}
	

	
	
	int convertchartoglfwkey(const char key) {
		if ('a' <= key&&key<='z')
		{
			return key - 32;
		}
	
		return key;

	}

	inputkey getinputkey(const char key) {
		return keylist[convertchartoglfwkey(key)];
	}

}