//
// Created by junchau on 10/23/23.
//

#ifndef TOOL_H
#define TOOL_H

#include "../Task/frame_buffer.h"
#include "../stdafx.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"
#include "../imgui/imgui_impl_opengl3_loader.h"
#include <GLFW/glfw3.h>

class Debug
{
public:
    Debug() = default;
};

class Gui
{
public:
    Gui();
    Gui(const Gui&) = delete;
    ~Gui();

    /**
     * @brief   Initiate GUI create by Imgui
     *
     * (Must be used in the thread where showImage() is)
     * @param   window_name
     * @return  Initiate successful
     */
    bool init(const char* window_name = "window");

    /**
     * @brief   Show GUI
     * @param   frame camera frame
     * @return  Is GUI running
     */
    bool showImage(Frame& frame, bool open);

private:
    void clear();
    void showMainContents(Frame &frame, bool open);
    static void glfw_error_callback(int error, const char* description)
    {
        fprintf(stderr, "GLFW Error %d: %s\n", error, description);
    }

    GLFWwindow* _window{};
    GLuint _texture;
    ImVec4 _clear_color;
    ImGuiIO* _io{};
    bool _cameraWindow;
};


#endif //TOOL_H
