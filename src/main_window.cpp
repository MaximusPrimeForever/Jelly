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

// Scenes
#include "graphics/scenes/xz_grid.h"
#include "graphics/scenes/textured_rectangle.h"
#include "graphics/scenes/awesome_rectangle.h"
#include "graphics/scenes/awesome_cube.h"
#include "graphics/scenes/awesome_cube_field.h"
#include "graphics/scenes/let_there_be_light.h"
#include "graphics/scenes/lit_container_party.h"

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
	// Background color
	this->background_color = ImVec4(0.1f, 0.1f, 0.1f, 1.00f);

	// Setup ImGui widgets
	this->settings_menu = new SettingsMenu(0.0, 0.0, g_app_font);

	// Setup default settings
	this->settings_menu->enable_depth_testing = true;
	this->settings_menu->show_grid = true;
	this->settings_menu->show_lit_container_party = true;

	// Set camera settings
	this->camera = new Camera(
		glm::vec3(1.0f, 1.0f, 3.0f),
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
	g_mouse.scroll_y_offset = static_cast<float>(yoffset);
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

	GLFWimage image;
	image.pixels = stbi_load("resources\\Prismatic_Jelly.png", &image.width, &image.height, 0, 4); //rgba channels 
	glfwSetWindowIcon(window, 1, &image);
	stbi_image_free(image.pixels);

	glfwMakeContextCurrent(this->window);
	glfwSwapInterval(1); // Enable vsync

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

	// Render scenes
	this->RenderOpenGL();

	// Render ImGui after RenderOpenGL() because some scenes have their own ImGui menu.
	ImGui::Render();
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
	g_app_font = io.Fonts->AddFontFromFileTTF(".\\fonts\\JetBrainsMono-Regular.ttf", FONT_SIZE * this->monitorDimensions.height_dpi_scale);
	ImGui::GetStyle().ScaleAllSizes(this->monitorDimensions.height_dpi_scale);
}

void MainWindow::ShowImGui() const
{
	this->settings_menu->Show();

	// Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	ImGui::ShowDemoWindow();

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
		this->camera->ProcessAxisLockedMovement(CAMERA_DIRECTION::RIGHT, frame_time);
	}
	if (glfwGetKey(this->window, GLFW_KEY_A) == GLFW_PRESS) {
		this->camera->ProcessAxisLockedMovement(CAMERA_DIRECTION::LEFT, frame_time);
	}

	// Y-axis
	if (glfwGetKey(this->window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		this->camera->ProcessAxisLockedMovement(CAMERA_DIRECTION::UP, frame_time);
	}
	if (glfwGetKey(this->window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		this->camera->ProcessAxisLockedMovement(CAMERA_DIRECTION::DOWN, frame_time);
	}

	// Z-axis
	if (glfwGetKey(this->window, GLFW_KEY_W) == GLFW_PRESS) {
		this->camera->ProcessAxisLockedMovement(CAMERA_DIRECTION::FORWARD, frame_time);
	}
	if (glfwGetKey(this->window, GLFW_KEY_S) == GLFW_PRESS) {
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

// Load scenes and set OpenGL settings that will not be changed throughout the run.
void MainWindow::SetupOpenGL()
{
	// Enable alpha blending (settings stolen from learnopengl.com without further thought)
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	ImGuiIO& io = ImGui::GetIO(); (void)io;

	this->render_targets[XZ_GRID] = RenderEntry{
		reinterpret_cast<RenderTarget*>(new XzGrid(this->camera)),
		&this->settings_menu->show_grid
	};
	this->render_targets[TEXTURED_RECTANGLE] = RenderEntry{
		reinterpret_cast<RenderTarget*>(new TexturedRectangle(this->camera)),
		&this->settings_menu->show_textured_rect
	};
	this->render_targets[AWESOME_RECTANGLE] = RenderEntry{
		reinterpret_cast<RenderTarget*>(new AwesomeRectangle(this->camera)),
		&this->settings_menu->show_awesome_rect
	};
	this->render_targets[AWESOME_CUBE] = RenderEntry{
		 reinterpret_cast<RenderTarget*>(new AwesomeCube(this->camera, &io.Framerate)),
		&this->settings_menu->show_awesome_cube
	};
	this->render_targets[AWESOME_CUBE_FIELD] = RenderEntry{
		reinterpret_cast<RenderTarget*>(new AwesomeCubeField(this->camera, &io.Framerate)),
		&this->settings_menu->show_awesome_cube_field
	};
	this->render_targets[LET_THERE_BE_LIGHT] = RenderEntry{
		reinterpret_cast<RenderTarget*>(new LetThereBeLight(this->camera, &io.Framerate)),
		&this->settings_menu->show_let_there_be_light
	};
	this->render_targets[LIT_CONTAINER_PARTY] = RenderEntry{
		reinterpret_cast<RenderTarget*>(new LitContainerParty(this->camera, &io.Framerate)),
		&this->settings_menu->show_lit_container_party
	};

}

// Draw scenes.
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
		RenderEntry current_target = this->render_targets[i];
		if (current_target.target == nullptr || !*current_target.should_render) continue;

		current_target.target->Render();
	}
}

