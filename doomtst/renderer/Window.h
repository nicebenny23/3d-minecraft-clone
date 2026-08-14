#pragma once
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <functional>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../math/vector2.h"
#include "../util/stbiload.h"
#include "../game/ecs/resources.h"
#include "../util/reference.h"
#include "../game/Core.h"
#include "../util/exception.h"

namespace renderer {

	struct Window : ecs::resource {
		using CursorCallback = std::function<void(double, double)>;
		using FrameBufferCallback = std::function<void(int, int)>;
		using MouseCallback = std::function<void(int, int, int)>;
		using KeyCallback = std::function<void(int, int, int, int)>;
		using ErrorCallback = std::function<void(int, const char*)>;

		stn::non_null<GLFWwindow> window;
		stn::non_null<GLFWmonitor> monitor;
		v2::Coord2 screen;
		GLFWimage icon;
		bool cursor_enabled = true;

		CursorCallback cursor_callback;
		FrameBufferCallback frame_buffer_callback;
		MouseCallback mouse_callback;
		KeyCallback key_callback;

		inline static ErrorCallback error_callback;

		Window(stn::non_null<GLFWwindow> window,stn::non_null<GLFWmonitor> monitor,v2::Coord2 screen)
			: window(window),monitor(monitor),screen(screen) {
			glfwSetWindowUserPointer(window.get_ptr(), this);
		}

		~Window() {
			glfwTerminate();
		}

		static Window& from(GLFWwindow* window) {
			return *static_cast<Window*>(
				glfwGetWindowUserPointer(window));
		}

		void set_icon(const char* image) {
			icon.pixels = gl_util::texture_for(&icon.width,&icon.height,image);
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

		void set_cursor_callback(CursorCallback callback) {
			cursor_callback = std::move(callback);

			glfwSetCursorPosCallback(
				window.get_ptr(),
				[](GLFWwindow* window, double x, double y) {
					auto& self = from(window);
					if (self.cursor_callback) {
						self.cursor_callback(x, y);
					}
				}
			);
		}

		void set_frame_buffer_callback(FrameBufferCallback callback) {
			frame_buffer_callback = std::move(callback);

			glfwSetFramebufferSizeCallback(window.get_ptr(),
				[](GLFWwindow* window, int width, int height) {
					auto& self = from(window);
					self.screen = v2::Coord2(width, height);
					glViewport(0, 0, width, height);

					if (self.frame_buffer_callback) {
						self.frame_buffer_callback(width, height);
					}
				}
			);
		}

		void set_mouse_callback(MouseCallback callback) {
			mouse_callback = std::move(callback);

			glfwSetMouseButtonCallback(window.get_ptr(),
				[](GLFWwindow* window, int button, int action, int mods) {
					auto& self = from(window);
					if (self.mouse_callback) {
						self.mouse_callback(button, action, mods);
					}
				}
			);
		}

		void set_key_callback(KeyCallback callback) {
			key_callback = std::move(callback);

			glfwSetKeyCallback(window.get_ptr(),
				[](GLFWwindow* window, int key, int scancode, int action, int mods) {
					auto& self = from(window);
					if (self.key_callback) {
						self.key_callback(key, scancode, action, mods);
					}
				}
			);
		}

		void set_error_callback(ErrorCallback callback) {
			error_callback = std::move(callback);

			glfwSetErrorCallback(
				[](int error, const char* description) {
					if (error_callback) {
						error_callback(error, description);
					}
				}
			);
		}

		void enable_cursor() {
			cursor_enabled = true;
			glfwSetInputMode(window.get_ptr(),GLFW_CURSOR,GLFW_CURSOR_NORMAL);
		}

		void disable_cursor() {
			cursor_enabled = false;
			glfwSetInputMode(window.get_ptr(),GLFW_CURSOR,GLFW_CURSOR_DISABLED);
		}

		double aspect_ratio() const {
			if (screen.x == 0) {
				return 1;
			}
			return static_cast<double>(screen.x) / static_cast<double>(screen.y);
		}

		v2::Vec2 centered(v2::Vec2 pos) const {
			return v2::Vec2((2.0f * pos.x / screen.x) - 1.0f, 1.0f - (2.0f * pos.y / screen.y)) / 2.0f;
		}

		v2::Vec2 fit_to_aspect_ratio(v2::Vec2 pos) const {
			return centered(pos) * v2::Vec2(1.0f, 1.0f / aspect_ratio());
		}
	};

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
		GLFWwindow* window = glfwCreateWindow(screen.x,screen.y,"skib",nullptr,nullptr);
		glfwMakeContextCurrent(window);
		glfwSetWindowSizeLimits(window,400,400,GLFW_DONT_CARE,GLFW_DONT_CARE);
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			stn::throw_logic_error("Failed to initialize GLAD");
		}
		app.emplace_resource<Window>(*window,*monitor,screen);
		glfwSwapInterval(0);
	}

}