#include "widgets/settings_menu.h"


SettingsMenu::SettingsMenu(double x_pos, double y_pos, ImFont* font) : BaseWidget(x_pos, y_pos, font)
{
	this->enableWireframe = false;
	this->showImageCompositor = false;
}

void SettingsMenu::Show()
{
	ImGui::PushFont(this->font);
	ImGui::Begin("Settings Menu");

	ImGui::Checkbox("Enable Wireframe", &this->enableWireframe);
	ImGui::Checkbox("Show ImageCompositor", &this->showImageCompositor);

	ImGui::End();
	ImGui::PopFont();
}
