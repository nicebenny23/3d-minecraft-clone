
#include <iostream>
#include "../game/Settings.h"
#include "../util/vector3.h"
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"

#include <string>
#include "../util/dynamicarray.h"
#pragma once

namespace console {

    class ImGuiStreamBuf : public std::streambuf {
    public:
        ImGuiStreamBuf();

        Cont::array<std::string, true> log;
        int sync() override;
        
    protected:
        // Override overflow to handle direct appending of text to log
        int overflow(int c) override;
      

    private:       
        static const int buffer_size = 12; // Buffer size, can adjust
           char* buffer ;
           std::string current_line;
       
    };
    // 6. Create UI Elements
    void renderconsole();
    void CreateConsoleBuffer();
    void consoleendloop();
    extern ImGuiStreamBuf imgui_stream_buf;
}

