#pragma once
#include "PageRenderer.h"
#include "../PageManager.h"

class HelpPage : public PageRenderer {
public:
    void render(float window_width, float window_height, float scale_factor) override;
    static HelpPage& getInstance() {
        static HelpPage instance;
        return instance;
    }
}; 