#include <cstdio>
#include <iostream>
#include <fstream>

#include "superglue.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imfilebrowser.h"

#include "settings.h"
#include "main_window.h"

#include <functional>
#include <stb_image.h>

// Render targets
#include "graphics/examples/texured_rectangle.h"
#include "graphics/examples/awesome_rectangle.h"
#include "graphics/examples/awesome_cube.h"
#include "graphics/examples/awesome_cube_field.h"
#include "graphics/examples/let_there_be_light.h"

#include "graphics/camera.h"


MainWindow::MainWindow()
{
	this->InitializeGlfw();

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		throw std::exception("Failed to initialize GLAD. Aborting..");
	}
	this->InitializeImGui();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(this->window, true);
	ImGui_ImplOpenGL3_Init(this->glsl_version);

	this->InitializeSettings();

	this->SetupOpenGL();

	this->background_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
}

MainWindow::~MainWindow()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(this->window);
    glfwTerminate();
}

void MainWindow::InitializeSettings()
{
	// Setup ImGui widgets
	this->im_comp = new ImageCompositor(0.0, 0.0, this->font, true);
	this->settings_menu = new SettingsMenu(0.0, 0.0, this->font);
	this->settings_menu->enable_depth_testing = true;
	this->settings_menu->show_let_there_be_light = true;

	// Set camera settings
	this->camera = new Camera(
		glm::vec3(0.0f, 0.0f, 3.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		CAMERA_DEFAULT_YAW,
		CAMERA_DEFAULT_PITCH
	);
	this->camera->look_sensitivity = this->settings_menu->mouse_sensitivity;

	// Set mouse settings
	this->disable_cursor = false;
	this->mouse.last_x = static_cast<float>(this->monitorDimensions.width) / 2.0f;
	this->mouse.last_y = static_cast<float>(this->monitorDimensions.height) / 2.0f;
}

void MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	g_mouse.scroll_y_offset = yoffset;
}

void MainWindow::InitializeGlfw()
{
	auto glfw_error_callback = [](int error, const char* description)
		{
			fprintf(stderr, "GLFW Error %d: %s\n", error, description);
		};

	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit()) { 
		throw std::exception("Failed to initialize glfwlib. Aborting..");
	}

	// GL 3.3 + GLSL 130
	this->glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create window with graphics context
	this->GetMonitorDimensions();
	if (this->monitorDimensions.width_dpi_scale != this->monitorDimensions.height_dpi_scale) {
		throw std::exception(
			"Width and height DPI scales are different. "
			"No idea how to handle that. fuck you"
		);
	}

	this->window = glfwCreateWindow(
		static_cast<int>(static_cast<float>(this->monitorDimensions.width) * WINDOW_WIDTH_REL_SCALE),
		static_cast<int>(static_cast<float>(this->monitorDimensions.height) * WINDOW_HEIGHT_REL_SCALE),
		APP_NAME,
		nullptr,
		nullptr
	);
	if (this->window == nullptr) { 
		throw std::exception("Failed to create main window. Aborting..");
	}

	glfwMakeContextCurrent(this->window);
	glfwSwapInterval(1); // Enable vsync

	// TODO: Find a way to move data from callback function to MainWindow
	//glfwSetCursorPosCallback(window, MouseCallback);
	glfwSetScrollCallback(window, MouseScrollCallback);
}

bool MainWindow::ShouldClose()
{
	return glfwWindowShouldClose(this->window);
}

void MainWindow::Show()
{
	// React to user input
	glfwPollEvents();
	this->ProcessInput();

	// Clear previous frame
	int display_w, display_h;
	glfwGetFramebufferSize(window, &display_w, &display_h);
	glViewport(0, 0, display_w, display_h);
	glClearColor(background_color.x * background_color.w, background_color.y * background_color.w, background_color.z * background_color.w, background_color.w);
	glClear(GL_COLOR_BUFFER_BIT);

	// Ready ImGui windows for rendering
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	this->ShowImGui();
	ImGui::Render();

	// Render
	this->RenderOpenGL();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glfwSwapBuffers(window);
}

void MainWindow::InitializeImGui()
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Apply DPI scale to font and UI elements 
	// TODO: check font path before loading.
	this->font = io.Fonts->AddFontFromFileTTF(".\\fonts\\JetBrainsMono-Regular.ttf", FONT_SIZE * this->monitorDimensions.height_dpi_scale);
	ImGui::GetStyle().ScaleAllSizes(this->monitorDimensions.height_dpi_scale);
}

void MainWindow::ShowImGui() const
{
	this->settings_menu->Show();

	// Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	ImGui::ShowDemoWindow();

	// Show image compositor
	if (this->settings_menu->show_image_compositor)
	{
		this->im_comp->Show();
	}

	this->camera->SetVerticalFov(this->settings_menu->vfov);
	this->camera->look_sensitivity = this->settings_menu->mouse_sensitivity;
}

void MainWindow::UpdateCameraFromMouse()
{
	double x_pos, y_pos;
	glfwGetCursorPos(this->window, &x_pos, &y_pos);

	float delta_x = (static_cast<float>(x_pos) - this->mouse.last_x);
	float delta_y = -1.0f * (static_cast<float>(y_pos) - this->mouse.last_y);

	this->mouse.last_x = static_cast<float>(x_pos);
	this->mouse.last_y = static_cast<float>(y_pos);

	// When cursor is enabled, ignore orientation changes but react to scrolling
	// FOV is controlled from the settings menu, so updated that instead of the camera directly
	this->settings_menu->vfov -= g_mouse.scroll_y_offset;
	g_mouse.scroll_y_offset = 0.0f;

	if (!this->disable_cursor) return;
	this->camera->ProcessAxisFreeMovement(delta_x, delta_y);
}

void MainWindow::UpdateCameraFromKeyboard() const
{

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	float frame_time = 1000.0f / io.Framerate;

	// X-axis
	if (glfwGetKey(this->window, GLFW_KEY_D) == GLFW_PRESS) {
		this->settings_menu->shift_x += 0.01f;
		this->camera->ProcessAxisLockedMovement(CAMERA_DIRECTION::RIGHT, frame_time);
	}
	if (glfwGetKey(this->window, GLFW_KEY_A) == GLFW_PRESS) {
		this->settings_menu->shift_x -= 0.01f;
		this->camera->ProcessAxisLockedMovement(CAMERA_DIRECTION::LEFT, frame_time);
	}

	// Y-axis
	if (glfwGetKey(this->window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		this->settings_menu->shift_y += 0.01f;
		this->camera->ProcessAxisLockedMovement(CAMERA_DIRECTION::UP, frame_time);
	}
	if (glfwGetKey(this->window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		this->settings_menu->shift_y -= 0.01f;
		this->camera->ProcessAxisLockedMovement(CAMERA_DIRECTION::DOWN, frame_time);
	}

	// Z-axis
	if (glfwGetKey(this->window, GLFW_KEY_W) == GLFW_PRESS) {
		this->settings_menu->shift_z -= 0.01f;
		this->camera->ProcessAxisLockedMovement(CAMERA_DIRECTION::FORWARD, frame_time);
	}
	if (glfwGetKey(this->window, GLFW_KEY_S) == GLFW_PRESS) {
		this->settings_menu->shift_z += 0.01f;
		this->camera->ProcessAxisLockedMovement(CAMERA_DIRECTION::BACKWARD, frame_time);
	}
}

bool MainWindow::GetMonitorDimensions()
{
	GLFWmonitor* primary_monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(primary_monitor);

	glfwGetMonitorContentScale(
		primary_monitor, 
		&this->monitorDimensions.width_dpi_scale, 
		&this->monitorDimensions.height_dpi_scale
	);
	this->monitorDimensions.width = mode->width;
	this->monitorDimensions.height = mode->height;
	return true;
}

void MainWindow::ProcessInput()
{
	// TODO: separate camera logic from user input

	// Close window when pressing ESC
	if (glfwGetKey(this->window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	// Toggle mouse using HOME
	if (glfwGetKey(this->window, GLFW_KEY_HOME) == GLFW_PRESS) {
		this->disable_cursor = !this->disable_cursor;

		if (this->disable_cursor) {
 			glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		} else {
			glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}

	// Update camera from mouse and keyboard inputs
	this->UpdateCameraFromMouse();
	this->UpdateCameraFromKeyboard();

	this->mix_value = std::clamp(this->mix_value, 0.0f, 1.0f);
}

void MainWindow::SetupOpenGL()
{
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	this->render_targets[TEXTURED_RECTANGLE] = reinterpret_cast<RenderTarget*>(new TexturedRectangle(this->camera));
	this->render_targets[AWESOME_RECTANGLE] = reinterpret_cast<RenderTarget*>(new AwesomeRectangle(this->camera));
	this->render_targets[AWESOME_CUBE] = reinterpret_cast<RenderTarget*>(new AwesomeCube(this->camera, &io.Framerate));
	this->render_targets[AWESOME_CUBE_FIELD] = reinterpret_cast<RenderTarget*>(new AwesomeCubeField(this->camera, &io.Framerate));
	this->render_targets[LET_THERE_BE_LIGHT] = reinterpret_cast<RenderTarget*>(new LetThereBeLight(this->camera, &io.Framerate));
}

void MainWindow::RenderOpenGL() const
{
	if (this->settings_menu->enable_wireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	} else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	if (this->settings_menu->enable_depth_testing) {
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	else {
		glDisable(GL_DEPTH_TEST);
	}

	for (int i = 0; i < RENDER_TARGET_COUNT; ++i)
	{
		RenderTarget* current_target = this->render_targets[i];
		if (current_target != NULL)
		{
			switch (i)
			{
			case TEXTURED_RECTANGLE:
			{
				TexturedRectangle* tex_rect = reinterpret_cast<TexturedRectangle*>(current_target);
				tex_rect->shift_x = this->settings_menu->shift_x;
				tex_rect->shift_y = this->settings_menu->shift_y;
				tex_rect->shift_z = this->settings_menu->shift_z;
				if (this->settings_menu->show_textured_rect) current_target->Render();
				break;
			}
			case AWESOME_RECTANGLE:
			{
				AwesomeRectangle* tex_rect = reinterpret_cast<AwesomeRectangle*>(current_target);
				tex_rect->mix_value = this->mix_value;
				if (this->settings_menu->show_awesome_rect) current_target->Render();
				break;
			}
			case AWESOME_CUBE:
			{
				if (this->settings_menu->show_awesome_cube) current_target->Render();
				break;
			}
			case AWESOME_CUBE_FIELD:
			{
				if (this->settings_menu->show_awesome_cube_field) current_target->Render();
				break;
			}
			case LET_THERE_BE_LIGHT:
			{
				LetThereBeLight* light_scene = reinterpret_cast<LetThereBeLight*>(current_target);
				light_scene->light_shift = glm::vec3(
					this->settings_menu->shift_x,
					this->settings_menu->shift_y,
					this->settings_menu->shift_z
				);
				light_scene->light_color = this->settings_menu->color_vector;
				if (this->settings_menu->show_let_there_be_light) current_target->Render();
				break;
			}
			default:
				current_target->Render();
				break;
			}
		}
	}
}

