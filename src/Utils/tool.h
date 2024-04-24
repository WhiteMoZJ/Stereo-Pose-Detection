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
     * @param   window_name window title
     * @return  Initiate successful
     */
    bool init(const char* window_name = "window", int width = 1600, int height = 900);

    /**
     * @brief   Show GUI
     * @param   frame camera frame
     * @param   open signal
     * @return  Is GUI running
     */
    bool update(const Frame& frame, bool open);

private:
    void updateWindow(const Frame &frame);

    void clear() const;

    static void glfw_error_callback(const int error, const char* description)
    {
        fprintf(stderr, "GLFW Error %d: %s\n", error, description);
    }

    GLFWwindow* _window{};
    GLuint _texture;
    ImVec4 _clear_color;
    bool _debug_info, _vsync;
    cv::Mat _mergedImg;

    CameraSettings settings = CameraSettings::getSettings();
};


#endif //TOOL_H
