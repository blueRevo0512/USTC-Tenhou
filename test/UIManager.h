#pragma once
#include <string>

class UIManager {
public:
    static UIManager& getInstance();
    void start_ui();
    void create_room();
    void join_room();
    
private:
    UIManager() = default;
    void show_help();
    void show_info();
}; 