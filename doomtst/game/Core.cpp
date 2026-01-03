#pragma once
#include "Core.h"
namespace Core {
#include <glad/glad.h>
#include <iostream>

   
    App game; 
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



    void App::ConnectToContext()
    {
        ctx = &CtxName::ctx;
    }

    void App::createWindow()
    {
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

    void App::CreateGrid()
    {
		Grid=&Ecs.insert_resource<grid::Grid>(1, ctx);
        
        ctx->Grid = Grid;
    }

    void App::InitInput() {
  
        ctx->Inp = &Inp;   
    }
    void App::InitOC()
    {
        ctx->Ecs = &Ecs;
    }

    void App::CreateWorld()
    {
		
        new (&world) World::world(0);
        ctx->wrld = &world;
    }

    void App::InitRenderer()
    {  
		ren = &Ecs.get_resource<renderer::Renderer>().unwrap();
        ctx->Ren = ren;
    }
   
}