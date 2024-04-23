//
// Created by junchau on 10/23/23.
//

#ifndef TOOL_H
#define TOOL_H

#include "../Task/frame_buffer.h"
#include "../Device/camera.h"
#include "../stdafx.h"

#include "../Imgui/imgui.h"
#include "../Imgui/imgui_impl_glfw.h"
#include "../Imgui/imgui_impl_opengl3.h"
#include "../Imgui/imgui_impl_opengl3_loader.h"
#include <GLFW/glfw3.h> // be sure glfw inluded after imgui

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
    bool init(const char* window_name = "window", int width = 1600, int height = 900);

    /**
     * @brief   Show GUI
     * @param   frame camera frame
     * @return  Is GUI running
     */
    bool showImage(Frame& frame, CameraSettings &settings, bool open);

private:
    void clear();
    /**
     * @brief Show frame detail information
     * @param frame
     */
    void showMainContents(Frame &frame);

    /**
     * @brief Show camera runtime settings
     * @param settings Camera Settings
     */
    void showCameraSettings(CameraSettings &settings);

    static void glfw_error_callback(int error, const char* description)
    {
        fprintf(stderr, "GLFW Error %d: %s\n", error, description);
    }

    GLFWwindow* _window{};
    GLuint _texture;
    ImVec4 _clear_color;
    ImGuiIO* _io{};
    bool _cameraWindow, _vsync;
};


#endif //TOOL_H
