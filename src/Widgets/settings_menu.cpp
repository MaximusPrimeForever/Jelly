#include "widgets/settings_menu.h"


SettingsMenu::SettingsMenu(double x_pos, double y_pos, ImFont* font) : BaseWidget(x_pos, y_pos, font)
{
	this->shift_x = 0.0f;
	this->shift_y = 0.0f;
	this->shift_z = 0.0f;
	this->enable_wireframe = false;
	this->show_image_compositor = false;
	this->enable_depth_testing = true;
}

void SettingsMenu::Show()
{
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::PushFont(this->font);
	ImGui::SetNextWindowPos(ImVec2(0.0, 0.0));
	ImGui::Begin("Settings Menu");
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
	ImGui::Text("OpenGL settings:");
	ImGui::Checkbox("Enable Wireframe", &this->enable_wireframe);
	ImGui::Checkbox("Enable depth testing", &this->enable_depth_testing);

	ImGui::Text("Widgets:");
	ImGui::Checkbox("Show ImageCompositor", &this->show_image_compositor);
	ImGui::SliderFloat("X-Axis", &this->shift_x, -1.0f, 1.0f);
	ImGui::SliderFloat("Y-Axis", &this->shift_y, -1.0f, 1.0f);
	ImGui::SliderFloat("Z-Axis", &this->shift_z, -1.0f, 1.0f);

	ImGui::End();
	ImGui::PopFont();
}
