#pragma once
#include "PageRenderer.h"
#include "../PageManager.h"
#include "ConsolePage.h"
#include <string>

class CreateRoomPage : public PageRenderer {
public:
    CreateRoomPage() : selected_length(0), port_choice(0), port("") {
        memset(port_buffer, 0, MAX_INPUT_LENGTH);
    }
    void render(float window_width, float window_height, float scale_factor) override;
    
    // 获取选择的值的静态方法
    static int getGameLength() { return getInstance().selected_length + 1; }  // 1-4
    static std::string getPortChoice() { 
        // 添加调试输出
        ConsolePage::print("Debug - Current port_choice value: " + std::to_string(getInstance().port_choice));
        // 修改这里，修复返回值
        return getInstance().port_choice == 0 ? "2" : "1";  // 0->random("2"), 1->specified("1")
    }
    static const std::string& getPort() { 
        // 添加调试输出
        ConsolePage::print("Debug - Port choice: " + std::to_string(getInstance().port_choice));
        ConsolePage::print("Debug - Port value: " + getInstance().port);
        return getInstance().port; 
    }
    static CreateRoomPage& getInstance() {
        static CreateRoomPage instance;
        return instance;
    }

private:
    int selected_length;  // 0: 一局, 1: 东风, 2: 半庄, 3: 全庄
    int port_choice;      // 0: random, 1: specified
    std::string port;
    bool is_combo_open = false;
    
    static const int MAX_INPUT_LENGTH = 64;
    char port_buffer[64];
    
    const char* game_lengths[4] = {"一局", "东风", "半庄", "全庄"};
}; 