#include "userinput.h"
#include <GLFW/glfw3.h>
namespace userinput {
	//([0-width],[0,height])
	v2::Vector2 mousepos;
	v2::Vector2 mouseposdt;
	//from ([-1,1],[-1,1])
	v2::Vector2 normedmousepos;
	dynamicarray::array<inputkey,true> keylist;

	void endupdate(){
		mouseposdt = v2::Vector2(0, 0);
		for (int i = 0; i < keylist.capacity; i++)
		{
			keylist[i].pressed = false;
			keylist[i].released = false;
		}
		
	}

	inputkey mouseleft()
	{
		return keylist[Mouse_leftindex];
	}

	inputkey mouseright()
	{
		return keylist[Mouse_rightindex];
	}

	void initiate()
	{

		mousepos = v2::Vector2(0, 0);
		mouseposdt = v2::Vector2(0, 0);
	keylist = dynamicarray::array<inputkey,true>(GLFW_KEY_LAST+ Extra_keys);
		for (int i = 0; i < keylist.capacity; i++)
		{
			keylist[i]=inputkey();
			
		}
	}
	void updatekey(int code, int action) {

		if (code< keylist.length)
		{
			keylist[code].update(action);

			
		}
		else
		{
				Assert("error code");
		}
	}


	
	
	int convertchartoglfwkey(const int key) {
		if ('a' <= key&&key<='z')
		{
			return key - 32;
		}
	
		return key;

	}

	inputkey getinputkey(const int key) {
		return keylist[convertchartoglfwkey(key)];
	}

	void inputkey::update(int action)
	{
		if (action == GLFW_PRESS)
		{

		pressed = true;
		held = true;

		}
		if (action == GLFW_RELEASE)
		{
			pressed = false;
			held = false;
			released = true;
		}
	}

}