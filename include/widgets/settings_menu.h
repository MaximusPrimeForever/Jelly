#pragma once
#include "widgets/base_widget.h"

class SettingsMenu : BaseWidget
{
public:
    bool enable_wireframe;
    bool enable_depth_testing;
    bool show_image_compositor;
    float vfov;
    float shift_x;
    float shift_y;
    float shift_z;
    bool show_textured_rect;
    bool show_awesome_rect;
    bool show_awesome_cube;
    bool show_awesome_cube_field;

    bool has_ui_updated;

    SettingsMenu(double x_pos, double y_pos, ImFont* font);
    void Show() override;
};
