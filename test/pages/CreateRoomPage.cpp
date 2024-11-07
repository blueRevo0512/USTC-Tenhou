#include "CreateRoomPage.h"
#include "ConsolePage.h"
#include "../MainWindow.h"
#include "../UIManager.h"
#include <thread>

void CreateRoomPage::render(float window_width, float window_height, float scale_factor) {
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);

    // 标题
    const char* title = "创建房间";
    ImGui::SetWindowFontScale(scale_factor * 1.5f);
    float title_width = ImGui::CalcTextSize(title).x;
    ImGui::SetCursorPosX((window_width - title_width) * 0.5f);
    ImGui::SetCursorPosY(window_height * 0.15f);
    ImGui::TextUnformatted(title);

    // 游戏长度选择区域
    float combo_width = 200 * scale_factor;
    float combo_height = 40 * scale_factor;
    float label_height = ImGui::GetTextLineHeight();
    float vertical_spacing = window_height * 0.05f;
    float start_y = window_height * 0.3f;

    ImGui::SetWindowFontScale(scale_factor);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));

    // 游戏长度标签
    ImGui::SetCursorPosX((window_width - combo_width) * 0.5f);
    ImGui::SetCursorPosY(start_y);
    ImGui::TextUnformatted("游戏长度：");
    
    // 下拉选择框
    ImGui::SetCursorPosX((window_width - combo_width) * 0.5f);
    ImGui::SetCursorPosY(start_y + label_height + 5);
    ImGui::PushItemWidth(combo_width);
    if (ImGui::BeginCombo("##game_length", game_lengths[selected_length])) {
        for (int i = 0; i < 4; i++) {
            bool is_selected = (selected_length == i);
            if (ImGui::Selectable(game_lengths[i], is_selected)) {
                selected_length = i;
            }
            if (is_selected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    // 端口选择区域
    float radio_y = start_y + combo_height + vertical_spacing * 2;
    float input_width = 200 * scale_factor;

    // 随机端口选项
    ImGui::SetCursorPosX((window_width - combo_width) * 0.5f);
    ImGui::SetCursorPosY(radio_y);
    if (ImGui::RadioButton("随机端口", port_choice == 0)) {
        port_choice = 0;
    }

    // 指定端口选项
    ImGui::SetCursorPosX((window_width - combo_width) * 0.5f);
    ImGui::SetCursorPosY(radio_y + label_height + 10);
    if (ImGui::RadioButton("指定端口", port_choice == 1)) {
        port_choice = 1;
    }

    // 端口输入框（仅在选择指定端口时显示）
    if (port_choice == 1) {
        ImGui::SetCursorPosX((window_width - input_width) * 0.5f);
        ImGui::SetCursorPosY(radio_y + label_height * 2 + 20);
        if (ImGui::InputText("##port", port_buffer, MAX_INPUT_LENGTH)) {
            port = std::string(port_buffer);
        }
    }

    ImGui::PopItemWidth();
    ImGui::PopStyleVar();

    // 按钮
    float button_width = 150 * scale_factor;
    float button_height = 40 * scale_factor;
    float button_spacing = 50 * scale_factor;
    float buttons_y = window_height * 0.75f;

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
        selected_length = 0;
        port_choice = 0;
        memset(port_buffer, 0, MAX_INPUT_LENGTH);
        port.clear();
    }

    // 创建按钮
    ImGui::SetCursorPosX(start_x + button_width + button_spacing);
    ImGui::SetCursorPosY(buttons_y);
    if (ImGui::Button("创建", ImVec2(button_width, button_height))) {
        // 在创建房间前打印调试信息
        ConsolePage::print("游戏长度: " + std::to_string(selected_length + 1));
        ConsolePage::print("端口选择: " + std::string(port_choice == 0 ? "随机" : "指定"));
        if (port_choice == 1) {
            ConsolePage::print("指定端口: " + port);
        }

        // 创建新线程来处理房间创建和页面切换
        std::thread([this]{
            PageManager::getInstance().setCurrentPage(Page::Console);
            UIManager::getInstance().create_room();
        }).detach();
    }

    ImGui::PopFont();
} 