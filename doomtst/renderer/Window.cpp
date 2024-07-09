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
        userinput::updatemousebutton(button, action);
     }
  static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        userinput::updatekey(key, action);
    }
  static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
  {
      userinput::mouseposdt = userinput::mousepos - glm::vec2(float(xpos), float(ypos));
      userinput::mousepos= glm::vec2(float(xpos), float(ypos));
  }

 static void framebuffer_size_callback(GLFWwindow* window, int widt1h, int heigh1t)
    {
        // make sure the viewport matches the new window dimensions; note that width and 
        // height will be significantly larger than specified on retina displays.
        glViewport(0, 0, widt1h, heigh1t);

    }
    void createcurwindow(int scrwidth, int scrheight)
    {
     
     //   glfwSetWindowMonitor(awindow, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
      
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        awindow = glfwCreateWindow(mode->width, mode->height, "Project",NULL, NULL);
        if (awindow == NULL)
        {
            
            glfwTerminate();

        }
        glfwMakeContextCurrent(awindow);
        
       

        width = mode->width;
        height = mode->height;
      
        glfwSetCursorPosCallback(awindow, cursor_position_callback);
        glfwSetFramebufferSizeCallback(awindow, framebuffer_size_callback);
        glfwSetMouseButtonCallback(awindow, mouse_button_callback);
        glfwSetKeyCallback(awindow, key_callback);
        glfwSetInputMode(awindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }



    void swapbuffer() {
        glfwSwapBuffers(awindow);
    }
    void processInput()
    {

        if (glfwGetKey(awindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(awindow, true);
    }
    bool shouldclose() {
        return glfwWindowShouldClose(awindow);
    }
}