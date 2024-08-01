#include <stdio.h>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imfilebrowser.h"

#include "settings.h"
#include "main_window.h"


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

	this->im_comp = new ImageCompositor(0.0, 0.0, this->font, true);
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
	if (this->MonitorDimensions.width_dpi_scale != this->MonitorDimensions.height_dpi_scale) {
		throw std::exception(
			"Width and height DPI scales are different. "
			"No idea how to handle that. fuck you"
		);
	}

	this->window = glfwCreateWindow(
		this->MonitorDimensions.width * WINDOW_WIDTH_REL_SCALE,
		this->MonitorDimensions.height * WINDOW_HEIGHT_REL_SCALE,
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
	this->font = io.Fonts->AddFontFromFileTTF(".\\fonts\\JetBrainsMono-Regular.ttf", FONT_SIZE * this->MonitorDimensions.height_dpi_scale);
	ImGui::GetStyle().ScaleAllSizes(this->MonitorDimensions.height_dpi_scale);
}

bool MainWindow::GetMonitorDimensions()
{
	GLFWmonitor* primary_monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(primary_monitor);

	glfwGetMonitorContentScale(
		primary_monitor, 
		&this->MonitorDimensions.width_dpi_scale, 
		&this->MonitorDimensions.height_dpi_scale
	);
	this->MonitorDimensions.width = mode->width;
	this->MonitorDimensions.height = mode->height;
	return true;
}

void MainWindow::ProcessInput()
{
	if (glfwGetKey(this->window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

bool MainWindow::ShouldClose()
{
	return glfwWindowShouldClose(this->window);
}

void MainWindow::Show()
{
    glfwPollEvents();
    this->ProcessInput();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    ImGui::ShowDemoWindow();

	// 2. Show image compositor
	this->im_comp->Show();

    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(background_color.x * background_color.w, background_color.y * background_color.w, background_color.z * background_color.w, background_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
}