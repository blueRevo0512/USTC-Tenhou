#include "ConsolePage.h"
#include "../MainWindow.h"

// 初始化静态成员
std::deque<std::string> ConsolePage::messages;
std::mutex ConsolePage::messages_mutex;
std::condition_variable ConsolePage::input_cv;

ConsolePage::ConsolePage() : input_ready(false) {
    memset(input_buffer, 0, sizeof(input_buffer));
}

void ConsolePage::print(const std::string& text) {
    std::lock_guard<std::mutex> lock(messages_mutex);
    messages.push_back(text);
    if (messages.size() > 1000) {
        messages.pop_front();
    }
}

void ConsolePage::clear() {
    std::lock_guard<std::mutex> lock(messages_mutex);
    messages.clear();
}

std::string ConsolePage::getInput() {
    auto& instance = getInstance();
    std::unique_lock<std::mutex> lock(messages_mutex);
    input_cv.wait(lock, [&instance]{ return instance.input_ready; });
    instance.input_ready = false;
    return instance.current_input;
}

void ConsolePage::render(float window_width, float window_height, float scale_factor) {
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);

    // 标题
    const char* title = "控制台";
    ImGui::SetWindowFontScale(scale_factor * 1.5f);
    float title_width = ImGui::CalcTextSize(title).x;
    ImGui::SetCursorPosX((window_width - title_width) * 0.5f);
    ImGui::SetCursorPosY(window_height * 0.05f);
    ImGui::TextUnformatted(title);

    // 控制台区域
    ImGui::SetWindowFontScale(scale_factor);
    float console_width = window_width * 0.8f;
    float console_height = window_height * 0.6f;
    float start_x = (window_width - console_width) * 0.5f;
    float start_y = window_height * 0.15f;

    // 输出区域
    ImGui::SetCursorPosX(start_x);
    ImGui::SetCursorPosY(start_y);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
    
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
    
    ImGui::BeginChild("ScrollingRegion", ImVec2(console_width, console_height), true, 
        ImGuiWindowFlags_HorizontalScrollbar);
    
    {
        std::lock_guard<std::mutex> lock(messages_mutex);
        for (const auto& msg : messages) {
            ImGui::TextWrapped("%s", msg.c_str());
        }
    }

    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
        ImGui::SetScrollHereY(1.0f);
    }
    
    ImGui::EndChild();
    ImGui::PopStyleColor();

    // 输入区域
    ImGui::SetCursorPosX(start_x);
    ImGui::SetCursorPosY(start_y + console_height + 10);
    ImGui::PushItemWidth(console_width);
    
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));
    
    if (ImGui::InputText("##Input", input_buffer, sizeof(input_buffer), 
        ImGuiInputTextFlags_EnterReturnsTrue)) {
        std::lock_guard<std::mutex> lock(messages_mutex);
        current_input = input_buffer;
        input_ready = true;
        input_cv.notify_one();  // 通知等待的线程有输入准备好了
        memset(input_buffer, 0, sizeof(input_buffer));
    }
    
    ImGui::PopStyleColor();
    ImGui::PopItemWidth();
    ImGui::PopStyleVar();

    // 返回按钮
    float button_width = 200 * scale_factor;
    float button_height = 40 * scale_factor;
    ImGui::SetWindowFontScale(scale_factor * 1.2f);
    ImGui::SetCursorPosX((window_width - button_width) * 0.5f);
    ImGui::SetCursorPosY(window_height * 0.85f);
    
    if (ImGui::Button("返回", ImVec2(button_width, button_height))) {
        PageManager::getInstance().setCurrentPage(Page::MainMenu);
    }

    ImGui::PopFont();
} 