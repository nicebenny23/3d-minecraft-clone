#include "iostream"
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#include "../game/ecs/ecs.h"
#include "dynamicarray.h"
#include "../math/vector2.h"
#include <glm/glm.hpp>
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
				pressed = false;
				held = false;
				released = true;
			}

		}
	};
	inline int convertchartoglfwkey(const int key) {
		if ('a' <= key && key <= 'z') {
			return key - 32;
		}

		return key;


	}
	struct  InputManager :ecs::resource {
		stn::array<InputKey> keys;
		v2::Vec2 mouse_position_dt;
		v2::Vec2 mouse_position;
		InputKey left_mouse() {
			return keys[mouse_left_index];
		}

		InputKey right_mouse() {
			return keys[mouse_right_index];
		}

		InputManager() {
			mouse_position = v2::Vec2(0, 0);
			mouse_position_dt = v2::Vec2(0, 0);
			keys = stn::array<InputKey>(GLFW_KEY_LAST + extra_keys);
			keys.reach(GLFW_KEY_LAST + extra_keys);
		}

		void update_key(int code, int action) {
			if (keys.contains_index(code)) {
				keys[code].update(action);
			}
			else {
				stn::throw_logic_error("key code {} has not been mapped", code);
			}
		}
		void endupdate() {
			mouse_position_dt = v2::Vec2(0, 0);
			for (int i = 0; i < keys.length(); i++) {
				keys[i].pressed = false;
				keys[i].released = false;
			}

		}



		InputKey getKey(const size_t key) {
			return keys[convertchartoglfwkey(key)];
		}

	};


}


// !userinp

