#pragma once
#include "userinput.h"
#include <GLFW/glfw3.h>
#include "../debugger/debug.h"
namespace userinput {
	//([0-width],[0,height])
	void InputManager::endupdate(){
		mouseposdt = v2::Vec2(0, 0);
		for (int i = 0; i < keylist.length(); i++)
		{
			keylist[i].pressed = false;
			keylist[i].released = false;
		}
		
	}

	inputkey InputManager::mouseleft()
	{
		return keylist[Mouse_leftindex];
	}

	inputkey InputManager::mouseright()
	{
		return keylist[Mouse_rightindex];
	}

	 InputManager::InputManager()
	{

		mousepos = v2::Vec2(0, 0);
		mouseposdt = v2::Vec2(0, 0);

		keylist = stn::array<inputkey>(GLFW_KEY_LAST+ Extra_keys);
		keylist.reach(GLFW_KEY_LAST + Extra_keys);
	}
	void InputManager::updatekey(int code, int action) {

		if (code< keylist.length())
		{
			keylist[code].update(action);

			
		}
		else
		{
				Assert("error code");
		}
	}


	
	
	int InputManager::convertchartoglfwkey(const int key) {
		if ('a' <= key&&key<='z')
		{
			return key - 32;
		}
	
		return key;

	}

	inputkey InputManager::getKey(const size_t key) {
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