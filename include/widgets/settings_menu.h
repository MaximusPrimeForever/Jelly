#pragma once
#include "widgets/base_widget.h"

class SettingsMenu : BaseWidget
{
public:
    bool enable_wireframe;
    bool show_image_compositor;
    float horizontal_offset;

    SettingsMenu(double x_pos, double y_pos, ImFont* font);
    void Show() override;
};
