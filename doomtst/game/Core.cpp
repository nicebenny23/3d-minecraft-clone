#pragma once
#include "Core.h"
namespace Core {
#include <glad/glad.h>
#include <iostream>

   
    App game; 
    void errorCallback(int /*error*/, const char* description)
    {
        Assert(description);
    }

     void framebufferSizeCallback(GLFWwindow* window, int newWidth, int newHeight)
    {
        glViewport(0, 0, newWidth, newHeight);
        game.Window.width = newWidth;
        game.Window.height = newHeight;
    }

    void keyCallback(GLFWwindow* /*window*/, int key, int /*scancode*/, int action, int /*mods*/)
    {
		game.ensure_resource<userinput::InputManager>().update_key(key, action);
    }

    void mouseButtonCallback(GLFWwindow* /*window*/, int button, int action, int /*mods*/)
    {
        if (button < 2)
        {
			game.ensure_resource<userinput::InputManager>().update_key(GLFW_KEY_LAST + button, action);
        }
    }

    void cursorPositionCallback(GLFWwindow* /*window*/, double xpos, double ypos)
    {
        v2::Vec2 new_mouse_position(CtxName::ctx.Window->FitToAspectRatio(v2::Vec2(xpos,ypos)));
		userinput::InputManager& manager = game.ensure_resource<userinput::InputManager>();
		manager.mouse_position_dt = new_mouse_position - manager.mouse_position;
		manager.mouse_position = new_mouse_position;
		    }


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
  
		ctx->Inp=&Ecs.insert_resource<userinput::InputManager>(); 
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