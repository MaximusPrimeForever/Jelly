#include "defs.h"
#include "window_utils.h"
#include "stb_image.h"

#include <GLFW/glfw3.h>

#include "settings.h"
#include "blending.h"

bool GetMonitorDimensions(OUT int* width, OUT int* height, OUT float* width_scale, OUT float* height_scale)
{
    if (NULL == width_scale || NULL == height_scale) return false;

    GLFWmonitor* primary_monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primary_monitor);

    glfwGetMonitorContentScale(primary_monitor, width_scale, height_scale);
    *width = mode->width;
    *height = mode->height;
    return true;
}

bool LoadDefaultImages(Image* bg_image, Image* fg_image)
{
    Image bg_image;
    unsigned char* bg_img = stbi_load(DEFAULT_BG_IMAGE_PATH, &bg_image->width, &bg_image->height, NULL, 4);
    if (bg_img == NULL) { return false; }

    bg_image->opacity = 1.0;
    bg_image->image_data = bg_img;

    // Load teapot
    Image fg_image;
    unsigned char* fg_img = stbi_load(DEFAULT_FG_IMAGE_PATH, &fg_image->width, &fg_image->height, NULL, 4);
    if (fg_img == NULL) { return false; }

    fg_image->opacity = 1.0;
    fg_image->image_data = fg_img;

    return true;
}

void ProcessInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}