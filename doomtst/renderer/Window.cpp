#include "Window.h"

constexpr float IDEAL_ASPECT_RATIO = 16.0f / 9.0f;
constexpr int MIN_WINDOW_SIZE = 400;

namespace window {

    //---------------------------------------------------------------------
    // Global Variables
    //---------------------------------------------------------------------
    GLFWwindow* awindow = nullptr;
    int width = 0;
    int height = 0;

    //---------------------------------------------------------------------
    // Internal Helper Functions and Callbacks (Private)
    //---------------------------------------------------------------------
    namespace {

        // Error callback: Logs GLFW errors.
        void errorCallback(int /*error*/, const char* description)
        {
            Assert(description);
        }

        // Framebuffer size callback: Updates viewport and global dimensions.
        void framebufferSizeCallback(GLFWwindow* /*window*/, int newWidth, int newHeight)
        {
            glViewport(0, 0, newWidth, newHeight);
            width = newWidth;
            height = newHeight;
        }

        // Key callback: Forwards key events to the user input module.
        void keyCallback(GLFWwindow* /*window*/, int key, int /*scancode*/, int action, int /*mods*/)
        {
            userinput::updatekey(key, action);
        }

        // Mouse button callback: Processes only primary mouse buttons (0 and 1).
        void mouseButtonCallback(GLFWwindow* /*window*/, int button, int action, int /*mods*/)
        {
            if (button < 2)
            {
                userinput::updatekey(GLFW_KEY_LAST + button, action);
            }
        }
        
        // Cursor position callback: Calculates movement delta and updates normalized mouse position.
        void cursorPositionCallback(GLFWwindow* /*window*/, double xpos, double ypos)
        {
            v2::Vector2 newPos(static_cast<float>(xpos), static_cast<float>(ypos));
            userinput::mouseposdt = newPos - userinput::mousepos;
            userinput::mousepos = newPos;
            userinput::normedmousepos = centerCoord(newPos);
        }
        // Registers GLFW callbacks.
        void setupCallbacks()
        {
            glfwSetCursorPosCallback(awindow, cursorPositionCallback);
            glfwSetFramebufferSizeCallback(awindow, framebufferSizeCallback);
            glfwSetMouseButtonCallback(awindow, mouseButtonCallback);
            glfwSetKeyCallback(awindow, keyCallback);
            glfwSetErrorCallback(errorCallback);
        }
        // Loads the OpenGL functions using GLAD.
        void loadGlad()
        {
            if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
            {
                Assert("Failed to initialize GLAD");
            }
        }
        // Initializes GLFW and creates a window using the primary monitor's resolution.
        void initGLFWWindow(const char* name)
        {
            if (!glfwInit())
            {
                Assert("Failed to initialize GLFW");
            }
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

            GLFWmonitor* monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode* mode = glfwGetVideoMode(monitor);
            width = mode->width;
            height = mode->height;

            awindow = glfwCreateWindow(width, height, name, nullptr, nullptr);
            if (!awindow)
            {
                glfwTerminate();
                Assert("Window creation failed: awindow is null");
            }
        }


        // Applies default window properties: sets the icon, enforces minimum size, and disables the cursor.
        void applyDefaultWindowProperties()
        {
            setIcon("images\\crystaloreenhanced.png");
            glfwSetWindowSizeLimits(awindow, MIN_WINDOW_SIZE,
                static_cast<int>(MIN_WINDOW_SIZE / IDEAL_ASPECT_RATIO),
                GLFW_DONT_CARE, GLFW_DONT_CARE);
            disableCursor();
        }

   

        

    } // end anonymous namespace

    //---------------------------------------------------------------------
    // Public Interface Functions
    //---------------------------------------------------------------------

    // Creates the window, initializes the OpenGL context, and sets up callbacks.
    // Note: scrWidth and scrHeight are currently unused as the primary monitor's resolution is used.
    void createWindow()
    {
        initGLFWWindow("Benny Render 3d");
        applyDefaultWindowProperties();
        glfwMakeContextCurrent(awindow);
        setupCallbacks();
        loadGlad();
        guirender::initgui();
    }

    // Enables the system cursor.


    // Indicates whether the window should close.
 

} // namespace window
