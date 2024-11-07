#pragma once
#include "PageRenderer.h"
#include "../PageManager.h"
#include <string>
#include <deque>
#include <mutex>
#include <condition_variable>

class ConsolePage : public PageRenderer {
public:
    ConsolePage();
    void render(float window_width, float window_height, float scale_factor) override;
    
    // 静态方法供其他代码使用
    static void print(const std::string& text);
    static void clear();
    static std::string getInput();
    static ConsolePage& getInstance() {
        static ConsolePage instance;
        return instance;
    }

private:
    static std::deque<std::string> messages;  // 静态消息缓冲区
    static std::mutex messages_mutex;         // 静态互斥锁
    static std::condition_variable input_cv;  // 条件变量用于等待输入
    
    char input_buffer[256];
    std::string current_input;
    bool input_ready;  // 标志输入是否准备好
}; 