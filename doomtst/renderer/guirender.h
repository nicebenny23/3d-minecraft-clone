#pragma once

#include "../math/vector3.h"
#include "../debugger/console.h"
#include "../imgui/imgui_impl_opengl3.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../game/ecs/ecs.h"
#include "../game/Core.h"
namespace guirender{

	void initgui();
	void destroygui();
	struct GuiSystem :ecs::System {   // Start a new ImGui frame
		GuiSystem() {


		}
		void run(ecs::Ecs& ecs) {
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			// ... your other windows
			console::Console::Instance().Render();
			// 7. Render UI
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}
	};
	struct ConsolePlugin :Core::Plugin {
		void build(Core::App& app) {
			initgui();
			app.emplace_system<GuiSystem>();
		}
	};
}

