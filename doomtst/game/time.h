#include "ecs/ecs.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../math/mathutil.h"
#include "Core.h"
#include <chrono>

#pragma once
namespace timing {
	using time_delay = double;
	struct Clock {
		double dt=1/60.0;
		double elapsed_time=0;
		void add(double delta) {
			dt = delta;
			elapsed_time += dt;
		}
		
		double fps() const{
			return 1 / dt;
		}
	};
	struct Duration;
	struct GlobalClock:ecs::resource {

		Clock clock;
		double smooth_dt;
		double elapsed_time() const {
			return clock.elapsed_time;
		}
		double dt() const {
			return clock.dt;
		}
		GlobalClock() {
			clock.dt = 1 / 60.f;

			smooth_dt = 1 / 60.0f;
			clock.elapsed_time= glfwGetTime();
		}
		double fps() const {
			return 1 / smooth_dt;
		}

		void calculate_fps() {

			double current_time = glfwGetTime();
			clock.dt = stn::min(current_time - clock.elapsed_time, 1.0f / min_frames);
			double update_speed = .2f;
			if (.05< clock.dt) {
				int l = 3;
			}
			if (std::floor(clock.elapsed_time / update_speed) != std::floor(current_time / update_speed)) {
				smooth_dt = clock.dt;
			}
			clock.elapsed_time= current_time;

		}

		double now() const {
			return clock.elapsed_time;
		}

	private:
		const int min_frames = 2;
	};
	struct FpsTimer :ecs::System {

		void run(ecs::Ecs& world) {
			world.ensure_resource<GlobalClock>().calculate_fps();
		}

	};
	struct TimePlugin {
		void operator()(core::App& app) {
			app.emplace_resource<timing::GlobalClock>();
			app.emplace_system<FpsTimer>();
		}
	};
	struct Duration {

		Duration(double waiting_time, GlobalClock& tman) :tm(tman.clock) {
			set(waiting_time);
		}

		Duration(GlobalClock& clock) :tm(clock.clock) {

		}
		Duration(Clock& clock) :tm(clock) {

		}

		Duration(double waiting_time,Clock& tman) :tm(tman) {
			set(waiting_time);
		}
		stn::Option<time_delay> remaining() {
			check_if_dead();
			if (end) {
				return end.unwrap() - tm->elapsed_time;
			}
			return stn::None;
		}

		double remaining_or_default() {
			return remaining().unwrap_or_default();
		}
		Clock& clock() {
			return *tm;
		}
		const Clock& clock() const {
			return *tm;
		}
		void disable() {
			end = stn::None;
		}
		void set(time_delay dur) {
			if (dur <= 0) {
				disable();
			}
			else {
				end = tm->elapsed_time + dur;
			}
		}
		//checks for inactivity then sets;
		bool is_inactive_set(float amount) {
			if (is_inactive()) {
				set(amount);
				return true;
			}
			return false;
		}
		bool is_active() const {
			check_if_dead();
			return end.is_some();
		}
		bool is_inactive() const {
			return !is_active();
		}
		stn::Option<double> end_time() const {
			return end;
		}
	private:

		void check_if_dead() const {
			if (end.is_some_and([&](double end) {
				return end < tm->elapsed_time; })) {
				end = stn::None;
			}
		}
		mutable stn::Option<double> end;
		stn::non_null <Clock> tm;
	};
	
	struct TimeProfiler {
		using clock = std::chrono::high_resolution_clock;
		std::chrono::time_point<clock> start;

		TimeProfiler() {
			reset();
		}

		double seconds() const{
			auto now = clock::now();
			std::chrono::duration<double> elapsed = now - start;
			return elapsed.count();
		}
		void reset() {
			start = clock::now();
		}
		double milliseconds() const {
			return 1000 * seconds();
		}
	};
}