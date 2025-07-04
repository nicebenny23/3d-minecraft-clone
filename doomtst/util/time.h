
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
			ElapsedTime = glfwGetTime();
			real_dt = 1 / 60.f;

		}
		float ElapsedTime;
		float real_dt;
		float dt;		
		float fps;
		
		void calcfps() {

			float CurrentTime= glfwGetTime();
			 real_dt = CurrentTime - ElapsedTime;
			ElapsedTime = CurrentTime;

		
			fps = 1.f / real_dt;
			float inter_rate = .03;
			dt = clamp(0.f, lerp(dt, real_dt,inter_rate), 1.0f / min_frames);
		}
	private:

		const int min_frames = 30;
	};
}