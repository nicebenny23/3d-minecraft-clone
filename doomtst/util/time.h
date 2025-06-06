#include <chrono>
#include "dynamicarray.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "mathutil.h"
using namespace dynamicarray;
using namespace std::chrono;
#pragma once
namespace timename {
	struct TimeManager
	{
		TimeManager() {
			realtime = 0;
			fps = 60;
			dt = 0;
		}
		float fps;
		float dt;
		float realtime;
		void calcfps() {

			auto currtime = glfwGetTime();
			float roughdt = currtime - realtime;
			//to prevent erros in debug
			float speedchange = .05;
			//slowly change and clamp it so it does not get out of control also we control for timescale
			dt = clamp(.0001f, lerp(dt, roughdt, speedchange), .05f);

			fps = 1.f / roughdt;

			realtime = currtime;
		}
	};
}