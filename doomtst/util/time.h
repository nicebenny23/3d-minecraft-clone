#include <chrono>
#include "dynamicarray.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "mathutil.h"
using namespace dynamicarray;
using namespace std::chrono;
#ifndef timehpp
#define timehpp
namespace timename {
	extern float fps;
	extern float dt;
	extern float gametime;
	extern float tfps;
	extern float averagedt;
	extern float smoothdt;
	void inittime(); 

	inline void calcfps() {

		auto currtime = glfwGetTime();
		dt = currtime - gametime;
		//to prevent erros in debug
		if (averagedt==0)
		{
			averagedt = 1/60.f;
		}
		averagedt = averagedt * .95f+ Min( dt,.2f)* .05f;
		smoothdt =Min(averagedt, .05f);
		
		fps = 1000 / smoothdt;

		gametime = currtime;
	}

}
#endif // !timehpp#pragma once
