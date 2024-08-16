#pragma once

#include "imgui.h"
#include "superglue.h"

#include "graphics/render_target.h"
#include "graphics/camera.h"

#include "widgets/settings_menu.h"

typedef enum RENDER_TARGET_ENUM_T
{
	TEXTURED_RECTANGLE = 0,
	AWESOME_RECTANGLE,
	AWESOME_CUBE,
	AWESOME_CUBE_FIELD,
	LET_THERE_BE_LIGHT,
	LIT_CONTAINER_PARTY,

	// Grid is drawn last due to transparency
	XZ_GRID,
	// Auto count
	RENDER_TARGET_COUNT
} RENDER_TARGET_ENUM;

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
	RenderEntry render_targets[RENDER_TARGET_COUNT] = { nullptr };

	void InitializeGlfw();
	void InitializeImGui();
	
	bool GetMonitorDimensions();
	void ProcessInput();
	void ShowImGui() const;
	void RenderOpenGL() const;
	void SetupOpenGL();

public:
	MainWindow();
	~MainWindow();

	void UpdateCameraFromMouse(float frame_time);
	void UpdateCameraFromKeyboard(float frame_time) const;
	bool ShouldClose();
	void Show();
	void InitializeSettings();
};

inline Mouse g_mouse;
void MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
