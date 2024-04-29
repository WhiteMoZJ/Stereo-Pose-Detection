//
// Created by junchau on 10/23/23.
//

#include "tool.h"

// Gui class function
Gui::Gui():
    frontBuffer(6)
{
    _texture = 0;
    _clearColor = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    _showDebugInfo = true;
    _isVsync = true;
    _gamma = 1.f;
}

Gui::~Gui()
{
    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(_window);
    glfwTerminate();
}

bool Gui::init(const char* window_name, const int width, const int height)
{
    ImGui::CreateContext();
    ImGui::GetIO().IniFilename = "../configs/config.ini";
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsClassic();

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

bool Gui::update()
{
    if (!glfwWindowShouldClose(_window)) {
        clear();
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();

        ImGui::NewFrame();
        updateWindow();
        ImGui::Render();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapInterval(_isVsync);   // enable vsync can reduce CPU usage
        glfwSwapBuffers(_window);
        return true;
    }
    return false;
}

void Gui::updateWindow()
{
    frontBuffer.getLatest(_displayFrame);
    _mergedImg = cv::Mat(480, 640 * 2 + 1, CV_8UC1, cv::Scalar(0));

	if(!_displayFrame.isEmpty()) {
		_displayFrame.images[0].copyTo(_mergedImg.colRange(0, _settings.getResolution().width));
		_displayFrame.images[1].copyTo(_mergedImg.colRange(_settings.getResolution().width + 1, _mergedImg.cols));

	    cv::Mat lookUpTable(1, 256, CV_8U);
	    uchar* p = lookUpTable.ptr();
	    for( int i = 0; i < 256; ++i)
	        p[i] = cv::saturate_cast<uchar>(pow(i / 255.0, _gamma) * 255.0);
	    cv::LUT(_mergedImg, lookUpTable, _mergedImg);
	}

    if (ImGui::Begin("Info", nullptr,
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {
        if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
            const cv::Size size(static_cast<int>(_mergedImg.cols * 0.4), static_cast<int>(_mergedImg.rows * 0.4));
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

    	if (ImGui::CollapsingHeader("Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
    		// ImGui::SliderFloat("size", &settings.size, 0.3, 1, "%.2f x");

    	    {   // gamma setting slider
    	        ImGui::SliderFloat("Gamma", &_gamma, 0.1f, 5.0f, "%.1f");
    	        ImGui::SameLine();
    	        if (ImGui::Button("Reset"))
    	            _gamma = 1.f;
    	    }
    	}

        if (ImGui::CollapsingHeader("Status", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Checkbox("Vsync", &_isVsync);
            ImGui::Checkbox("Debug Message", &_showDebugInfo);
            ImGui::Text("%.1f FPS/%.3f ms",
                        ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
            if (_showDebugInfo) {
                ImGui::TextDisabled(("          Camera: " +  _settings.cameraName).c_str());
                ImGui::TextDisabled(("Firmware Version: " + _settings.firmwareVersion).c_str());
                ImGui::TextDisabled(("   Serial Number: " + _settings.serialNum).c_str());

                ImGui::TextDisabled("   Frame Counter: %zu", _displayFrame.seq);
                ImGui::TextDisabled("      Time Stamp: %.3f", _displayFrame.timeStamp);
                ImGui::TextDisabled("    Running Time: %02d:%02d:%02d:%03d",
                            static_cast<int>(_displayFrame.timeStamp) / 3600000, static_cast<int>(_displayFrame.timeStamp) / 60000 % 60,
                            static_cast<int>(_displayFrame.timeStamp) / 1000 % 60, static_cast<int>(_displayFrame.timeStamp) % 1000);
            }
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
    glClearColor(_clearColor.x * _clearColor.w, _clearColor.y * _clearColor.w,
                 _clearColor.z * _clearColor.w, _clearColor.w);
    glClear(GL_COLOR_BUFFER_BIT);
    if (_texture)
    {
        glDeleteTextures(1, &_texture);
    }
}