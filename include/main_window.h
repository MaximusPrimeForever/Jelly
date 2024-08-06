#pragma once

#include "imgui.h"
#include "superglue.h"

#include "graphics/shaders.h"
#include "graphics/render_target.h"

#include "widgets/image_compositor.h"
#include "widgets/settings_menu.h"

typedef enum RENDER_TARGET_ENUM_T
{
	TEXTURED_RECTANGLE = 0,
	AWESOME_RECTANGLE,
	RENDER_TARGET_COUNT
} RENDER_TARGET_ENUM;


typedef struct Monitor_t
{
	int width;
	int height;
	float width_dpi_scale;
	float height_dpi_scale;
} Monitor;

class MainWindow
{
private:
	GLFWwindow* window;
	const char* glsl_version;
	Monitor monitorDimensions;
	ImVec4 background_color;

	// Settings
	SettingsMenu* settings_menu;

	// Widgets
	ImageCompositor* im_comp;
	ImFont* font;

	// OpenGL
	RenderTarget* render_targets[RENDER_TARGET_COUNT] = { 0 };
	float mix_value = 0.0;

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
	bool ShouldClose();
	void Show();
};
