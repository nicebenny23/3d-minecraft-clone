#pragma once
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../math/vector2.h"
#include "../util/stbiload.h"
#include "../game/ecs/resources.h"
#include "../util/reference.h"
#include "../game/Core.h"
#include "../util/exception.h"

namespace renderer {
	struct Window :ecs::resource {
		stn::non_null<GLFWwindow> window;
		stn::non_null<GLFWmonitor> monitor;
		v2::Coord2 screen;
		GLFWimage icon;
		bool cursor_enabled;
		Window(stn::non_null<GLFWwindow> window, stn::non_null<GLFWmonitor> monitor,v2::Coord2 scrn) :window(window), monitor(monitor),screen(scrn) {

		}
		~Window() {
			glfwTerminate();
		}
		void set_icon(const char* image) {
			icon.pixels = gl_util::texture_for(&icon.width, &icon.height, image); // Expects RGBA data.
			glfwSetWindowIcon(window.get_ptr(), 1, &icon);
		}
		void set_name(std::string_view name) {
			glfwSetWindowTitle(window.get_ptr(), name.data());
		}
		void fullscreen() {
			glfwMaximizeWindow(window.get_ptr());
		}
		void reset_buffer() {
			glfwSwapBuffers(window.get_ptr());

		}

		void set_cursor_callback(GLFWcursorposfun CursorCallback) {
			glfwSetCursorPosCallback(window.get_ptr(), CursorCallback);
		}
		void set_frame_buffer_callback(GLFWframebuffersizefun FrameBufferCallback) {
			glfwSetFramebufferSizeCallback(window.get_ptr(), FrameBufferCallback);
		}
		void set_mouse_callback(GLFWmousebuttonfun MouseButtonCallback) {
			glfwSetMouseButtonCallback(window.get_ptr(), MouseButtonCallback);
		}
		void set_key_callback(GLFWkeyfun KeyCallback) {
			glfwSetKeyCallback(window.get_ptr(), KeyCallback);
		}
		void set_error_callback(GLFWerrorfun ErrorCallback) {
			glfwSetErrorCallback(ErrorCallback);
		}
		void enable_cursor() {
			cursor_enabled = true;
			glfwSetInputMode(window.get_ptr(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		void disable_cursor() {
			cursor_enabled = false;
			glfwSetInputMode(window.get_ptr(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}

		double aspect_ratio() const {
			if (screen.x==0) {
				return 1;
			}
			return static_cast<double>(screen.x) / static_cast<double>(screen.y);
		}
		v2::Vec2 centered(v2::Vec2 pos) const {

			return v2::Vec2((2.0f * pos.x / screen.x) - 1.0f, 1.0f - (2.0f * pos.y / screen.y))/2.0f;
		}

		// Adjusts coordinates to maintain square proportions based on the current aspect ratio.
		v2::Vec2 fit_to_aspect_ratio(v2::Vec2 pos) const {

			return centered(pos) * v2::Vec2(1.0f, 1.0f / aspect_ratio());
		}
	};
	inline void error_callback(int /*error*/, const char* description) {
		throw std::logic_error(description);
	}
	inline void frame_buffer_callback(GLFWwindow* window, int new_width, int new_height) {
		glViewport(0, 0, new_width, new_height);
		core::game.Ecs.get_resource<renderer::Window>().screen = v2::Coord2(new_width, new_height);
	}
	inline void window_plugin(core::App& app) {
		if (!glfwInit()) {
			stn::throw_logic_error("Failed to initialize GLFW");
		}
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_SAMPLES, 4);
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		v2::Coord2 screen(mode->width, mode->height);
		GLFWwindow* window = glfwCreateWindow(screen.x, screen.y, "skib", nullptr, nullptr);
		glfwMakeContextCurrent(window);
		glfwSetWindowSizeLimits(window, 400,
			static_cast<int>(400),
			GLFW_DONT_CARE, GLFW_DONT_CARE);
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			stn::throw_logic_error("Failed to initialize GLAD");
		}
		Window& game_window= app.emplace_resource<Window>(*window, *monitor, screen); 
		game_window.set_frame_buffer_callback(frame_buffer_callback);
		game_window.set_error_callback(error_callback);		
		glfwSwapInterval(0);
	}




}

