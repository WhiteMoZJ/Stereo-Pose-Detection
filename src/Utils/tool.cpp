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
    _cameraWindow = true;
    _vsync = false;
}

Gui::~Gui()
{
    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(_window);
    glfwTerminate();
    _cameraWindow = false;
}

bool Gui::init(const char* window_name, int width, int height)
{
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
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    _io = &ImGui::GetIO(); (void)&_io;
    _io->IniFilename = "../configs/config.ini";

    ImGui::StyleColorsClassic();

    ImGui_ImplGlfw_InitForOpenGL(_window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    return true;
}

bool Gui::showImage(Frame &frame, CameraSettings &settings, bool open)
{
    if (frame.empty()) return true;
    cv::Mat merged_img(frame.images[0].rows, frame.images[0].cols * 2 + 1,
                       frame.images[0].type(),cv::Scalar(0));
    frame.images[0].copyTo(merged_img.colRange(0, frame.images[0].cols));
    frame.images[1].copyTo(merged_img.colRange(frame.images[1].cols + 1, merged_img.cols));

    if (!glfwWindowShouldClose(_window)) {
        clear();

        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (ImGui::Begin("camera", nullptr,
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar |
                     ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse |
                     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground)) {
            if (open) {
                cv::resize(merged_img, merged_img, merged_img.size() / 2, 0, 0, cv::INTER_NEAREST_EXACT);
                cv::cvtColor(merged_img, merged_img, cv::COLOR_RGB2BGRA);

                glGenTextures(1, &_texture);
                glBindTexture(GL_TEXTURE_2D, _texture);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, merged_img.cols, merged_img.rows,
                             0, GL_RGBA, GL_UNSIGNED_BYTE, merged_img.data);

                ImGui::Image(reinterpret_cast<void *>(static_cast<intptr_t>(_texture)),
                             ImVec2(static_cast<float>(merged_img.cols), static_cast<float>(merged_img.rows)));
                showMainContents(frame);
                showCameraSettings(settings);
            }
            ImGui::End();
        }

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

void Gui::clear()
{
    glClearColor(_clear_color.x * _clear_color.w, _clear_color.y * _clear_color.w,
                 _clear_color.z * _clear_color.w, _clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    if (_texture)
    {
        glDeleteTextures(1, &_texture);
    }
}

void Gui::showMainContents(Frame &frame)
{
    if (ImGui::CollapsingHeader("status", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("%.3f ms/frame (%.1f FPS)",
                    1000.0f / _io->Framerate, _io->Framerate);
        ImGui::Checkbox("vsync", &_vsync);

        ImGui::Checkbox("debug message", &_cameraWindow);
        if (_cameraWindow) {
            ImGui::Text("counter:       %zu", frame.seq);
            ImGui::Text("time stamp:    %.3f", frame.timeStamp);
            ImGui::Text("run time:      %02d:%02d:%02d.%03d",
                        (int) frame.timeStamp / 3600000, (int) frame.timeStamp / 60000 % 60,
                        (int) frame.timeStamp / 1000 % 60, (int) frame.timeStamp % 1000);
        }
//        ImGui::ColorEdit3("background color", (float *) &_clear_color);
    }
}

void Gui::showCameraSettings(CameraSettings &settings)
{
    if (ImGui::CollapsingHeader("settings", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::SliderFloat("size", &settings.size, 0.5, 2, "%.2f x");
        ImGui::SliderFloat("exposure time(0 for auto)", &settings.exposureTime, 0.0f, 10.0f, "%.4f ms");
    }
}




