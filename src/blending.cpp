// STB
#include "stb_image.h"
#include "superglue.h"

#include "blending.h"

#include <malloc.h>
#include <algorithm>

// Simple helper function to load an image into a OpenGL texture with common settings
bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height)
{
    // Load from file
    int image_width = 0;
    int image_height = 0;
    unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
    if (image_data == NULL)
        return false;

    // Create a OpenGL texture identifier
    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

    // Upload pixels into texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    stbi_image_free(image_data);

    *out_texture = image_texture;
    *out_width = image_width;
    *out_height = image_height;

    return true;
}

// Simple helper function to load an image into a OpenGL texture with common settings
bool LoadTextureFromStbBuffer(IN unsigned char* image_data, IN int img_width, IN int img_height, OUT GLuint* out_texture)
{
    // Create a OpenGL texture identifier
    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

    // Upload pixels into texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_width, img_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    //stbi_image_free(image_data);

    *out_texture = image_texture;
    return true;
}

bool BlendTextures(GLuint* in_texture_1, GLuint* in_texture_2, GLuint* out_texture)
{
    // TBD

    return true;
}

// Take bg and fg images, blend them, and return the result
// Output image will take dimensions from the larger of input images
bool BlendRawImages(
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
