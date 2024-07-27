#include "defs.h"
#include "window_init.h"

#include <GLFW/glfw3.h>

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