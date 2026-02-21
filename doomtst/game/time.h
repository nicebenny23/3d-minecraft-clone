#include "ecs/ecs.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../math/mathutil.h"
#include "Core.h"
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
	struct Duration;
	struct TimeManager:ecs::resource
	{
		double real_dt;

		double dt;
		double smooth_fps;
		double elapsed_time;

		TimeManager() {

			dt = 0;
			elapsed_time = glfwGetTime();
			real_dt = 1 / 60.f;
			smooth_fps=0;
			fps_counter = 0;
		}
			
		
		
		void calculate_fps() {

			double CurrentTime= glfwGetTime();
			 real_dt = CurrentTime - elapsed_time;
			elapsed_time = CurrentTime;
			

			double fps = 1.f / real_dt;
			double inter_rate = 1;
			dt = clamp(real_dt,0.f, 1.0f / min_frames);
			
			
			double fps_change_rate = .2;
			fps_counter += real_dt;
			if (fps_change_rate <fps_counter)
			{
				fps_counter = 0;
				double fps_inter_rate =1;
				smooth_fps = stn::lerp(smooth_fps, fps, fps_inter_rate);

			}
		}
		Duration current_time();

		time now() {
			return time(elapsed_time);
		}
		
	private:
		double fps_counter;
		const int min_frames = 20;
	};
	struct FpsTimer :ecs::System {

		void run(ecs::Ecs& world) {
			world.ensure_resource<TimeManager>().calculate_fps();
		}

	};
	struct TimePlugin :Core::Plugin {
		void build(Core::App& App) {
			App.emplace_resource<timename::TimeManager>();
			App.emplace_system<FpsTimer>();
		}
	};
	enum class DurationState {
		active=0,
		ending=1,
		inactive=2
	};
	struct Duration {
		Duration() :tm(nullptr), active(false) {

		}
		Duration(double waiting_time, TimeManager* tman):tm(tman),active(true) {
			set(waiting_time);
		}
		Duration(TimeManager* tman) :tm(tman), active(false) {
			
		}
		double remaining() {
			if (!active)
			{
				throw std::logic_error("cannot compute the remaining time of a Duration");
			}
			if (!tm)
			{
				throw std::logic_error("cant use unitilized Duration");
			}
			return stn::max(0, end-tm->now());
		}
		void disable() {
			active = false;
		}
		void set(double dur) {
			if (dur<=0)
			{
				throw std::logic_error("Duration must wasit for a positive number of time");
			}

			if (!tm)
			{
				throw std::logic_error("cant use unitilized Duration");
			}
			end = tm->now() + dur;
			active = true;
			
		}
		bool is_active() const{

			if (!tm)
			{
				throw std::logic_error("cant use unitilized Duration");
			}
			return active;
		}
		DurationState state() {

		if (!tm)
		{
			throw std::logic_error("cant use unitilized Duration");
		}
		if (!active)
		{
		return DurationState::inactive;
		}
		if (tm->now()< end )
		{
		return DurationState::active;
		}
		active = false;
		return DurationState::ending;

	}
	private:
		time end;
		TimeManager* tm;
		bool active;
	};
}