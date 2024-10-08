#pragma once

#include "imgui.h"

#define ASPECT_RATIO (16.0f/10.0f)

#define WINDOW_WIDTH_REL_SCALE (0.8f)
#define WINDOW_HEIGHT_REL_SCALE (0.8f)
#define FONT_SIZE (16)

#define DEFAULT_BG_IMAGE_PATH ".\\imgs\\background.png"
#define DEFAULT_FG_IMAGE_PATH ".\\imgs\\teapot.png"
#define APP_NAME "Jelly"

inline ImFont* g_app_font;

// Camera settings
/// FOV
#define CAMERA_MIN_VERTICAL_FOV (1.0f)
#define CAMERA_MAX_VERTICAL_FOV (120.0f)
#define CAMERA_DEFAULT_VERTICAL_FOV (80.0f)

#define CAMERA_DEFAULT_VELOCITY (0.005f)

/// Yaw/Pitch
#define CAMERA_BOTTOM_PITCH_LIMIT (-89.0f)
#define CAMERA_TOP_PITCH_LIMIT (89.0f)
#define CAMERA_DEFAULT_PITCH (-10.0f)
#define CAMERA_DEFAULT_YAW (-110.0f)

/// Draw distances
#define CAMERA_MIN_DRAW_DISTANCE (0.01f)
#define CAMERA_MAX_DRAW_DISTANCE (100.0f)

// Mouse settings
#define MOUSE_MIN_SENSITIVITY (0.001f)
#define MOUSE_MAX_SENSITIVITY (1.0f)
#define MOUSE_DEFAULT_SENSITIVITY (0.05f)
#define MOUSE_DRAG_SENSITIVITY (0.09f)
#define MOUSE_SCROLL_SENSITIVITY (2.0f)

// Keyboard settings
#define KEYBOARD_PRESS_TIMEOUT_SECS (0.100)
