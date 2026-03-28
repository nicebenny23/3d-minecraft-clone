#include "ecs/ecs.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../math/mathutil.h"
#include "Core.h"
#pragma once
namespace timename {
	using time_delay = double;
	struct time {
		double value;

		explicit time(double val) : value(val) {
		}
		time() {
			value = 0;
		}
		double dist(const time& oth) const {
			return std::fabs(oth.value - value);
		}

		bool operator<(const time& oth) const {
			return value < oth.value;
		}
		bool operator==(const time& oth) const {
			return value == oth.value;
		}

		time operator+(double offset) const {
			return time(value + offset);
		}
		time operator-(double offset) const {
			return time(value - offset);
		}

		time_delay operator-(time offset) const {
			return value - offset.value;
		}

	};

	struct Duration;
	struct TimeManager :ecs::resource {
		double real_dt;

		double dt;
		double smooth_fps;
		double elapsed_time;

		TimeManager() {

			dt = 0;
			elapsed_time = glfwGetTime();
			real_dt = 1 / 60.f;
			smooth_fps = 0;
			fps_counter = 0;
		}



		void calculate_fps() {

			double CurrentTime = glfwGetTime();
			real_dt = CurrentTime - elapsed_time;
			elapsed_time = CurrentTime;


			double fps = 1.f / real_dt;
			double inter_rate = 1;
			dt = math::clamp(real_dt, 0.L, 1.0L / min_frames);


			double fps_change_rate = .2;
			fps_counter += real_dt;
			if (fps_change_rate < fps_counter) {
				fps_counter = 0;
				double fps_inter_rate = 1;
				smooth_fps = stn::lerp(smooth_fps, fps, fps_inter_rate);

			}
		}
		Duration current_time();

		time now() const{
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
		active = 0,
		ending = 1,
		inactive = 2
	};
	struct Duration {

		Duration(double waiting_time, TimeManager& tman) :tm(tman){
			set(waiting_time);
		}
		Duration(TimeManager& tman) :tm(tman){

		}
		stn::Option<time_delay> remaining() {
			if (end) {
				return stn::max(0, end.unwrap() - tm->now());
			}
			return stn::None;
		}
		void disable() {
			end= stn::None;
		}
		void set(time_delay dur) {
			if (dur <= 0) {
				disable();
			}
			else {
				end = tm->now() + dur;
			}
		}
		bool is_active() const {

			return end.is_some();
		}
		DurationState state() const{

			if (!end) {
				return DurationState::inactive;
			}
			if (tm->now() < end.unwrap()) {
				return DurationState::active;
			}
			end = stn::None;
			return DurationState::ending;

		}
	private:
		mutable stn::Option<time> end;
		stn::non_null<TimeManager> tm;
	};
}