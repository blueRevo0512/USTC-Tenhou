#include "AboutPage.h"
#include "../MainWindow.h"

void AboutPage::render(float window_width, float window_height, float scale_factor) {
    float text_y = window_height * 0.05f;
    float line_spacing = ImGui::GetTextLineHeight() * 1.1f;
    float button_height = 50 * scale_factor;
    float button_y = window_height * 0.9f;
    float button_margin = button_height * 0.5f;  // 按钮上方的边距改小一些

    struct TextLine {
        const char* text;
        bool center;
    };

    const TextLine about_text[] = {
        {"本项目基于 https://github.com/Agony5757/mahjong", true},
        {"", true},
        {"项目使用了以下工具和库：", true},
        {"• ImGUI、boost/asio、CMake、git、其他开源工具", false},
        {"", true},
        {"特别感谢：", true},
        {"• 韩东起（MSRA、OIST、USTC）", false},
        {"• Agony（USTC）", false},
        {"• 以及 https://github.com/Agony5757/mahjong 的其他贡献者", false},
        {"", true},
        {"开发者：", true},
        {"• 金天昊（USTC）、邹弘熙（USTC）", true},
        {"", true},
        {"如果您遇到任何问题或想为项目做出贡献，请通过以下方式联系我们：", true},
        {"• QQ：2328036454", false},
        {"• 邮箱：jintianhao@mail.ustc.edu.cn", false},
        {"", true},
        {"感谢您游玩本游戏！", true}
    };

    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
    ImGui::SetWindowFontScale(scale_factor * 0.7f);

    for (const auto& line : about_text) {
        if (text_y + line_spacing > button_y - button_margin) {
            break;
        }

        float text_width = ImGui::CalcTextSize(line.text).x;
        float text_x;
        
        if (line.center) {
            text_x = (window_width - text_width) * 0.5f;
        } else {
            text_x = window_width * 0.35f;
        }

        ImGui::SetCursorPosX(text_x);
        ImGui::SetCursorPosY(text_y);
        ImGui::TextUnformatted(line.text);
        text_y += line_spacing;
    }

    // 返回按钮
    float button_width = 200 * scale_factor;
    ImGui::SetWindowFontScale(scale_factor * 1.2f);
    ImGui::SetCursorPosX((window_width - button_width) * 0.5f);
    ImGui::SetCursorPosY(button_y);
    
    if (ImGui::Button("返回", ImVec2(button_width, button_height))) {
        PageManager::getInstance().setCurrentPage(Page::MainMenu);
    }

    ImGui::PopFont();
} 