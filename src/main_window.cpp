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

#include <stb_image.h>

#include "graphics/examples/texured_rectangle.h"
#include "graphics/examples/awesome_rectangle.h"
#include "graphics/examples/awesome_cube.h"
#include "graphics/examples/awesome_cube_field.h"

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

	this->camera = new Camera(
		glm::vec3(0.0f, 0.0f, 3.0f), 
		glm::vec3(0.0f, 0.0f, 0.0f),
		DEFAULT_VERTICAL_FOV
	);

	this->SetupOpenGL();

	// Setup ImGui widgets
	this->im_comp = new ImageCompositor(0.0, 0.0, this->font, true);
	this->settings_menu = new SettingsMenu(0.0, 0.0, this->font);
	this->settings_menu->show_awesome_cube_field = true;

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
	if (glfwGetKey(this->window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	float camera_speed = 0.05f;
	glm::vec3 camera_pos_delta = glm::vec3();
	glm::vec3 camera_front = glm::vec3(0.0f, 0.0f, -1.0f);

	// X-axis
	if (glfwGetKey(this->window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		this->settings_menu->shift_x += 0.01f;
		camera_pos_delta += this->camera->right * camera_speed;
	}
	if (glfwGetKey(this->window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		this->settings_menu->shift_x -= 0.01f;
		camera_pos_delta += this->camera->right * -camera_speed;
	}

	// Y-axis
	if (glfwGetKey(this->window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		this->settings_menu->shift_y += 0.01f;
		camera_pos_delta += this->camera->up * camera_speed;
	}
	if (glfwGetKey(this->window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		this->settings_menu->shift_y -= 0.01f;
		camera_pos_delta += this->camera->up * -camera_speed;
	}

	// Z-axis
	if (glfwGetKey(this->window, GLFW_KEY_UP) == GLFW_PRESS) {
		this->settings_menu->shift_z -= 0.01f;
		camera_pos_delta += camera_front * camera_speed;
	}
	if (glfwGetKey(this->window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		this->settings_menu->shift_z += 0.01f;
		camera_pos_delta += camera_front * -camera_speed;
	}

	// Set camera to point straight
	this->camera->AddPositionDelta(camera_pos_delta, false);
	this->camera->SetTarget(this->camera->position + camera_front);

	this->mix_value = std::clamp(this->mix_value, 0.0f, 1.0f);
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

void MainWindow::ShowImGui()
{
	this->settings_menu->Show();

	// Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	ImGui::ShowDemoWindow();

	// Show image compositor
	if (this->settings_menu->show_image_compositor)
	{
		this->im_comp->Show();
	}
}

void MainWindow::RenderOpenGL()
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

	// Computationally heavy tasks should be done only if UI has been updated
	// To avoid running them every frame when values haven't changed
	if (this->settings_menu->has_ui_updated)
	{
		/*
		 * Note: interestingly, ProcessInput() modified values in the settings menu
		 * which get reflected in ImGui, the sliders move, but doesn't trigger this condition.
		 * ImGui methods must return `true` only if the user interacted with the widget directly
		 */
		this->camera->SetVFov(this->settings_menu->vfov);
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
				if (this->settings_menu->show_awesome_cube) current_target->Render();
				break;
			case AWESOME_CUBE_FIELD:
				if (this->settings_menu->show_awesome_cube_field) current_target->Render();
				break;
			default:
				current_target->Render();
				break;
			}
		}
	}
}

void MainWindow::SetupOpenGL()
{
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	this->render_targets[TEXTURED_RECTANGLE] = reinterpret_cast<RenderTarget*>(new TexturedRectangle(this->camera));
	this->render_targets[AWESOME_RECTANGLE] = reinterpret_cast<RenderTarget*>(new AwesomeRectangle(this->camera));
	this->render_targets[AWESOME_CUBE] = reinterpret_cast<RenderTarget*>(new AwesomeCube(this->camera, &io.Framerate));
	this->render_targets[AWESOME_CUBE_FIELD] = reinterpret_cast<RenderTarget*>(new AwesomeCubeField(this->camera,&io.Framerate));
}
