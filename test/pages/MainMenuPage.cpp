#include "MainMenuPage.h"
#include "../MainWindow.h"

void MainMenuPage::render(float window_width, float window_height, float scale_factor) {
    float title_area_height = window_height * 0.4f;
    float content_start_y = title_area_height;
    
    // 标题区域（占窗口高度的40%）
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
    
    // 标题
    const char* title = "欢迎来到 USTC 天凤!";
    ImGui::SetWindowFontScale(scale_factor * 2.0f);
    float title_width = ImGui::CalcTextSize(title).x;
    float title_height = ImGui::GetFontSize();
    ImGui::SetCursorPosX((window_width - title_width) * 0.5f);
    ImGui::SetCursorPosY((title_area_height - title_height * 2.5f) * 0.5f);
    ImGui::TextUnformatted(title);

    // 副标题
    const char* subtitle = "立直麻将のSEKAI与无法和牌の蝌蝻";
    ImGui::SetWindowFontScale(scale_factor * 1.2f);
    float subtitle_width = ImGui::CalcTextSize(subtitle).x;
    ImGui::SetCursorPosX((window_width - subtitle_width) * 0.5f);
    ImGui::SetCursorPosY((title_area_height + title_height * 0.5f) * 0.5f);
    ImGui::TextUnformatted(subtitle);

    // 按钮区域（占窗口高度的55%，留出5%的底部空间）
    float button_area_height = window_height * 0.55f;
    float button_width = 300 * scale_factor;
    float button_height = 60 * scale_factor;
    float total_buttons_height = button_height * 5 + (4 * button_height * 0.5f);
    float buttons_start_y = content_start_y + (button_area_height - total_buttons_height) * 0.5f;

    // 按钮样式
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(20 * scale_factor, 10 * scale_factor));
    ImGui::SetWindowFontScale(scale_factor * 1.2f);

    const char* button_labels[] = {"创建房间", "加入房间", "游戏帮助", "关于", "退出"};
    for (int i = 0; i < 5; i++) {
        ImGui::SetCursorPosX((window_width - button_width) * 0.5f);
        ImGui::SetCursorPosY(buttons_start_y + i * (button_height * 1.5f));
        
        if (ImGui::Button(button_labels[i], ImVec2(button_width, button_height))) {
            switch (i) {
                case 0: // 创建房间
                    PageManager::getInstance().setCurrentPage(Page::CreateRoom);
                    break;
                case 1: // 加入房间
                    PageManager::getInstance().setCurrentPage(Page::JoinRoom);
                    break;
                case 2: // 游戏帮助
                    PageManager::getInstance().setCurrentPage(Page::Help);
                    break;
                case 3: // 关于
                    PageManager::getInstance().setCurrentPage(Page::About);
                    break;
                case 4: // 退出
                    MainWindow::closeWindow();
                    break;
            }
        }
    }

    ImGui::PopStyleVar();
    ImGui::PopFont();
} 