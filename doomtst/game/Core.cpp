#pragma once
#include "Core.h"
namespace Core {
   
    Engine game; 
    // Error callback: Logs GLFW errors.
    void errorCallback(int /*error*/, const char* description)
    {
        Assert(description);
    }

    // Framebuffer size callback: Updates viewport and global dimensions.
    void framebufferSizeCallback(GLFWwindow* window, int newWidth, int newHeight)
    {
        glViewport(0, 0, newWidth, newHeight);
        game.Window.width = newWidth;
        game.Window.height = newHeight;
    }

    // Key callback: Forwards key events to the user input module.
    void keyCallback(GLFWwindow* /*window*/, int key, int /*scancode*/, int action, int /*mods*/)
    {
        game.Inp.updatekey(key, action);
    }

    // Mouse button callback: Processes only primary mouse buttons (0 and 1).
    void mouseButtonCallback(GLFWwindow* /*window*/, int button, int action, int /*mods*/)
    {
        if (button < 2)
        {
            game.Inp.updatekey(GLFW_KEY_LAST + button, action);
        }
    }

    // Cursor position callback: Calculates movement delta and updates normalized mouse position.
    void cursorPositionCallback(GLFWwindow* /*window*/, double xpos, double ypos)
    {
        v2::Vec2 newPos(static_cast<float>(xpos), static_cast<float>(ypos));
        game.Inp.mouseposdt = newPos - game.Inp.mousepos;
        game.Inp.mousepos = newPos;
        game.Inp.normedmousepos = CtxName::ctx.Window->Center(newPos);
    }
    // Registers GLFW callbacks.



    void Engine::ConnectToContext()
    {
        ctx = &CtxName::ctx;
    }

    void Engine::createWindow()
    {
        InitTime();
        InitInput();
        new (&Window) window::Window("Benny Render 3d", "images\\crystaloreenhanced.png");
      
        Window.setCursorCallback(cursorPositionCallback);
        Window.SetFrameBufferCallback(framebufferSizeCallback);
        Window.SetMouseCallback(mouseButtonCallback);
        Window.SetKeyCallback(keyCallback);
        Window.SetErrorCallBack(errorCallback);
        
        ctx->Window = &Window;
        guirender::initgui();
    }

    void Engine::CreateGrid()
    {
      
        new(&Grid) grid::Grid(1,ctx);
        ctx->Grid = &Grid;
    }

    void Engine::InitTime()
    {
        ctx->Time = &Time;
    }
    void Engine::InitInput() {
  
        ctx->Inp = &Inp;   
    }
    void Engine::InitOC()
    {
        ctx->OC = &OC;
        ctx->OC->inject_context(ctx); 
        new (&SystemRunner) SystemExecutor(&OC);
        ctx->Executor = &SystemRunner;
    }

    void Engine::CreateWorld()
    {
        new (&world) World::world(0);
        ctx->wrld = &world;
    }

    void Engine::InitRenderer()
    {
        ctx->Ren = &ren;
    }
   
}