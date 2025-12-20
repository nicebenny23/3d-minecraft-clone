#include "../debugger/debug.h"
#include "Window.h"
#include "../util/stbiload.h"
constexpr float IDEAL_ASPECT_RATIO = 16.0f / 9.0f;
namespace window {


	// Enables the system cursor.


	// Indicates whether the window should close.
	void Window::LoadGlad() {
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			Assert("Failed to initialize GLAD");
		}
	}

	void Window::ApplyDefaults() {

		glfwSetWindowSizeLimits(WinPtr, state.MinSize,
			static_cast<int>(state.MinSize / IDEAL_ASPECT_RATIO),
			GLFW_DONT_CARE, GLFW_DONT_CARE);
		DisableCursor();
	}

	void Window::SetIcon(const char* ImgPath) {
		state.icon.pixels = texdata::loadtexdata(&state.icon.width, &state.icon.height, ImgPath); // Expects RGBA data.
		glfwSetWindowIcon(WinPtr, 1, &state.icon);
	}


	void Window::InitGLFW() {
		if (!glfwInit()) {
			Assert("Failed to initialize GLFW");
		}
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		MonPtr = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(MonPtr);
		width = mode->width;
		height = mode->height;

		WinPtr = glfwCreateWindow(width, height, state.Name, nullptr, nullptr);
		if (WinPtr == nullptr) {
			glfwTerminate();
			Assert("Window creation failed: WinPtr is null");
		}
	}

	Window::Window(const char* name, const char* icon) {
		state.Name = name;
		InitGLFW();
		ApplyDefaults();
		SetIcon(icon);
		glfwMakeContextCurrent(WinPtr);

		LoadGlad();
	}



	void Window::EnableCursor() {
		state.cursorEnabled = true;
		glfwSetInputMode(WinPtr, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
	void Window::DisableCursor() {
		state.cursorEnabled = false;
		glfwSetInputMode(WinPtr, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	float Window::AspectRatio() {
		return static_cast<float>(width) / static_cast<float>(height);

	}
	v2::Vec2 Window::Center(v2::Vec2 pos) {
		return v2::Vec2((2.0f * pos.x / width) - 1.0f, 1.0f - (2.0f * pos.y / height));
	}

	v2::Vec2 Window::FitToAspectRatio(v2::Vec2 pos) {
		return Center(pos) * v2::Vec2(1.0f, 1.0f / AspectRatio());
	}



	void Window::setCursorCallback(GLFWcursorposfun CursorCallback) {
		glfwSetCursorPosCallback(WinPtr, CursorCallback);
	}
	void Window::SetFrameBufferCallback(GLFWframebuffersizefun FrameBufferCallback) {
		glfwSetFramebufferSizeCallback(WinPtr, FrameBufferCallback);
	}
	void Window::SetMouseCallback(GLFWmousebuttonfun MouseButtonCallback) {
		glfwSetMouseButtonCallback(WinPtr, MouseButtonCallback);
	}
	void Window::SetKeyCallback(GLFWkeyfun KeyCallback) {
		glfwSetKeyCallback(WinPtr, KeyCallback);

	}
	void Window::SetErrorCallBack(GLFWerrorfun ErrorCallback) {
		glfwSetErrorCallback(ErrorCallback);
	}
} // namespace window
