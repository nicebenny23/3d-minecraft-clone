#include "guirender.h"

#include "../renderer/Window.h"
#include <iostream>

namespace guirender {

    void initgui()
    {

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable keyboard controls

        ImGui_ImplGlfw_InitForOpenGL(window::awindow, true);
        ImGui_ImplOpenGL3_Init("#version 330");
        console::CreateConsoleBuffer();
    }
    void destroygui()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
    void rendergui()
    {   // Start a new ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

      

        console::renderconsole();
        // 7. Render UI
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    }
}
