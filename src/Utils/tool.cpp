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
    _mergedImg = cv::Mat(_settings.getResolution().height, _settings.getResolution().width * 2 + 1, CV_8UC1, cv::Scalar(0));

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
                ImGui::SliderFloat("FOV", &camera_fov, 10.f, 100.0f, "%.1f");
                ImGui::SameLine();
                if (ImGui::Button("Reset"))
                    camera_fov = 70.f;
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
 * @param point The 3D space point.
 */
void FX_COORD (ImDrawList* d, V2 a, V2 b, V2 s, const Eigen::Vector4f& point, const Eigen::Vector4f& origin, const float fov, const ImColor color)
{
    ImVec2 center = ImVec2(a.x + s.x/2, a.y + s.y / 2);

    // projection matrix
    Eigen::Matrix4f proj = Eigen::Matrix4f::Zero();
    proj << 1.0f / tan(fov * 3.1416 / 360) * 1.7778f, 0, 0, 0,
            0, 1.0f / tan(fov * 3.1416 / 360), 0, 0,
            0, 0, -1.0002f, -1.0f,
            0, 0, -0.002f, 0;

    Eigen::Vector4f point_nor  = proj * point;
    Eigen::Vector4f origin_nor = proj * origin;
    d->AddLine(ImVec2(origin_nor.x() + center.x, -origin_nor.y() + center.y),
        ImVec2(point_nor.x() + center.x, -point_nor.y() + center.y),
        color, 1);
}

void FX(ImDrawList* d, V2 a, V2 b, V2 s, ImVec4 m, float t, const Eigen::Vector4f& point, const float fov)
{
    ImVec2 center = ImVec2(a.x + s.x/2, a.y + s.y / 2);

    // projection matrix
    Eigen::Matrix4f proj = Eigen::Matrix4f::Zero();
    proj << 1.7778f / tan(fov * 3.1416 / 360), 0, 0, 0,
            0, 1.0f / tan(fov * 3.1416 / 360), 0, 0,
            0, 0, -1.0002f, -1.0f,
            0, 0, -0.002f, 0;

    Eigen::Vector4f point_nor  = proj * point;
    d->AddCircle(ImVec2(point_nor.x() + center.x, -point_nor.y() + center.y), 5, ImColor(1, 1, 1, 1), 0, 1);
}

void Gui::updatePose(PointSet& pointset)
{
    // pose viewport
    // n(x, y, z) is the normal vector of the plain of the camera direction and z-axis
    // R = I + sin θ * ssmatrix + (1 - cos θ) * ssmatrix^2
    // θ = 90 degree
    // ssmatrix <<  0, -z,  y,
    //              z,  0, -x,
    //             -y,  x,  0;

    const Eigen::Vector3f camera_direction_nor = (camera_target - camera_position).normalized();
    const Eigen::Vector3f norm_vec = camera_direction_nor.cross(Eigen::Vector3f(0, 0 ,1)).normalized();
    Eigen::Matrix3f ssmatrix = Eigen::Matrix3f::Zero();
    ssmatrix << 0, -norm_vec.z(), norm_vec.y(),
                norm_vec.z(), 0, -norm_vec.x(),
                -norm_vec.y(), norm_vec.x(), 0;

    Eigen::Matrix3f up_rot = Eigen::Matrix3f::Identity() + ssmatrix + ssmatrix * ssmatrix;
    // std::cout << "up_rot: \n" << up_rot << std::endl;

    // in opengl, view space is right-handed, clip space is left-handed
    const Eigen::Vector3f camera_up_nor = (up_rot * camera_direction_nor).normalized();
    Eigen::Matrix4f rot_matrix = Eigen::Matrix4f::Identity();
    rot_matrix.block<1, 3>(0, 0) = camera_direction_nor.cross(camera_up_nor);
    rot_matrix.block<1, 3>(1, 0) = camera_up_nor;
    rot_matrix.block<1, 3>(2, 0) = camera_direction_nor;

    Eigen::Matrix4f trans_matrix = Eigen::Matrix4f::Identity();
    trans_matrix << 1, 0, 0, 0,
                    0, 1, 0, 0,
                    0, 0, 1, camera_position.norm(),
                    0, 0, 0, 1;

    // pose viewport
    if (ImGui::Begin("Pose", nullptr,
        ImGuiWindowFlags_NoCollapse)) {
        const ImVec2 size = ImGui::GetWindowSize();
        const ImVec2 p = ImGui::GetItemRectSize();
        // get the size of the window title, to prevent the points from being out of the viewport
        const auto a = ImVec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y + p.y);
        const auto b = ImVec2(a.x + size.x, a.y + size.y - p.y);

        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        draw_list->PushClipRect(a, b);

        // transform the points to camera coordination
        Eigen::Vector4f origin_cam = trans_matrix * rot_matrix * Eigen::Vector4f(coord[3].x(), coord[3].y(), coord[3].z(), 1);
        for (int i = 0; i < 4; i++) {
            Eigen::Vector4f point_cam = trans_matrix * rot_matrix * Eigen::Vector4f(coord[i].x(), coord[i].y(), coord[i].z(), 1);
            FX_COORD(draw_list, a, b, size, point_cam, origin_cam, camera_fov, color[i]);
        }

        for (auto &i : pointset.points) {
            Eigen::Vector4f point_cam = trans_matrix * rot_matrix * i;
            FX(draw_list, a, b, size, ImVec4(1, 1, 1, 1), 0, point_cam, camera_fov);
        }
        draw_list->PopClipRect();

        std::cout << "\n";

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

