#pragma once
#define NOMINMAX
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../util/vector2.h"#include "../util/stbiload.h"

namespace CtxName {
    struct Context;

}
namespace window {
    struct WindowState
    {
        bool cursorEnabled;
        GLFWimage icon;
        const char* Name;

        const int MinSize = 400;

    };

    struct Window {
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
        }
      
        
        Window(const char* name,const char* icon);
        
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
        v2::Vector2 Center(v2::Vector2 pos);

        // Adjusts coordinates to maintain square proportions based on the current aspect ratio.
          v2::Vector2 FitToAspectRatio(v2::Vector2 pos);


          bool shouldClose()
          {
              return glfwWindowShouldClose(WinPtr);
          }
    private:
        void LoadGlad();
        void InitGLFW();
    };


// Returns the current window aspect ratio.





}

