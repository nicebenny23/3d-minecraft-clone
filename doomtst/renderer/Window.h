#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../util/vector2.h"
#include "../util/userinput.h"
#include "../external/stb_image.h"
#include "guirender.h"
#include "../util/stbiload.h"

namespace window {

	extern GLFWwindow* awindow;
	extern int width;
	extern int height;



void createWindow();


// Sets the window icon using the image at the specified path.
inline void setIcon(const char* imagePath)
{
    GLFWimage icon;
    icon.pixels = texdata::loadtexdata(&icon.width, &icon.height, imagePath); // Expects RGBA data.
    glfwSetWindowIcon(awindow, 1, &icon);
}

// Swaps the front and back buffers.
inline  void swapBuffers()
{
    glfwSwapBuffers(awindow);
}


inline  void enableCursor()
{
    glfwSetInputMode(awindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

// Disables (hides) the system cursor.
inline void disableCursor()
{
    glfwSetInputMode(awindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}


inline bool shouldClose()
{
    return glfwWindowShouldClose(awindow);
}
// Returns the current window aspect ratio.

inline float getAspectRatio()
{
    return static_cast<float>(width) / static_cast<float>(height);
}

inline v2::Vector2 centerCoord(v2::Vector2 pos)
{
    return v2::Vector2((2.0f * pos.x / width) - 1.0f,
        1.0f - (2.0f * pos.y / height));
}

// Adjusts coordinates to maintain square proportions based on the current aspect ratio.
inline v2::Vector2 aspectAdjustedCoord(v2::Vector2 pos)
{
    return centerCoord(pos) * v2::Vector2(1.0f, 1.0f / getAspectRatio());
}

}

