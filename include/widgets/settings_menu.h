#pragma once
#include <iostream>
#include <string>
#include "settings.h"
#include "widgets/base_widget.h"

class SettingsMenu : BaseWidget
{
public:
    bool enable_wireframe;
    bool enable_depth_testing;

	// Camera
	float vfov;
	bool enable_flight_mode;
	glm::vec3 camera_position;

	// Scenes
	bool show_grid;
	bool show_textured_rect;
    bool show_awesome_rect;
    bool show_awesome_cube;
    bool show_awesome_cube_field;
	bool show_let_there_be_light;
	bool show_lit_container_party;
	bool show_global_world;

    bool has_ui_updated;
	float mouse_sensitivity;

	SettingsMenu(double x_pos, double y_pos, ImFont* font) : BaseWidget(x_pos, y_pos, font)
	{
		this->enable_flight_mode = false;
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
		this->show_global_world = false;

		this->has_ui_updated = false;
		this->mouse_sensitivity = MOUSE_DEFAULT_SENSITIVITY;
	}

	void Show()
	{
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		this->has_ui_updated = false;

		ImGui::PushFont(this->font);
		ImGui::SetNextWindowPos(ImVec2(0.0, 0.0));

		ImGui::Begin("Settings Menu");
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

		ImGui::SeparatorText("OpenGL settings");
		this->has_ui_updated |= ImGui::Checkbox("Enable wireframe", &this->enable_wireframe);
		this->has_ui_updated |= ImGui::Checkbox("Enable depth testing", &this->enable_depth_testing);
		this->has_ui_updated |= ImGui::Checkbox("Show grid", &this->show_grid);

		ImGui::SeparatorText("Camera settings");
		{
			ImGui::Text("Camera position: %.2f, %.2f, %.2f", this->camera_position.x, this->camera_position.y, this->camera_position.z);
			ImGui::Text("Mode (Press HOME or me to toggle)");
			float mode_color_hue;
			std::string mode_name;
			if (this->enable_flight_mode)
			{
				mode_name = "Flight Mode";
				mode_color_hue = 1.0;

			}
			else {
				mode_name = "Static mode";
				mode_color_hue = 2.0;
			}
			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(mode_color_hue / 7.0f, 0.6f, 0.6f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(mode_color_hue / 7.0f, 0.7f, 0.7f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(mode_color_hue / 7.0f, 0.8f, 0.8f));

			this->enable_flight_mode ^= ImGui::Button(mode_name.c_str());
			ImGui::PopStyleColor(3);
		}

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
		this->has_ui_updated |= ImGui::Checkbox("Global world", &this->show_global_world);

		ImGui::End();
		ImGui::PopFont();
	}

};
