
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "mathutil.h"
#pragma once
namespace timename {
	struct TimeManager
	{
		TimeManager() {
			
			fps = 60;
			dt = 0;
		}
		
		float ElapsedTime;
		float fps;
		float dt;
		void calcfps() {

			float CurrentTime= glfwGetTime();
			float Roughdt = CurrentTime - ElapsedTime;
			ElapsedTime = CurrentTime;

		
			fps = 1.f / Roughdt;
			dt = clamp(0.f, Roughdt, 1.0f / MaxFrames);
		}
	private:

		const int MaxFrames = 2000;
	};
}