#include "ecs/ecs.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../math/mathutil.h"
#include "Core.h"
#include <chrono>

#pragma once
namespace timing {
	using time_delay = double;

	struct Duration;
	struct WorldClock :ecs::resource {

		double dt;
		double elapsed_time;
		double smooth_dt;
		WorldClock() {
			dt = 1 / 60.f;

			smooth_dt = 1 / 60.0f;
			elapsed_time = glfwGetTime();
		}
		double fps() const {
			return 1 / smooth_dt;
		}


		void calculate_fps() {

			double current_time = glfwGetTime();
			dt = stn::min(current_time - elapsed_time, 1.0f / min_frames);
			double update_speed = .2f;
			if (.05<dt) {
				int l = 3;
			}
			if (std::floor(elapsed_time / update_speed) != std::floor(current_time / update_speed)) {
				smooth_dt = dt;
			}
			elapsed_time = current_time;

		}
		Duration make_duration();

		double now() const {
			return elapsed_time;
		}

	private:
		const int min_frames = 2;
	};
	struct FpsTimer :ecs::System {

		void run(ecs::Ecs& world) {
			world.ensure_resource<WorldClock>().calculate_fps();
		}

	};
	struct TimePlugin {
		void operator()(core::App& app) {
			app.emplace_resource<timing::WorldClock>();
			app.emplace_system<FpsTimer>();
		}
	};
	struct Duration {

		Duration(double waiting_time, WorldClock& tman) :tm(tman) {
			set(waiting_time);
		}

		Duration(WorldClock& clock) :tm(clock) {

		}
		stn::Option<time_delay> remaining() {
			check_if_dead();
			if (end) {
				return end.unwrap() - tm->now();
			}
			return stn::None;
		}

		double remaining_or_default() {
			return remaining().unwrap_or_default();
		}
		WorldClock& clock() {
			return *tm;
		}
		const WorldClock& clock() const {
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
				end = tm->now() + dur;
			}
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
				return end < tm->now(); })) {
				end = stn::None;
			}
		}
		mutable stn::Option<double> end;
		stn::non_null<WorldClock> tm;
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