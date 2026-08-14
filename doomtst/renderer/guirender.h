#pragma once

#include "../math/vector3.h"
#include "../debugger/console.h"
#include "../imgui/imgui_impl_opengl3.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../game/ecs/ecs.h"
#include "../game/Core.h"
#include "Window.h"
namespace guirender{
	void destroygui();
	struct GuiSystem :ecs::System {   
		GuiSystem() {


		}
		~GuiSystem() {
			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplGlfw_Shutdown();
			ImGui::DestroyContext();
		}
		void run(ecs::Ecs& ecs) {
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
		
			ecs.insert_resource<console::Console>(ecs).render();
			ImGui::render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}
	};
	inline void console_plugin(core::App& app){
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		ImGui_ImplGlfw_InitForOpenGL(app.Ecs.get_resource<renderer::Window>().window.get_ptr(), true);
		ImGui_ImplOpenGL3_Init("#version 330"); 
		app.emplace_system<GuiSystem>();
	};
}

