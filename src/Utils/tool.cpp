//
// Created by junchau on 10/23/23.
//

#include "tool.h"

// Debug class functions

// Gui class function
Gui::Gui()
{
    _texture = 0;
    _clear_color = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    _debug_info = true;
    _vsync = false;
    _mergedImg = cv::Mat(480, 640, CV_8UC3, cv::Scalar(0));
}

Gui::~Gui()
{
    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(_window);
    glfwTerminate();
    _debug_info = false;
}

bool Gui::init(const char* window_name, const int width, const int height)
{
    ImGui::CreateContext();
    ImGui::GetIO().IniFilename = "../configs/config.ini";
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    ImGui::StyleColorsClassic();
    ImGuiStyle style = ImGui::GetStyle();
    style.WindowRounding = 0.5f;

    glfwSetErrorCallback(glfw_error_callback);
    if(!glfwInit()){
        return false;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    _window = glfwCreateWindow(width, height, window_name, nullptr, nullptr);
    if (!_window) {
        printf("GUI show failed\n");
        return false;
    }
    glfwMakeContextCurrent(_window);

    ImGui_ImplGlfw_InitForOpenGL(_window, true);
    ImGui_ImplOpenGL3_Init("#version 410");
    return true;
}

bool Gui::update(const Frame &frame, const bool open)
{
    if(!frame.empty()) {
        _mergedImg = cv::Mat(frame.images[0].rows, frame.images[0].cols * 2 + 1,
                             frame.images[0].type(), cv::Scalar(0));
        frame.images[0].copyTo(_mergedImg.colRange(0, frame.images[0].cols));
        frame.images[1].copyTo(_mergedImg.colRange(frame.images[1].cols + 1, _mergedImg.cols));
    }
    if (!glfwWindowShouldClose(_window)) {
        clear();
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        updateWindow(frame);

        ImGui::Render();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(_window);
        glfwSwapInterval(_vsync);   // enable or disable vsync
        return true;
    }
    else {
        return false;
    }
}

void Gui::updateWindow(const Frame &frame)
{
    if (ImGui::Begin("camera", nullptr,
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {
        if (ImGui::CollapsingHeader("camera", ImGuiTreeNodeFlags_DefaultOpen)) {
            const cv::Size size(_mergedImg.cols * settings.size, _mergedImg.rows * settings.size);
            cv::resize(_mergedImg, _mergedImg, size, 0, 0, cv::INTER_NEAREST_EXACT);
            cv::cvtColor(_mergedImg, _mergedImg, cv::COLOR_RGB2BGRA);

            glGenTextures(1, &_texture);
            glBindTexture(GL_TEXTURE_2D, _texture);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _mergedImg.cols, _mergedImg.rows,
                         0, GL_RGBA, GL_UNSIGNED_BYTE, _mergedImg.data);

            ImGui::Image(reinterpret_cast<void *>(static_cast<intptr_t>(_texture)),
                         ImVec2(static_cast<float>(_mergedImg.cols), static_cast<float>(_mergedImg.rows)));
        }

        if (ImGui::CollapsingHeader("status", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Text("%.3f ms/frame (%.1f FPS)",
                        1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::Checkbox("vsync", &_vsync);

            ImGui::Checkbox("debug message", &_debug_info);
            if (_debug_info) {
                ImGui::Text("counter:       %zu", frame.seq);
                ImGui::Text("time stamp:    %.3f", frame.timeStamp);
                ImGui::Text("run time:      %02d:%02d:%02d.%03d",
                            static_cast<int>(frame.timeStamp) / 3600000, static_cast<int>(frame.timeStamp) / 60000 % 60,
                            static_cast<int>(frame.timeStamp) / 1000 % 60, static_cast<int>(frame.timeStamp) % 1000);
            }
        }

        if (ImGui::CollapsingHeader("settings", ImGuiTreeNodeFlags_DefaultOpen)) {
            // ImGui::SliderFloat("size", &settings.size, 0.3, 1, "%.2f x");
            ImGui::SliderFloat("exposure time(0 for auto)", &settings.gamma, 0.0f, 10.0f, "%.4f ms");
        }

        ImGui::End();
    }

    if (ImGui::Begin("Pose", nullptr,
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {
        ImGui::End();
    }
}

void Gui::clear() const
{
    glClearColor(_clear_color.x * _clear_color.w, _clear_color.y * _clear_color.w,
                 _clear_color.z * _clear_color.w, _clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    if (_texture)
    {
        glDeleteTextures(1, &_texture);
    }
}