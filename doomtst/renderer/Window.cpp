#include "Window.h"
#include "../util/userinput.h"

namespace window {
    GLFWwindow* awindow;
    int height;
    int width;
    
    bool createwindow()
    {
        return false;
    }
    void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
    {
        userinput::updateotherkeys(button, action);
     }
  static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
      if (key == GLFW_KEY_ESCAPE)
      {
          userinput::updateotherkeys(256, action);
      }
      
        userinput::updatekey(key, action);
    }
  static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
  {
      userinput::mouseposdt = userinput::mousepos - glm::vec2(float(xpos), float(ypos));
      userinput::mousepos= glm::vec2(float(xpos), float(ypos)); 
      
      userinput::normedmousepos= v2::Vector2( (2.0 * xpos / width) - 1.0, (1-(2.0 * ypos / height))/1.777);
  }

 static void framebuffer_size_callback(GLFWwindow* window, int widt1h, int heigh1t)
    {
        // make sure the viewport matches the new window dimensions; note that width and 
        // height will be significantly larger than specified on retina displays.
        glViewport(0, 0, widt1h, heigh1t);

    }
 void setcursor(bool mode)
 {
     if (mode)
     {
         glfwSetInputMode(awindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
     }
     else
     {
         glfwSetInputMode(awindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
     }
    
 }
 void createwindow(int scrwidth, int scrheight)
    {
     
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        width = mode->width;
        height = mode->height;
        awindow = glfwCreateWindow(width, height, "benny render 3d",NULL, NULL);
        if (awindow == NULL)
        {
            
            glfwTerminate();
            Assert("window was null");
        }
        glfwMakeContextCurrent(awindow);
        
       

        
        setcursor(false);
        glfwSetCursorPosCallback(awindow, cursor_position_callback);
        glfwSetFramebufferSizeCallback(awindow, framebuffer_size_callback);
        glfwSetMouseButtonCallback(awindow, mouse_button_callback);
        glfwSetKeyCallback(awindow, key_callback);
      
    }
    


    void swapbuffer() {
        glfwSwapBuffers(awindow);
    }
    void processInput()
    {

    }
    bool shouldclose() {
        return glfwWindowShouldClose(awindow);
    }
}