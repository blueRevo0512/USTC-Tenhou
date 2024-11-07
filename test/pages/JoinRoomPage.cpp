#include "JoinRoomPage.h"
#include "ConsolePage.h"
#include "../MainWindow.h"
#include "../UIManager.h"
#include <thread>

void JoinRoomPage::render(float window_width, float window_height, float scale_factor) {
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);

    // 标题
    const char* title = "加入房间";
    ImGui::SetWindowFontScale(scale_factor * 1.5f);
    float title_width = ImGui::CalcTextSize(title).x;
    ImGui::SetCursorPosX((window_width - title_width) * 0.5f);
    ImGui::SetCursorPosY(window_height * 0.2f);
    ImGui::TextUnformatted(title);

    // 输入框区域
    float input_width = 300 * scale_factor;
    float input_height = 40 * scale_factor;
    float label_height = ImGui::GetTextLineHeight();
    float vertical_spacing = window_height * 0.05f;
    float start_y = window_height * 0.35f;

    ImGui::SetWindowFontScale(scale_factor);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
    ImGui::PushItemWidth(input_width);

    // IP地址输入
    ImGui::SetCursorPosX((window_width - input_width) * 0.5f);
    ImGui::SetCursorPosY(start_y);
    ImGui::TextUnformatted("IP地址：");
    
    ImGui::SetCursorPosX((window_width - input_width) * 0.5f);
    ImGui::SetCursorPosY(start_y + label_height + 5);

    // 检查IP输入框是否被点击
    bool is_ip_active = ImGui::IsItemActive();
    if (!was_ip_active && is_ip_active) {
        memset(ip_buffer, 0, MAX_INPUT_LENGTH);
        ip_address.clear();
    }
    was_ip_active = is_ip_active;

    if (ImGui::InputText("##ip", ip_buffer, MAX_INPUT_LENGTH)) {
        ip_address = std::string(ip_buffer);
    }
    
    // 端口输入
    ImGui::SetCursorPosX((window_width - input_width) * 0.5f);
    ImGui::SetCursorPosY(start_y + label_height + input_height + vertical_spacing);
    ImGui::TextUnformatted("端口号：");
    
    ImGui::SetCursorPosX((window_width - input_width) * 0.5f);
    ImGui::SetCursorPosY(start_y + label_height + input_height + vertical_spacing + label_height + 5);
    if (ImGui::InputText("##port", port_buffer, MAX_INPUT_LENGTH)) {
        port = std::string(port_buffer);
    }
    
    ImGui::PopItemWidth();
    ImGui::PopStyleVar();

    // 按钮
    float button_width = 150 * scale_factor;
    float button_height = 40 * scale_factor;
    float button_spacing = 50 * scale_factor;
    float buttons_y = window_height * 0.7f;

    ImGui::SetWindowFontScale(scale_factor * 1.2f);

    // 计算两个按钮的总宽度和间距
    float total_width = button_width * 2 + button_spacing;
    float start_x = (window_width - total_width) * 0.5f;

    // 返回按钮
    ImGui::SetCursorPosX(start_x);
    ImGui::SetCursorPosY(buttons_y);
    if (ImGui::Button("返回", ImVec2(button_width, button_height))) {
        PageManager::getInstance().setCurrentPage(Page::MainMenu);
        // 重置状态
        was_ip_active = false;
        strcpy(ip_buffer, "127.0.0.1");
        ip_address = "127.0.0.1";
        memset(port_buffer, 0, MAX_INPUT_LENGTH);
        port.clear();
    }

    // 加入按钮
    ImGui::SetCursorPosX(start_x + button_width + button_spacing);
    ImGui::SetCursorPosY(buttons_y);
    if (ImGui::Button("加入", ImVec2(button_width, button_height))) {
        // 检查输入是否有效
        if (ip_address.empty() || port.empty()) {
            std::thread([]{
                PageManager::getInstance().setCurrentPage(Page::Console);
                ConsolePage::print("错误：IP地址和端口号不能为空");
            }).detach();
            return;
        }

        // 显示输入值
        std::thread([this]{
            PageManager::getInstance().setCurrentPage(Page::Console);
            ConsolePage::print("尝试连接到服务器：");
            ConsolePage::print("IP地址: " + ip_address);
            ConsolePage::print("端口号: " + port);
            UIManager::getInstance().join_room();  // 暂时注释掉实际的加入操作
        }).detach();
    }

    ImGui::PopFont();
} 