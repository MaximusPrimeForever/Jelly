#pragma once
#include "settings.h"
#include "widgets/base_widget.h"

#include <glm/gtc/type_ptr.hpp>

class SettingsMenu : BaseWidget
{
public:
    bool enable_wireframe;
    bool enable_depth_testing;
    float vfov;
    float shift_x;
    float shift_y;
    float shift_z;
	float color_buffer[4] = { 1.0, 1.0, 1.0, 1.0 };
	glm::vec4 color_vector;

	bool show_grid;
	bool show_textured_rect;
    bool show_awesome_rect;
    bool show_awesome_cube;
    bool show_awesome_cube_field;
	bool show_let_there_be_light;
	bool show_lit_container_party;

    bool has_ui_updated;
	float mouse_sensitivity;

	SettingsMenu(double x_pos, double y_pos, ImFont* font) : BaseWidget(x_pos, y_pos, font)
	{
		this->shift_x = 0.0f;
		this->shift_y = 0.0f;
		this->shift_z = 0.0f;
		this->enable_wireframe = false;
		this->enable_depth_testing = false;
		this->vfov = CAMERA_DEFAULT_VERTICAL_FOV;

		this->show_grid = false;
		this->show_textured_rect = false;
		this->show_awesome_rect = false;
		this->show_awesome_cube = false;
		this->show_awesome_cube_field = false;
		this->show_let_there_be_light = false;
		this->show_lit_container_party = false;

		this->has_ui_updated = false;
		this->mouse_sensitivity = MOUSE_DEFAULT_SENSITIVITY;
	}

	void Show()
	{
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		this->has_ui_updated = false;

		ImGui::PushFont(this->font);
		ImGui::Begin("Settings Menu");
		ImGui::SetNextWindowPos(ImVec2(0.0, 0.0));
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
		ImGui::Text("Press HOME to release mouse");

		ImGui::SeparatorText("OpenGL settings");
		this->has_ui_updated |= ImGui::Checkbox("Enable wireframe", &this->enable_wireframe);
		this->has_ui_updated |= ImGui::Checkbox("Enable depth testing", &this->enable_depth_testing);
		this->has_ui_updated |= ImGui::Checkbox("Show grid", &this->show_grid);

		ImGui::SeparatorText("Camera settings");
		this->has_ui_updated |= ImGui::SliderFloat(
			"FOV (vertical)",
			&this->vfov,
			CAMERA_MIN_VERTICAL_FOV,
			CAMERA_MAX_VERTICAL_FOV
		);

		ImGui::SeparatorText("Input settings");
		this->has_ui_updated |= ImGui::SliderFloat(
			"Mouse sensitivity",
			&this->mouse_sensitivity,
			MOUSE_MIN_SENSITIVITY,
			MOUSE_MAX_SENSITIVITY
		);

		ImGui::SeparatorText("Scenes");
		this->has_ui_updated |= ImGui::Checkbox("Show textured rectangle", &this->show_textured_rect);
		this->has_ui_updated |= ImGui::Checkbox("Show awesome rectangle", &this->show_awesome_rect);
		this->has_ui_updated |= ImGui::Checkbox("Show awesome cube", &this->show_awesome_cube);
		this->has_ui_updated |= ImGui::Checkbox("Show awesome cube field", &this->show_awesome_cube_field);
		this->has_ui_updated |= ImGui::Checkbox("Let there be light!", &this->show_let_there_be_light);
		this->has_ui_updated |= ImGui::Checkbox("Lit container party", &this->show_lit_container_party);

		ImGui::End();
		ImGui::PopFont();
	}

};
