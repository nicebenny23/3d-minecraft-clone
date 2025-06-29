#include "console.h"
#include "../game/Core.h"
#include <iostream>
#include <streambuf>
#include <string>
#include <streambuf>
#include <string>
#include "../util/dynamicarray.h"

#include "../game/GameContext.h"

    console::ImGuiStreamBuf::ImGuiStreamBuf() {

        log.destroy();
        current_line.clear();
        buffer = new char[buffer_size];
        std::fill(buffer, buffer + buffer_size, '\0');
        setp(buffer, buffer + buffer_size);
         
        
        //this makes the buffer full
    }

    

    // Override overflow to handle direct appending of text to log
    int console::ImGuiStreamBuf::overflow(int c)  {
        sync();
       // buffer[0] = c;
          return c;
           
    }

    int console::ImGuiStreamBuf::sync()
    {
        for (int i = 0; i < buffer_size; i++)
        {
           
        
          int c = buffer[i];
          if (c== '\0')
          {
              break;
          }
         
          // Convert character to string and push it to the log
          if (c == '\n') {
           log.push(current_line);
           current_line.clear();
           }
           else {
       current_line.push_back(static_cast<char>(c));
              }
          buffer[i] = '\0';
        }
        //since we modify positoin it tlls the whole thing to reset
        setp(buffer, buffer + buffer_size);
        return 0;
    }
    


console::ImGuiStreamBuf console::imgui_stream_buf;  // Instance of the custom stream buffer 
void console::CreateConsoleBuffer()
{
    
    console::imgui_stream_buf = console::ImGuiStreamBuf();
    std::cout.rdbuf(&imgui_stream_buf);

}
void console::consoleendloop()
{
    while (!CtxName::ctx.Window->shouldClose())
    {
        renderconsole();
    }
}
static ImGuiTextFilter filter;
void console::renderconsole()
{

    imgui_stream_buf.sync();
    
    ImGui::Begin("Debug Custom");
   

    filter.Draw("Filter");

    ImGui::BeginChild("ConsoleChild", ImVec2(0, 200), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
    ImVec4 yellow = ImVec4(1, 1, 0, 1);
    ImVec4 red = ImVec4(1, 0, 0, 1);
    ImVec4 white = ImVec4(1, 1, 1, 1);
        for (int i = 0; i < imgui_stream_buf.log.length; i++) {
            const char* str = imgui_stream_buf.log[i].c_str();
            if (filter.PassFilter(str+sizeof(char))) {
               
                ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
                switch (str[0]) {
                case 'y':
                color= yellow;
                 break;
                case 'w':
                    color = white;
                    break;
                case 'r':
                    color = red;
                    break;
                }
               
             
                ImGui::TextColored(color,str+sizeof(char));
            }
        }
    
    bool isNearBottom = ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - 1;

    // Auto-scroll to bottom if the user is near the bottom
    if (isNearBottom) {
        ImGui::SetScrollHereY(1.0f);
    }
    ImGui::EndChild();
    
    ImGui::BeginChild("Settings", ImVec2(0, 50), true, 0);
    ImGui::Checkbox("xray", &settings::Gamesettings.viewmode);

    //doesssent work yet make sure to implement fix for arrays from chainpool library
    if (ImGui::Button("reset"))
    {
        CreateConsoleBuffer();
    }
    ImGui::EndChild();

    ImGui::End();
}
