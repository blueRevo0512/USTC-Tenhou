#include "HelpPage.h"
#include "../MainWindow.h"

void HelpPage::render(float window_width, float window_height, float scale_factor) {
    float text_y = window_height * 0.05f;
    float line_spacing = ImGui::GetTextLineHeight() * 1.1f;
    float button_height = 50 * scale_factor;
    float button_y = window_height * 0.9f;
    float button_margin = button_height * 0.5f;

    struct TextLine {
        const char* text;
        bool center;
    };

    const TextLine help_text[] = {
        {"这是一款基于天凤规则的四人联机立直麻将游戏", true},
        {"", true},
        {"游戏规则：", true},
        {"您可以在以下网址查看天凤麻将的具体规则：", true},
        {"• https://zh.wikipedia.org/wiki/日本麻雀", true},
        {"• https://zh.wikipedia.org/wiki/天鳳_(麻將)", true},
        {"", true},
        {"在 USTC 天凤中，我们添加了三个新的役满：", true},
        {"1. 红专并进（双倍役满）", true},
        {"使用包含5m、5p、5s、7z的四副刻子或杠子和牌", true},
        {"对于5m、5p、5s，必须包含赤宝牌", true},
        {"2. 又红又专（役满）", true},
        {"红专并进中的一副刻子或杠子改为对子", true},
        {"对于5m、5p、5s，仍需包含赤宝牌", true},
        {"3. 理实交融（役满）", true},
        {"使用包含数牌中1、9、5、8的四副刻子或杠子和牌", true},
        {"", true},
        {"祝您游戏愉快！", true}
    };

    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
    ImGui::SetWindowFontScale(scale_factor * 0.7f);

    for (const auto& line : help_text) {
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