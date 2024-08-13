#include "widgets/settings_menu.h"


SettingsMenu::SettingsMenu(double x_pos, double y_pos, ImFont* font) : BaseWidget(x_pos, y_pos, font)
{
	this->shift_x = 0.0f;
	this->shift_y = 0.0f;
	this->shift_z = 0.0f;
	this->enable_wireframe = false;
	this->show_image_compositor = false;
	this->enable_depth_testing = true;
	this->vfov = 45.0f;

	this->show_textured_rect = false;
	this->show_awesome_rect = false;
	this->show_awesome_cube = false;
	this->show_awesome_cube_field = false;

	this->has_ui_updated = false;
}

void SettingsMenu::Show()
{
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	this->has_ui_updated = false;

	ImGui::PushFont(this->font);
	ImGui::SetNextWindowPos(ImVec2(0.0, 0.0));
	ImGui::Begin("Settings Menu");
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
	ImGui::Text("Press HOME to release mouse");
	ImGui::Text("OpenGL settings:");
	this->has_ui_updated |= ImGui::Checkbox("Enable Wireframe", &this->enable_wireframe);
	this->has_ui_updated |= ImGui::Checkbox("Enable depth testing", &this->enable_depth_testing);
	this->has_ui_updated |= ImGui::SliderFloat("FOV (Vertical)", &this->vfov, 10.0f, 100.0f);

	ImGui::Text("Widgets:");
	this->has_ui_updated |= ImGui::Checkbox("Show ImageCompositor", &this->show_image_compositor);
	this->has_ui_updated |= ImGui::Checkbox("Show textured rectangle", &this->show_textured_rect);
	this->has_ui_updated |= ImGui::SliderFloat("X-Axis", &this->shift_x, -1.0f, 1.0f);
	this->has_ui_updated |= ImGui::SliderFloat("Y-Axis", &this->shift_y, -1.0f, 1.0f);
	this->has_ui_updated |= ImGui::SliderFloat("Z-Axis", &this->shift_z, -1.0f, 1.0f);

	this->has_ui_updated |= ImGui::Checkbox("Show awesome rectangle", &this->show_awesome_rect);
	this->has_ui_updated |= ImGui::Checkbox("Show awesome cube", &this->show_awesome_cube);
	this->has_ui_updated |= ImGui::Checkbox("Show awesome cube field", &this->show_awesome_cube_field);

	ImGui::End();
	ImGui::PopFont();
}
