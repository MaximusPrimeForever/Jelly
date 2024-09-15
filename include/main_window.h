#pragma once

#include <functional>

#include "imgui.h"
#include "superglue.h"

#include "graphics/render_target.h"
#include "graphics/camera.h"

#include "widgets/settings_menu.h"


typedef struct RenderEntry_t
{
	RenderTarget* target;
	bool* should_render;
} RenderEntry;

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
	float scroll_y_offset;
	bool is_visible;
} Mouse;

class MainWindow
{
private:
	// Window
	GLFWwindow* window;
	const char* glsl_version;
	Monitor monitorDimensions;
	ImVec4 background_color;

	// Settings
	SettingsMenu* settings_menu;
	double key_last_pressed_time;

	// Widgets
	ImFont* font;

	// OpenGL
	Mouse mouse;
	Camera* camera;
	std::vector<RenderEntry> render_targets;

	void InitializeGlfw();
	void InitializeImGui();
	
	bool GetMonitorDimensions();
	void ProcessInput();
	void ShowImGui() const;
	void RenderOpenGL() const;
	void SetupOpenGL();

	void FrameBufferResizeCallback(GLFWwindow* window, int width, int height);

public:
	MainWindow();
	~MainWindow();

	void UpdateCameraFromMouse(float frame_time);
	void UpdateCameraFromKeyboard(float frame_time) const;
	bool ShouldClose();
	void Show();
	void InitializeSettings();
	glm::vec3 GetRayFromScreenSpace(GLuint x, GLuint y);

	std::function<void(GLFWwindow*, int, int)> window_resize_callback;
};

inline Mouse g_mouse = {0.0f, 0.0f, 0.0f, true};
void MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
