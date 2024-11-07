#pragma once
#include "imgui.h"
#include <GLFW/glfw3.h>

class PageRenderer {
public:
    virtual void render(float window_width, float window_height, float scale_factor) = 0;
    virtual ~PageRenderer() = default;
}; 