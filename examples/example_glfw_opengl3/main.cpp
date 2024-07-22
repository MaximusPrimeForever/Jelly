#include <stdio.h>
#include <iostream>

// IMGUI
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "imfilebrowser.h"

// OpenGL/GLFW
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

#include "blending.h"
#include "settings.h"
#include "window_init.h"

#define BG_IMG_ID (1)
#define FG_IMG_ID (2)

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif


static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

// Main code
int main(int, char**)
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
    int monitor_width, monitor_height;
    float monitor_width_dpi_scale, monitor_height_dpi_scale;
    GetMonitorDimensions(&monitor_width , &monitor_height, &monitor_width_dpi_scale, &monitor_height_dpi_scale);
    if (monitor_width_dpi_scale != monitor_height_dpi_scale) {
        std::cout << "Width and height DPI scales are different. ";
        std::cout << "No idea how to handle that. fuck you";
        return 1;
    }

    GLFWwindow* window = glfwCreateWindow(
        monitor_width * WINDOW_WIDTH_REL_SCALE,
        monitor_height * WINDOW_HEIGHT_REL_SCALE,
        "Dear ImGui GLFW+OpenGL3 example", 
        nullptr,
        nullptr
    );
    if (window == nullptr) return 1;


    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    ImFont* font = io.Fonts->AddFontFromFileTTF("JetBrainsMono-Regular.ttf", FONT_SIZE * monitor_width_dpi_scale);
    ImGui::GetStyle().ScaleAllSizes(monitor_width_dpi_scale);

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Our state
    bool show_demo_window = false;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Default images
    Image bg_image;
    unsigned char* bg_img = stbi_load(".\\imgs\\background.png", &bg_image.width, &bg_image.height, NULL, 4);
    if (bg_img == NULL)
        return false;

    bg_image.opacity = 1.0;
    bg_image.image_data = bg_img;

	// Load teapot
    Image fg_image;
    unsigned char* fg_img = stbi_load(".\\imgs\\teapot.png", &fg_image.width, &fg_image.height, NULL, 4);
    if (fg_img == NULL)
        return false;

    fg_image.opacity = 1.0;
    fg_image.image_data = fg_img;
    int fg_image_pos_x = 0;
    int fg_image_pos_y = 0;

    Image blended_image;
    // Create an OpenGL texture
    GLuint blended_texture = 0;
    glGenTextures(1, &blended_texture);

    // Setup filtering parameters for display
    glBindTexture(GL_TEXTURE_2D, blended_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

    // initialize blended result to background image as a default
    blended_image.width = bg_image.width;
    blended_image.height = bg_image.height;
    unsigned char* blended_image_data = (unsigned char*)malloc(blended_image.width * blended_image.height * RGBA_SIZE);
    if (blended_image_data == NULL) return false;
    blended_image.image_data = blended_image_data;
    
    // blending mode list
    const char* blending_modes_strings[] = { "Normal", "Add", "Multiply", "Screen" };
    const BlendingMode blending_modes_enum[] = { BlendingMode::NORMAL, BlendingMode::ADD, BlendingMode::MULTIPLY, BlendingMode::SCREEN };
    static int current_mode = 0;

    // Create a texture to display blended image
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, blended_image.width, blended_image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, blended_image.image_data);

    ImGui::FileBrowser fileDialog;
    bool has_ui_values_changed = false;
    BlendRawImages(&bg_image, &fg_image, fg_image_pos_x, fg_image_pos_y, blending_modes_enum[current_mode], &blended_image);

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        has_ui_values_changed = false;

        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 3. Show image
        {
            ImGui::PushFont(font);
            ImGui::Begin("OpenGL Image Blender");

            if (ImGui::Button("Select background image...")) { fileDialog.Open(BG_IMG_ID); }
            if (ImGui::Button("Select foreground image...")) { fileDialog.Open(FG_IMG_ID); }

            fileDialog.Display();
            if (fileDialog.HasSelected()) {
                int width, height;
                int selected_file = -1;
                std::string filename = fileDialog.GetSelected(&selected_file).string();
                unsigned char* data = stbi_load(filename.c_str(), &width, &height, NULL, RGBA_SIZE);

                if (selected_file == FG_IMG_ID) {
                    if (NULL != data) {
                        fg_image.width = width;
                        fg_image.height = height;
                        fg_image.image_data = data; 
                    }
                }

                if (selected_file == BG_IMG_ID) {
                    if (NULL != data) {
                        bg_image.width = width;
                        bg_image.height = height;
                        bg_image.image_data = data;
                    }
                }

                fileDialog.ClearSelected();
                has_ui_values_changed |= true;
            }

            ImGui::Text("pointer = %x", blended_texture);
            ImGui::Text("size = %d x %d", blended_image.width, blended_image.height);

            has_ui_values_changed |= ImGui::SliderFloat("FG image opacity", &fg_image.opacity, 0.0f, 1.0f);
            has_ui_values_changed |= ImGui::SliderInt("FG image x", &fg_image_pos_x, -1 * bg_image.width, bg_image.width);
            has_ui_values_changed |= ImGui::SliderInt("FG image y", &fg_image_pos_y, -1 * bg_image.height, bg_image.width);

            has_ui_values_changed |= ImGui::ListBox("listbox", &current_mode, blending_modes_strings, IM_ARRAYSIZE(blending_modes_strings));

            // Update texture to new blended image
            if (has_ui_values_changed) {
                BlendRawImages(&bg_image, &fg_image, fg_image_pos_x, fg_image_pos_y, blending_modes_enum[current_mode], &blended_image);
            }
            glBindTexture(GL_TEXTURE_2D, blended_texture);
            glTexSubImage2D(
                GL_TEXTURE_2D, 0, 0, 0, 
                blended_image.width, blended_image.height,
                GL_RGBA, GL_UNSIGNED_BYTE, blended_image.image_data
            );
            ImGui::Image((void*)(intptr_t)blended_texture, ImVec2(blended_image.width, blended_image.height));

            ImGui::End();
            ImGui::PopFont();
        }

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
