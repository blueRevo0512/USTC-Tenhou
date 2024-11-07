#pragma once
#include <string>
#include "imgui.h"
#include <GLFW/glfw3.h>
#include "PageManager.h"

class MainWindow {
public:
    static MainWindow& getInstance();
    void init();
    void run();
    void cleanup();
    bool should_close() const { return glfwWindowShouldClose(window); }
    static void closeWindow() { getInstance().window_should_close = true; }

private:
    MainWindow() = default;
    void setup_style();
    
    GLFWwindow* window = nullptr;
    bool is_fullscreen = false;
    bool window_should_close = false;
    int window_width = 1280;
    int window_height = 720;
    ImVec2 last_window_pos = ImVec2(0, 0);
    ImVec2 last_window_size = ImVec2(0, 0);
}; 