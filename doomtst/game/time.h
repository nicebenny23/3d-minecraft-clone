#include "ecs/ecs.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../math/mathutil.h"
#pragma once
namespace timename {
	struct time {
		double value;

		explicit time(double val) : value(val) {}
		time() 
			{
				value = 0;
			}
		double dist(const time& oth) const {
			return std::fabs(oth.value - value);
		}

		bool operator<(const time& oth) const { return value < oth.value; }
		bool operator==(const time& oth) const { return value == oth.value; }

		time operator+(double offset) const { return time(value + offset); }
		time operator-(double offset) const { return time(value - offset); }

		double operator-(time offset) const { return (value - offset.value); }

	};
	struct duration;
	struct TimeManager:ecs::resource
	{
		double real_dt;

		double dt;
		double smooth_fps;
		double ElapsedTime;

		TimeManager() {

			dt = 0;
			ElapsedTime = glfwGetTime();
			real_dt = 1 / 60.f;
			smooth_fps=0;
			fps_counter = 0;
		}
			
		
		
		void calcfps() {

			double CurrentTime= glfwGetTime();
			 real_dt = CurrentTime - ElapsedTime;
			ElapsedTime = CurrentTime;
			

			double fps = 1.f / real_dt;
			double inter_rate = 1;
			dt = clamp(real_dt,0.f, 1.0f / min_frames);
			
			
			double fps_change_rate = .2;
			fps_counter += real_dt;
			if (fps_change_rate <fps_counter)
			{
				fps_counter = 0;
				double fps_inter_rate =1;
				smooth_fps = lerp(smooth_fps, fps, fps_inter_rate);

			}
		}
		duration create_dur();

		time now() {
			return time(ElapsedTime);
		}
		
	private:
		double fps_counter;
		const int min_frames = 20;
	};

	enum class duration_state {
		active=0,
		ending=1,
		inactive=2
	};
	struct duration {
		duration() :tm(nullptr), active(false) {

		}
		duration(double wait, TimeManager* tman):tm(tman),active(true) {
			set(wait);
		}
		duration(TimeManager* tman) :tm(tman), active(false) {
			
		}
		double remaining() {
			if (!active)
			{
				throw std::logic_error("cannot compute the remaining time of a duration");
			}
			if (!tm)
			{
				throw std::logic_error("cant use unitilized duration");
			}
			return Max(0, end-tm->now());
		}
		void disable() {
			active = false;
		}
		void set(double dur) {
			if (dur<=0)
			{
				throw std::logic_error("duration must wasit for a positive number of time");
			}

			if (!tm)
			{
				throw std::logic_error("cant use unitilized duration");
			}
			end = tm->now() + dur;
			active = true;
			
		}
		bool is_active() const{

			if (!tm)
			{
				throw std::logic_error("cant use unitilized duration");
			}
			return active;
		}
		duration_state state() {

		if (!tm)
		{
			throw std::logic_error("cant use unitilized duration");
		}
		if (!active)
		{
		return duration_state::inactive;
		}
		if (tm->now()< end )
		{
		return duration_state::active;
		}
		active = false;
		return duration_state::ending;

	}
	private:
		time end;
		TimeManager* tm;
		bool active;
	};
}