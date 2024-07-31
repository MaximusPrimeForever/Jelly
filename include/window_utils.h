#pragma once

#include "defs.h"

bool GetMonitorDimensions(OUT int* width, OUT int* height, OUT float* width_scale, OUT float* height_scale);

bool LoadDefaultImages(Image* bg_image, Image* fg_image);

void ProcessInput(GLFWwindow* window);
