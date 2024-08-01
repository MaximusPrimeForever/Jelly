
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

#include <malloc.h>
#include <algorithm>

#include "widgets/image_compositor.h"

ImageCompositor::ImageCompositor(double x_pos, double y_pos, ImFont* font, bool should_load_default_images)
    :BaseWidget(x_pos, y_pos, font),
    blending_modes_strings{ "Normal", "Add", "Multiply", "Screen" },
    blending_modes_enum{ BlendingMode::NORMAL, BlendingMode::ADD, BlendingMode::MULTIPLY, BlendingMode::SCREEN },
    fg_image_pos_x{ 0 }, fg_image_pos_y{ 0 }
{
    if (should_load_default_images) {
        if (!this->LoadDefaultImages()) {
            throw std::exception("Failed to load default images. Aborting..");
        }
    }
    else {
        throw std::exception("TODO: implement logic for when default images are not loaded. Aborting..");
    }

    this->blended_texture = 0;
    glGenTextures(1, &this->blended_texture);

    // Setup filtering parameters for display
    glBindTexture(GL_TEXTURE_2D, blended_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

    // initialize blended result to background image as a default
    blended_image.width = bg_image.width;
    blended_image.height = bg_image.height;
    unsigned char* blended_image_data = (unsigned char*)malloc(blended_image.width * blended_image.height * RGBA_SIZE);
    if (blended_image_data == NULL) {
        throw std::exception("Failed to allocate memory for blended image. Aborting..");
    }
    blended_image.image_data = blended_image_data;

    this->current_blending_mode = 0;
    // Create a texture to display blended image
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA,
        blended_image.width,
        blended_image.height,
        0, GL_RGBA, GL_UNSIGNED_BYTE,
        blended_image.image_data
    );

    //ImGui::FileBrowser file_dialog;
    bool have_ui_values_changed = false;
    BlendRawImages(
        &bg_image,
        &fg_image,
        this->fg_image_pos_x,
        this->fg_image_pos_y,
        blending_modes_enum[current_blending_mode],
        &blended_image
    );
}

void ImageCompositor::Show()
{
    this->have_ui_values_changed = false;

    ImGui::PushFont(this->font);
    ImGui::Begin("OpenGL Image Blender");

    if (ImGui::Button("Select background image...")) { this->file_dialog.Open(BG_IMG_ID); }
    if (ImGui::Button("Select foreground image...")) { this->file_dialog.Open(FG_IMG_ID); }

    this->file_dialog.Display();
    if (this->file_dialog.HasSelected()) {
        int width, height;
        int selected_file = -1;
        std::string filename = this->file_dialog.GetSelected(&selected_file).string();
        unsigned char* data = stbi_load(filename.c_str(), &width, &height, NULL, RGBA_SIZE);

        if (selected_file == FG_IMG_ID) {
            if (NULL != data) {
                fg_image.width = width;
                fg_image.height = height;
                fg_image.image_data = data;
            }
        }

        if (selected_file == BG_IMG_ID) {
            if (NULL != data) {
                bg_image.width = width;
                bg_image.height = height;
                bg_image.image_data = data;
            }
        }

        this->file_dialog.ClearSelected();
        this->have_ui_values_changed |= true;
    }

    ImGui::Text("pointer = %x", blended_texture);
    ImGui::Text("size = %d x %d", blended_image.width, blended_image.height);

    this->have_ui_values_changed |= ImGui::SliderFloat("FG image opacity", &fg_image.opacity, 0.0f, 1.0f);
    this->have_ui_values_changed |= ImGui::SliderInt("FG image x", &this->fg_image_pos_x, -1 * bg_image.width, bg_image.width);
    this->have_ui_values_changed |= ImGui::SliderInt("FG image y", &this->fg_image_pos_y, -1 * bg_image.height, bg_image.width);
    this->have_ui_values_changed |= ImGui::ListBox("listbox", &this->current_blending_mode, blending_modes_strings, IM_ARRAYSIZE(blending_modes_strings));

    if (this->have_ui_values_changed) {
        this->BlendRawImages(
            &bg_image, &fg_image,
            fg_image_pos_x, fg_image_pos_y,
            blending_modes_enum[this->current_blending_mode],
            &blended_image
        );
    }
    // Update texture to new blended image
    glBindTexture(GL_TEXTURE_2D, blended_texture);
    glTexSubImage2D(
        GL_TEXTURE_2D, 0, 0, 0,
        blended_image.width, blended_image.height,
        GL_RGBA, GL_UNSIGNED_BYTE, blended_image.image_data
    );
    ImGui::Image((void*)(intptr_t)blended_texture, ImVec2(blended_image.width, blended_image.height));
    ImGui::End();
    ImGui::PopFont();
}

bool ImageCompositor::LoadDefaultImages()
{
    unsigned char* bg_img = stbi_load(
        DEFAULT_BG_IMAGE_PATH,
        &this->bg_image.width,
        &this->bg_image.height,
        NULL, 4
    );
    if (bg_img == NULL) { return false; }

    bg_image.opacity = 1.0;
    bg_image.image_data = bg_img;

    // Load teapot
    unsigned char* fg_img = stbi_load(
        DEFAULT_FG_IMAGE_PATH,
        &this->fg_image.width,
        &this->fg_image.height,
        NULL, 4
    );
    if (fg_img == NULL) { return false; }

    fg_image.opacity = 1.0;
    fg_image.image_data = fg_img;

    return true;
}

bool ImageCompositor::BlendTextures(GLuint* in_texture_1, GLuint* in_texture_2, GLuint* out_texture)
{
    // TBD

    return true;
}

// Take bg and fg images, blend them, and return the result
// Output image will take dimensions from the larger of input images
bool ImageCompositor::BlendRawImages(
    IN Image* bg_image,
    IN Image* fg_image,
    IN uint32_t fg_pos_x,
    IN uint32_t fg_pos_y,
    IN BlendingMode mode,
    OUT Image* final_image
) {
    // assume background image is the canvas
    memcpy(final_image->image_data, bg_image->image_data, bg_image->width * bg_image->height * RGBA_SIZE);

    // fg image pass
    for (size_t i = 0; i < fg_image->height; i++) {
        for (size_t j = 0; j < fg_image->width; j++) {
            int x_offset_final_image = fg_pos_x + j;
            int y_offset_final_image = (fg_pos_y + i);

            // if computed x offset is outside bg image, do nothing
            if (x_offset_final_image != std::clamp(x_offset_final_image, 0, (int)bg_image->width - 1)) {
                continue;
            }
            // same for y
            if (y_offset_final_image != std::clamp(y_offset_final_image, 0, (int)bg_image->height - 1)) {
                continue;
            }

            int bg_pixel_index = (y_offset_final_image * final_image->width + x_offset_final_image) * RGBA_SIZE;
            int blended_pixel_index = bg_pixel_index;
            int fg_pixel_index = (i * fg_image->width + j) * RGBA_SIZE;

            double fg_alpha = (fg_image->image_data[fg_pixel_index + RGBA_SIZE - 1] / 255.0) * fg_image->opacity;

            // otherwise, do blending operation
            for (size_t k = 0; k < RGBA_SIZE - 1; k++) {
                // perform blending mode computation
                double blended_channel = 0;
                double fg_current_channel = fg_image->image_data[fg_pixel_index + k] / 255.0;
                double bg_current_channel = bg_image->image_data[bg_pixel_index + k] / 255.0;

                blended_channel = 0;
                switch (mode)
                {
                case BlendingMode::ADD:
                    blended_channel = fg_current_channel * fg_alpha + bg_current_channel;
                    break;
                case BlendingMode::MULTIPLY:
                    blended_channel = fg_alpha * (bg_current_channel * fg_current_channel) + (1 - fg_alpha) * bg_current_channel;
                    break;
                case BlendingMode::SCREEN:
                case BlendingMode::NORMAL:
                default:
                    blended_channel = fg_current_channel;
                    break;
                }
                final_image->image_data[blended_pixel_index + k] = (unsigned char)(std::clamp(blended_channel, 0.0, 1.0) * 255);
            }
        }
    }

    return true;
}
