#include "guirender.h"
#include "../game/Core.h"
#include "../game/GameContext.h"
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_opengl3.h"
#include "../imgui/imgui_impl_glfw.h"
namespace guirender {

    void initgui()
    {

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable keyboard controls

        ImGui_ImplGlfw_InitForOpenGL(Core::game.Ecs.get_resource<window::Window>().unwrap().WinPtr, true);
        ImGui_ImplOpenGL3_Init("#version 330");
     
    }
    void destroygui()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
	
}
