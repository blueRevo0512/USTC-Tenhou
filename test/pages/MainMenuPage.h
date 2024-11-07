#pragma once
#include "PageRenderer.h"
#include "../PageManager.h"

class MainMenuPage : public PageRenderer {
public:
    void render(float window_width, float window_height, float scale_factor) override;
    static MainMenuPage& getInstance() {
        static MainMenuPage instance;
        return instance;
    }
}; 