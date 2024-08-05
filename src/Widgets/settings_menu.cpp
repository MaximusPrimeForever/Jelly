#include "widgets/settings_menu.h"


SettingsMenu::SettingsMenu(double x_pos, double y_pos, ImFont* font) : BaseWidget(x_pos, y_pos, font)
{
	this->horizontal_offset = 0.0f;
	this->enable_wireframe = false;
	this->show_image_compositor= false;
}

void SettingsMenu::Show()
{
	ImGui::PushFont(this->font);
	ImGui::SetNextWindowPos(ImVec2(0.0, 0.0));
	ImGui::Begin("Settings Menu");

	ImGui::Checkbox("Enable Wireframe", &this->enable_wireframe);
	ImGui::Checkbox("Show ImageCompositor", &this->show_image_compositor);
	ImGui::Text("Horizontal slider");
	ImGui::SliderFloat("-1 ... 1", &this->horizontal_offset, -1.0f, 1.0f);

	ImGui::End();
	ImGui::PopFont();
}
