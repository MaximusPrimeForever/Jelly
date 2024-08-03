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

#include "io/shaders.h"

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

	this->SetupOpenGL();

	// Setup ImGui widgets
	this->im_comp = new ImageCompositor(0.0, 0.0, this->font, true);
	this->settings_menu = new SettingsMenu(0.0, 0.0, this->font);

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
		this->monitorDimensions.width * WINDOW_WIDTH_REL_SCALE,
		this->monitorDimensions.height * WINDOW_HEIGHT_REL_SCALE,
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
	if (this->settings_menu->showImageCompositor)
	{
		this->im_comp->Show();
	}
}

void MainWindow::RenderOpenGL()
{
	if (this->settings_menu->enableWireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	} else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	//float timeValue = glfwGetTime();
	//float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
	//int vertexColorLocation = glGetUniformLocation(this->shaderProgram, "ourColor");
	glUseProgram(this->shaderProgram);

	//glUniform4f(vertexColorLocation,
	//	(sin(timeValue + 1.0) / 2.0f) + 0.5f, 
	//	(sin(timeValue + 2.0) / 2.0f) + 0.5f, 
	//	(sin(timeValue + 3.0) / 2.0f) + 0.5f, 
	//	1.0f
	//);

	glBindVertexArray(this->VAO[0]);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}

void MainWindow::SetupOpenGL()
{
	Shader shader_paths[] = {
		Shader{".\\shaders\\vertex.glsl", GL_VERTEX_SHADER, 0},
		Shader{".\\shaders\\frag.glsl", GL_FRAGMENT_SHADER, 0},
	};
	this->shaderProgram = BuildProgram(shader_paths, sizeof(shader_paths) / sizeof(Shader), true);

	// Begin VAO
	glGenVertexArrays(2, this->VAO);
	glBindVertexArray(this->VAO[0]);
	
	float vertices[] = {
		// positions         // colors
		 0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
		-0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
		 0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // top
	};

	// Triangle VBO
	GLuint vbo[2];
	glGenBuffers(2, vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Vertices
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Color
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (sizeof(float) * 3));
	glEnableVertexAttribArray(1);

}