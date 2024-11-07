#pragma once

enum class Page {
    MainMenu,
    About,
    Help,
    CreateRoom,
    JoinRoom,
    Console
};

class PageManager {
public:
    static PageManager& getInstance() {
        static PageManager instance;
        return instance;
    }
    
    void setCurrentPage(Page page) { current_page = page; }
    Page getCurrentPage() const { return current_page; }

private:
    PageManager() = default;
    Page current_page = Page::MainMenu;
}; 