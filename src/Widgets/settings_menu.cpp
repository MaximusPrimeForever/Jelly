#include "widgets/settings_menu.h"


SettingsMenu::SettingsMenu(double x_pos, double y_pos, ImFont* font) : BaseWidget(x_pos, y_pos, font)
{
	this->horizontal_offset = 0.0f;
	this->enable_wireframe = false;
	this->show_image_compositor= false;
}

void SettingsMenu::Show()
{
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::PushFont(this->font);
	ImGui::SetNextWindowPos(ImVec2(0.0, 0.0));
	ImGui::Begin("Settings Menu");
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

	ImGui::Checkbox("Enable Wireframe", &this->enable_wireframe);
	ImGui::Checkbox("Show ImageCompositor", &this->show_image_compositor);
	ImGui::Text("Horizontal slider");
	ImGui::SliderFloat("-1 ... 1", &this->horizontal_offset, -1.0f, 1.0f);

	ImGui::End();
	ImGui::PopFont();
}
