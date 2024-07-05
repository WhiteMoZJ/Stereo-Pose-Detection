//
// Created by junchau on 10/23/23.
//

#include "tool.h"

// Gui class function
Gui::Gui():
    frontBuffer(3)
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

    glfwTerminate();
}

bool Gui::init(const char* window_name, const int width, const int height)
{
    if(!glfwInit()){
        return false;
    }
    glfwSetErrorCallback(glfw_error_callback);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    _window = glfwCreateWindow(width, height, window_name, nullptr, nullptr);
    if (!_window) {
        printf("GUI show failed\n");
        return false;
    }
    glfwMakeContextCurrent(_window);

    ImGui::CreateContext();
    ImGui::GetIO().IniFilename = "../configs/config.ini";
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsClassic();

    ImGui_ImplGlfw_InitForOpenGL(_window, true);
    ImGui_ImplOpenGL3_Init("#version 410");

    return true;
}

bool Gui::update(PointSet& pointset)
{
    if (!glfwWindowShouldClose(_window)) {
        clear();
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();

        ImGui::NewFrame();
        updateWindow();
        updatePose(pointset);
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

        // camera viewport
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

        // setting options viewport
    	if (ImGui::CollapsingHeader("Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
    	    {   // gamma setting slider
    	        ImGui::SliderFloat("Gamma", &_gamma, 0.1f, 5.0f, "%.1f");
    	        ImGui::SameLine();
    	        if (ImGui::Button("Reset"))
    	            _gamma = 1.f;
    	    }
    	}

        // status viewport
        if (ImGui::CollapsingHeader("Status", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Checkbox("Vsync", &_isVsync);
            ImGui::Checkbox("Debug Message", &_showDebugInfo);
            ImGui::Text("GUI Framerate: %.1f FPS/%.3f ms",
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
}

#define V2 ImVec2
/**
 * @brief Render the pose
 * get the space points and render the pose
 * calculate the perspective projection of the 3D points
 *
 * @param d The ImDrawList object.
 * @param a upper-left corner
 * @param b lower-right corner
 * @param s size (== b - a)
 * @param m x,y = mouse position (normalized so 0,0 over 'a'; 1,1 is over 'b', not clamped)
 *          z,w = left/right button held. <-1.0f not pressed, 0.0f just pressed, >0.0f time held.
 * @param t time
 */
void FX (ImDrawList* d, V2 a, V2 b, V2 s, ImVec4 m, float t, Eigen::Vector3f& points)
{
    // Draw the pose
    ImVec2 p = ImVec2(points.x(), points.y());
    d->AddCircleFilled(p, 5, IM_COL32(255, 0, 0, 255));
}

void Gui::updatePose(PointSet& pointset) const
{
    // pose viewport
    if (ImGui::Begin("Pose", nullptr,
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {
        ImGuiIO& io = ImGui::GetIO();
        ImVec2 size(320.0f, 180.0f);
        ImGui::InvisibleButton("canvas", size);
        ImVec2 p0 = ImGui::GetItemRectMin();
        ImVec2 p1 = ImGui::GetItemRectMax();
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        draw_list->PushClipRect(p0, p1);

        ImVec4 mouse_data;
        mouse_data.x = (io.MousePos.x - p0.x) / size.x;
        mouse_data.y = (io.MousePos.y - p0.y) / size.y;
        mouse_data.z = io.MouseDownDuration[0];
        mouse_data.w = io.MouseDownDuration[1];

        FX(draw_list, p0, p1, size, mouse_data, static_cast<float>(ImGui::GetTime()), pointset.points[0]);
        draw_list->PopClipRect();
        ImGui::End();
    }
}

void Gui::clear() const
{
    glClearColor(_clearColor.x, _clearColor.y, _clearColor.z, _clearColor.w);
    glClear(GL_COLOR_BUFFER_BIT);
    if (_texture)
        glDeleteTextures(1, &_texture);
}
