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
        
        ImGui_ImplGlfw_InitForOpenGL(CtxName::ctx.Window->WinPtr, true);
        ImGui_ImplOpenGL3_Init("#version 330");
     
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

      
        // ... your other windows
        console::Console::Instance().Render();

    
        // 7. Render UI
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    }
}
