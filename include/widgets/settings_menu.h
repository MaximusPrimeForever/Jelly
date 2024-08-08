#pragma once
#include "widgets/base_widget.h"

class SettingsMenu : BaseWidget
{
public:
    bool enable_wireframe;
    bool enable_depth_testing;
    bool show_image_compositor;
    float shift_x;
    float shift_y;
    float shift_z;

    SettingsMenu(double x_pos, double y_pos, ImFont* font);
    void Show() override;
};
