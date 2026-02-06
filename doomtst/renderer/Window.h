#pragma once
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../math/vector2.h"
#include "../util/stbiload.h"
#include "../game/ecs/resources.h"

namespace CtxName {
	struct Context;

}
namespace window {
	struct WindowState {
		bool cursorEnabled;
		GLFWimage icon;
		const char* name;

		const int MinSize = 400;
	};

	struct Window:ecs::resource {
		GLFWwindow* WinPtr = nullptr;
		GLFWmonitor* MonPtr = nullptr;
		int width = 0;
		int height = 0;
		WindowState state;
		CtxName::Context* ctx;
		Window() {
			WinPtr = nullptr;
			MonPtr = nullptr;
			width = 0;
			height = 0;
			ctx = nullptr;
		}


		Window(const char* name, const char* icon);

		void ApplyDefaults();

		void SetIcon(const char* ImgPath);

		void SwapBuffers() {

			glfwSwapBuffers(WinPtr);
		}

		void setCursorCallback(GLFWcursorposfun CursorCallback);
		void SetFrameBufferCallback(GLFWframebuffersizefun FramebufferCallback);
		void SetMouseCallback(GLFWmousebuttonfun MouseButtonCallback);
		void SetKeyCallback(GLFWkeyfun KeyCallback);
		void SetErrorCallBack(GLFWerrorfun ErrorCallback);
		void EnableCursor();
		void DisableCursor();
		float AspectRatio();
		v2::Vec2 Center(v2::Vec2 pos);

		// Adjusts coordinates to maintain square proportions based on the current aspect ratio.
		v2::Vec2 FitToAspectRatio(v2::Vec2 pos);


		bool shouldClose() {
			return glfwWindowShouldClose(WinPtr);
		}
	private:
		void LoadGlad();
		void InitGLFW();
	};


	// Returns the current window aspect ratio.





}

