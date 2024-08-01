#pragma once

#include "defs.h"
#include "imgui.h"

#include "widgets/image_compositor.h"

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
	Monitor MonitorDimensions;
	ImVec4 background_color;

	// Widgets
	ImageCompositor* im_comp;
	ImFont* font;

	void InitializeGlfw();
	void InitializeImGui();
	
	bool GetMonitorDimensions();
	void ProcessInput();
public:
	MainWindow();
	~MainWindow();
	bool ShouldClose();
	void Show();
};
