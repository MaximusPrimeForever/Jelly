#pragma once
#include <iostream>
#include "base_widget.h"
#include "blending.h"
#include "settings.h"

#define BG_IMG_ID (1)
#define FG_IMG_ID (2)
#include "imfilebrowser.h"

#include <imgui_impl_opengl3.h>

class ImageCompositor : BaseWidget
{
private:
	Image fg_image;
	Image bg_image;
    Image blended_image;

    int fg_image_pos_x;
    int fg_image_pos_y;

    GLuint blended_texture;
    const char* blending_modes_strings[BLENDING_MODE_COUNT];
    const BlendingMode blending_modes_enum[BLENDING_MODE_COUNT];
    int current_blending_mode;

    ImGui::FileBrowser file_dialog;
    bool LoadDefaultImages();

public:
	ImageCompositor(double x_pos, double y_pos, ImFont* font, bool should_load_default_images);
    bool BlendRawImages(
        IN Image* bg_image,
        IN Image* fg_image,
        IN uint32_t fg_pos_x,
        IN uint32_t fg_pos_y,
        IN BlendingMode mode,
        OUT Image* final_image
    );
    bool BlendTextures(GLuint* in_texture_1, GLuint* in_texture_2, GLuint* out_texture);
    void Show() override;
    bool have_ui_values_changed;
};
