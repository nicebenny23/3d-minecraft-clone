#include "userinput.h"
#include <GLFW/glfw3.h>
namespace userinput {
	glm::vec2 mousepos;
	glm::vec2 mouseposdt;
	inputkey mouseleft;
	inputkey mouseright;
	dynamicarray::array<inputkey> keylist;

	void endupdate(){
		mouseposdt = glm::vec2(0, 0);
		for (int i = 0; i < keylist.capacity; i++)
		{
			keylist[i].pressed = false;
			keylist[i].relesed = false;
		}
		mouseleft.pressed = false;
		mouseleft.relesed = false;
		mouseright.pressed = false;
		mouseright.relesed = false;
	}

	void initiate()
	{
		mouseleft = inputkey();
		mouseright = inputkey();
		mousepos = glm::vec2(0, 0);
		mouseposdt = glm::vec2(0, 0);
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
			keylist[code].relesed = true;
		}
		
	}

	void updatemousebutton(int button, int pressedorrelesed)
	{
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
				mouseright.relesed = true;
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
				mouseleft.relesed = true;
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