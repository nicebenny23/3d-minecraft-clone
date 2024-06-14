#include "userinput.h"
#include <GLFW/glfw3.h>
namespace userinput {
	glm::vec2 mousepos;
	glm::vec2 mouseposdt;
	dynamicarray::array<inputkey> keylist;
	void endupdate(){
		mouseposdt = glm::vec2(0, 0);
		for (int i = 0; i < keylist.capacity; i++)
		{
			keylist[i].pressed = false;
			keylist[i].relesed = false;
		}
	}

	void initiate()
	{
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