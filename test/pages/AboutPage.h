#pragma once
#include "PageRenderer.h"
#include "../PageManager.h"

class AboutPage : public PageRenderer {
public:
    void render(float window_width, float window_height, float scale_factor) override;
    static AboutPage& getInstance() {
        static AboutPage instance;
        return instance;
    }
}; 