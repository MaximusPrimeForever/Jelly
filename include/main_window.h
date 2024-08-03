#pragma once

#include "imgui.h"
#include "superglue.h"

#include "widgets/image_compositor.h"
#include "widgets/settings_menu.h"
#include "io/shaders.h"

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
	GLuint VAO[2];
	ShaderProgram* shader_program;


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
