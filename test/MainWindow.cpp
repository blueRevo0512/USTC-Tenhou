#include "MainWindow.h"
#include "pages/MainMenuPage.h"
#include "pages/AboutPage.h"
#include "pages/HelpPage.h"
#include "pages/JoinRoomPage.h"
#include "pages/CreateRoomPage.h"
#include "pages/ConsolePage.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

MainWindow& MainWindow::getInstance() {
    static MainWindow instance;
    return instance;
}

void MainWindow::setup_style() {
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 12.0f;
    style.FrameRounding = 8.0f;
    style.PopupRounding = 8.0f;
    style.ScrollbarRounding = 8.0f;
    style.GrabRounding = 8.0f;
    style.TabRounding = 8.0f;
    style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
    
    // 更新颜色主题
    ImVec4* colors = style.Colors;
    colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.98f);
    colors[ImGuiCol_Button] = ImVec4(0.41f, 0.41f, 0.41f, 0.40f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.50f, 0.50f, 0.50f, 0.80f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.37f, 0.37f, 0.37f, 1.00f);
    colors[ImGuiCol_Text] = ImVec4(0.95f, 0.95f, 0.95f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
}

void MainWindow::init() {
    if (!glfwInit()) {
        return;
    }

    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_SAMPLES, 4); // 启用多重采样抗锯齿

    // 获取显示器DPI缩放
    GLFWmonitor* primary = glfwGetPrimaryMonitor();
    float xscale, yscale;
    glfwGetMonitorContentScale(primary, &xscale, &yscale);

    // 根据DPI缩放调整窗口大小
    window = glfwCreateWindow(window_width * xscale, window_height * yscale, "USTC 天凤", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    
    // 配置字体渲染选项
    ImFontConfig font_config;
    font_config.OversampleH = 2;
    font_config.OversampleV = 2;
    font_config.PixelSnapH = true;
    
    io.Fonts->AddFontFromFileTTF("c:/windows/fonts/msyh.ttc", 24.0f, &font_config, io.Fonts->GetGlyphRangesChineseFull());

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    setup_style();
}

void MainWindow::run() {
    MainMenuPage& main_menu_page = MainMenuPage::getInstance();
    AboutPage& about_page = AboutPage::getInstance();
    HelpPage& help_page = HelpPage::getInstance();
    JoinRoomPage& join_room_page = JoinRoomPage::getInstance();
    CreateRoomPage& create_room_page = CreateRoomPage::getInstance();
    ConsolePage& console_page = ConsolePage::getInstance();

    while (!glfwWindowShouldClose(window) && !window_should_close) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 设置窗口属性
        ImGuiIO& io = ImGui::GetIO();
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(io.DisplaySize);
        
        ImGui::Begin("Page", nullptr, 
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse);

        float window_width = ImGui::GetWindowWidth();
        float window_height = ImGui::GetWindowHeight();
        float scale_factor = std::min(window_width / 1280.0f, window_height / 720.0f);

        // 根据当前页面状态渲染不同的内容
        switch (PageManager::getInstance().getCurrentPage()) {
            case Page::MainMenu:
                main_menu_page.render(window_width, window_height, scale_factor);
                break;
            case Page::CreateRoom:
                create_room_page.render(window_width, window_height, scale_factor);
                break;
            case Page::About:
                about_page.render(window_width, window_height, scale_factor);
                break;
            case Page::Help:
                help_page.render(window_width, window_height, scale_factor);
                break;
            case Page::JoinRoom:
                join_room_page.render(window_width, window_height, scale_factor);
                break;
            case Page::Console:
                console_page.render(window_width, window_height, scale_factor);
                break;
            default:
                main_menu_page.render(window_width, window_height, scale_factor);
                break;
        }

        ImGui::End();

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }
}

void MainWindow::cleanup() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
} 