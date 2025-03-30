//
// Created by junchau on 10/23/23.
//

#ifndef TOOL_H
#define TOOL_H

#include "frame_buffer.h"
#include "pointset_buffer.h"
#include "../Device/camera.h"
#include "../stdafx.h"

#include "../Imgui/imgui.h"
#include "../Imgui/imgui_impl_glfw.h"
#include "../Imgui/imgui_impl_opengl3.h"

#include <GLFW/glfw3.h> // be sure glfw inluded after imgui


/**
 * @brief The Gui class represents a graphical user interface created using Imgui.
 */
class Gui
{
public:
    Gui();
    Gui(const Gui&) = delete;
    ~Gui();

    /**
     * @brief Initializes the GUI created by Imgui.
     *
     * This function must be used in the thread where showImage() is called.
     *
     * @param window_name The title of the window.
     * @param width The width of the window.
     * @param height The height of the window.
     * @return true if the initialization is successful, false otherwise.
     */
    bool init(const char* window_name = "window", int width = 1600, int height = 900);

    /**
     * @brief Shows the GUI.
     *
     * @param pointset The PointSet object to display.
     * @return true if the GUI is running, false otherwise.
     */
    bool update(PointSet& pointset);

    FrameBuffer frontBuffer;

private:
    /**
     * @brief Updates the window.
     */
    void updateWindow();

    /**
     * @brief Updates the pose.
     *
     * @param pointset The PointSet object to display.
     */
    void updatePose(PointSet& pointset) ;

    /**
     * @brief Clears the GUI.
     */
    void clear() const;

    /**
     * @brief GLFW error callback function.
     *
     * @param error The error code.
     * @param description The error description.
     */
    static void glfw_error_callback(const int error, const char* description)
    {
        fprintf(stderr, "GLFW Error %d: %s\n", error, description);
    }

    GLFWwindow*     _window{};
    GLuint          _texture;
    ImVec4          _clearColor;
    ImVec4          _mouseData{};
    bool            _showDebugInfo, _isVsync;
    float           _gamma;

    cv::Mat         _mergedImg;
    Frame           _displayFrame;
    CameraSettings& _settings = CameraSettings::getSettings();

    SpacePoints     _lastSpacePoints;

    std::array<Eigen::Vector3f, 4> coord = {Eigen::Vector3f(10.0f, 0.0f, 0.0f),
                                            Eigen::Vector3f(0.0f, 10.0f, 0.0f),
                                            Eigen::Vector3f(0.0f, 0.0f, 10.0f),
                                            Eigen::Vector3f(0.0f, 0.0f, 0.0f)};
    std::array<ImColor, 4> color = {ImColor(1.0f, 0.0f, 0.0f, 1.0f),
                                    ImColor(0.0f, 1.0f, 0.0f, 1.0f),
                                    ImColor(0.0f, 0.0f, 1.0f, 1.0f),
                                    ImColor(1.0f, 1.0f, 1.0f, 1.0f)};
    Eigen::Vector3f camera_position = Eigen::Vector3f(100.0f, 100.0f, 100.0f);
    Eigen::Vector3f camera_target = Eigen::Vector3f(0.0f, 0.0f, 0.0f);
    float camera_fov = 70.0f;
};

#endif // TOOL_H
