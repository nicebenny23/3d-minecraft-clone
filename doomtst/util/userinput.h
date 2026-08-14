#include "iostream"
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#include "../game/ecs/ecs.h"
#include "dynamicarray.h"
#include "../math/vector2.h"
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include "../renderer/Window.h"
#pragma once 
namespace userinput {

	constexpr size_t mouse_left_index = GLFW_KEY_LAST + GLFW_MOUSE_BUTTON_LEFT;
	constexpr size_t mouse_right_index = GLFW_KEY_LAST + GLFW_MOUSE_BUTTON_RIGHT;
	constexpr size_t extra_keys=2;
	constexpr size_t escape_key = GLFW_KEY_ESCAPE;
	constexpr size_t shift_key = GLFW_KEY_LEFT_SHIFT;
	struct InputKey {
		bool held;
		bool released;
		bool pressed;
		InputKey() {

			held = false;
			pressed = false;
			released = false;

		}

		void update(int action) {
			if (action == GLFW_PRESS) {

				pressed = true;
				held = true;

			}
			if (action == GLFW_RELEASE) {
				held = false;
				released = true;
			}

		}
	};
	inline size_t convertchartoglfwkey(size_t key) {
		if ('a' <= key && key <= 'z') {
			return key - 32;
		}

		return key;


	}
	struct  InputManager :ecs::resource {
		stn::array<InputKey> keys;

		v2::Vec2 adjusted_mouse_position_dt;
		v2::Vec2 mouse_position;
		InputKey left_mouse() {
			return keys[mouse_left_index];
		}

		InputKey right_mouse() {
			return keys[mouse_right_index];
		}

		InputManager() {
			mouse_position = v2::Vec2(0, 0);
			adjusted_mouse_position_dt = v2::Vec2(0, 0);
			keys = stn::array<InputKey>(GLFW_KEY_LAST + extra_keys);
		}

		void update_key(int code, int action) {
			if (keys.contains_index(code)) {
				keys[code].update(action);
			}
			else {
				stn::throw_logic_error("key code {} has not been mapped", code);
			}
		}
	

		InputKey key(const size_t key_index) {
			return keys[convertchartoglfwkey(key_index)];
		}

	};
	struct InputPollingSystem :ecs::System {
		void run(ecs::Ecs& world) {
			userinput::InputManager& manager=world.get_resource<userinput::InputManager>();
			manager.adjusted_mouse_position_dt = v2::Vec2(0, 0);
			for (InputKey& key: manager.keys) {
				key.pressed = false;
				key.released = false;
			}
			glfwPollEvents();
		}
	};
	inline void user_input_plugin(core::App& app) {
		app.insert_plugin(renderer::window_plugin);
		renderer::Window& window=app.Ecs.get_resource<renderer::Window>();
		app.emplace_system< InputPollingSystem>();
		app.emplace_resource< InputManager>();
		ecs::Ecs& ecs = app.Ecs;
		window.set_key_callback(
			[&ecs](int key, int, int action, int) {
				ecs.get_resource<InputManager>().update_key(key, action);
			});

		window.set_mouse_callback(
			[&ecs](int button, int action, int) {
				ecs.get_resource<InputManager>().update_key(GLFW_KEY_LAST + button, action);
			});

		window.set_cursor_callback(
			[&ecs](double xpos, double ypos) {
				auto& input = ecs.get_resource<InputManager>();
				auto& window = ecs.get_resource<renderer::Window>();
				auto position = window.fit_to_aspect_ratio(v2::Vec2(xpos, ypos));
				input.adjusted_mouse_position_dt =position - input.mouse_position;
				input.mouse_position = position;
			});
	}
}


// !userinp

