#pragma once
#include "PageRenderer.h"
#include "../PageManager.h"
#include <string>

class JoinRoomPage : public PageRenderer {
public:
    JoinRoomPage() : ip_address("127.0.0.1"), port(""), was_ip_active(false) {
        strcpy(ip_buffer, "127.0.0.1");
        memset(port_buffer, 0, MAX_INPUT_LENGTH);
    }
    void render(float window_width, float window_height, float scale_factor) override;
    
    // 获取输入值的静态方法
    static const std::string& getIpAddress() { return getInstance().ip_address; }
    static const std::string& getPort() { return getInstance().port; }
    static JoinRoomPage& getInstance() {
        static JoinRoomPage instance;
        return instance;
    }

private:
    std::string ip_address;
    std::string port;
    bool was_ip_active;
    
    static const int MAX_INPUT_LENGTH = 64;
    char ip_buffer[64];
    char port_buffer[64];
}; 