#pragma once
#include "widgets/base_widget.h"

class SettingsMenu : BaseWidget
{
public:
    bool enableWireframe;
    bool showImageCompositor;

    SettingsMenu(double x_pos, double y_pos, ImFont* font);
    void Show() override;
};
