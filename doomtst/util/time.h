
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "mathutil.h"
#pragma once
namespace timename {
	struct TimeManager
	{
		TimeManager() {
			
			dt = 0;
			ElapsedTime = glfwGetTime();
			real_dt = 1 / 60.f;

		}
		float ElapsedTime;
		float real_dt;
		float dt;		
		
		float smooth_fps;
		void calcfps() {

			float CurrentTime= glfwGetTime();
			 real_dt = CurrentTime - ElapsedTime;
			ElapsedTime = CurrentTime;
			

			float fps = 1.f / real_dt;
			float inter_rate = 1;
			dt = clamp(0.f, lerp(dt, real_dt,inter_rate), 1.0f / min_frames);
			
			
			float fps_change_rate = .2;
			fps_counter += real_dt;
			if (fps_change_rate <fps_counter)
			{
				fps_counter = 0;
				float fps_inter_rate = .1;
				smooth_fps = lerp(smooth_fps, fps, fps_inter_rate);

			}
		}
	private:
		float fps_counter;
		const int min_frames = 20;
	};
}