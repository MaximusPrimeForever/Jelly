#pragma once

#include "imgui.h"
#include "superglue.h"

#include "graphics/render_target.h"
#include "graphics/camera.h"

#include "widgets/image_compositor.h"
#include "widgets/settings_menu.h"

typedef enum RENDER_TARGET_ENUM_T
{
	TEXTURED_RECTANGLE = 0,
	AWESOME_RECTANGLE,
	AWESOME_CUBE,
	AWESOME_CUBE_FIELD,

	// Auto count
	RENDER_TARGET_COUNT
} RENDER_TARGET_ENUM;


typedef struct Monitor_t
{
	int width;
	int height;
	float width_dpi_scale;
	float height_dpi_scale;
} Monitor;

typedef struct Mouse_t
{
	float last_x;
	float last_y;
	float delta_x;
	float delta_y;
	float sensitivity;
} Mouse;

class MainWindow
{
private:
	GLFWwindow* window;
	const char* glsl_version;
	Monitor monitorDimensions;
	ImVec4 background_color;

	// Settings
	bool disable_cursor;
	SettingsMenu* settings_menu;

	// Widgets
	ImageCompositor* im_comp;
	ImFont* font;

	// OpenGL
	Mouse mouse;
	Camera* camera;
	RenderTarget* render_targets[RENDER_TARGET_COUNT] = { 0 };
	float mix_value = 0.5;

	void InitializeGlfw();
	void InitializeImGui();
	
	bool GetMonitorDimensions();
	void ProcessInput();
	void ShowImGui();
	void RenderOpenGL();
	void SetupOpenGL();

public:
	MainWindow();
	~MainWindow();

	void UpdateCameraFromMouse();
	bool ShouldClose();
	void Show();
	void InitializeSettings();
};

inline Mouse g_mouse;
void MouseCallback(GLFWwindow* window, double xpos, double ypos);
