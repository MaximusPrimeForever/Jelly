#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "io/file_io.h"

#include "graphics/shaders.h"
#include "graphics/render_target.h"
#include "graphics/camera.h"
#include "graphics/awesome_gl.h"
#include "graphics/phong_light.h"
#include "graphics/material.h"
#include "graphics/mesh.h"


class GlobalWorld : RenderTarget
{
	// Constants
	const float LIGHT_POSITION_OFFSET_FACTOR = 5.0f;
	const glm::vec3 LIGHT_COLOR = glm::vec3(1.0f, 1.0f, 1.0f);

	const glm::vec3 MATERIAL_COLOR = glm::vec3(1.0f, 0.5f, 0.31f);
	const float MATERIAL_SHININESS = 32.0f;

	const size_t LIGHT_COUNT = 4;

private:
	// OpenGL
	float* fps_;
	GLuint obj_vao_;
	GLuint light_vao_;
	ShaderProgram* objects_program_ = nullptr;
	ShaderProgram* light_program_ = nullptr;

	// ImGui
	int current_light_selection_ = 0;
	bool is_flashlight_on = false;
	PointLight* current_light;
	glm::vec3 current_light_pos_offset_ = glm::vec3(0.0f);
	glm::vec3 current_light_initial_pos_ = glm::vec3(0.0f);
	glm::vec3 current_light_color_;
	float imgui_light_rgba[3] = { LIGHT_COLOR.r, LIGHT_COLOR.g, LIGHT_COLOR.b };

	// Rendering
	std::vector<glm::vec3> cube_positions;

	PhongMaterial cube_material;
	DirectionalLight dir_light_;
	SpotLight flashlight_;
	std::vector<PointLight> point_lights_;
public:

	GlobalWorld(Camera* cam, float* fps) : RenderTarget(cam), fps_(fps)
	{
		constexpr GLuint stride = 8;

		GLuint vbo;
		GLuint vertex_attrb;

		// Build shaders
		Shader light_shaders[] = {
			Shader{".\\shaders\\world\\vertex_light.glsl", GL_VERTEX_SHADER, 0},
			Shader{".\\shaders\\world\\frag_light.glsl", GL_FRAGMENT_SHADER, 0},
		};
		this->light_program_ = new ShaderProgram(light_shaders, sizeof(light_shaders) / sizeof(Shader));

		float vertices[] = {
			// positions          // normals           // texture coords
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
			-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

			 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
			 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
		};

		Shader object_shaders[] = {
			Shader{".\\shaders\\world\\vertex_objects.glsl", GL_VERTEX_SHADER, 0},
			Shader{".\\shaders\\world\\frag_objects.glsl", GL_FRAGMENT_SHADER, 0},
		};
		this->objects_program_ = new ShaderProgram(object_shaders, sizeof(object_shaders) / sizeof(Shader));

		// Setup obj
		glGenVertexArrays(1, &this->obj_vao_);
		glGenBuffers(1, &vbo);

		/// Load vertices data
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindVertexArray(this->obj_vao_);

		/// Cube vertices
		vertex_attrb = 0;
		glVertexAttribPointer(vertex_attrb, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), static_cast<void*>(0));
		glEnableVertexAttribArray(vertex_attrb);

		/// Normal vectors
		vertex_attrb = 1;
		glVertexAttribPointer(vertex_attrb, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(vertex_attrb);

		/// Texture coords
		vertex_attrb = 2;
		glVertexAttribPointer(vertex_attrb, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(vertex_attrb);

		this->cube_material = PhongMaterial(
			"textures\\steel_box_diffuse.png", 
			"textures\\steel_box_specular.png", 
			MATERIAL_SHININESS
		);

		glBindTexture(GL_TEXTURE_2D, this->cube_material.diffuse_texture);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0);
		glBindTexture(GL_TEXTURE_2D, this->cube_material.specular_texture);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0);
		glBindTexture(GL_TEXTURE_2D, 0);

		this->InitializeCubePositions();

		// Setup lights
		this->InitializeLights();

		glGenVertexArrays(1, &this->light_vao_);
		glBindVertexArray(this->light_vao_);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		vertex_attrb = 0;
		glVertexAttribPointer(vertex_attrb, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), static_cast<void*>(0));
		glEnableVertexAttribArray(vertex_attrb);
	}

	void InitializeCubePositions()
	{
		this->cube_positions.emplace_back(-2.0f, -1.0f, -1.0f);
		this->cube_positions.emplace_back(2.0f, 5.0f, -10.0f);
		this->cube_positions.emplace_back(-1.5f, -2.2f, -2.5f);
		this->cube_positions.emplace_back(-3.8f, -2.0f, -8.3f);
		this->cube_positions.emplace_back(2.4f, -0.4f, -3.5f);
		this->cube_positions.emplace_back(-1.7f, 3.0f, -5.5f);
		this->cube_positions.emplace_back(1.3f, -2.0f, -2.5f);
		this->cube_positions.emplace_back(1.5f, 2.0f, -2.5f);
		this->cube_positions.emplace_back(1.5f, 0.2f, -1.5f);
		this->cube_positions.emplace_back(-1.3f, 1.0f, -1.5f);
	}

	void InitializeLights()
	{
		this->dir_light_ = DirectionalLight(
			glm::vec3(1.0f),
			glm::vec3(-0.2f, -1.0f, -0.3f)
		);

		this->point_lights_.emplace_back(
			glm::vec3(0.7f, 0.2f, 2.0f),
			glm::vec3(0.7f, 0.2f, 1.0f),
			21.0f
		);
		this->point_lights_.emplace_back(
			glm::vec3(2.3f, -3.3f, -4.0f),
			glm::vec3(0.3f, 0.3f, 0.0f),
			13.0f
		);
		this->point_lights_.emplace_back(
			glm::vec3(-4.0f, 2.0f, -12.0f),
			glm::vec3(0.33f, 0.2f, 1.0f),
			13.0f
		);
		this->point_lights_.emplace_back(
			glm::vec3(0.0f, 0.0f, -3.0f),
			glm::vec3(0.0f, 0.0f, 1.0f),
			30.0f
		);
		this->current_light = &this->point_lights_[this->current_light_selection_];
		this->current_light_initial_pos_ = this->current_light->position;

		this->flashlight_ = SpotLight(
			this->cam_->position,
			this->cam_->front,
			glm::vec3(2.0f),
			50.0f,
			10.0f,
			22.5f
		);
	}

	void RenderImGui() override
	{
		ImGui::Begin("Global world settings");

		ImGui::SeparatorText("Light Selector");
		bool point_light_was_selected = false;

		for (size_t i = 0; i < LIGHT_COUNT - 1; i++)
		{
			std::string button_name = "Light " + std::to_string(i + 1);
			point_light_was_selected |= ImGui::RadioButton(button_name.c_str(), &this->current_light_selection_, i); ImGui::SameLine();
		}
		point_light_was_selected |= ImGui::RadioButton(
			("Light " + std::to_string(LIGHT_COUNT)).c_str(),
			&this->current_light_selection_,
			LIGHT_COUNT - 1
		);

		this->current_light = &this->point_lights_[this->current_light_selection_];
		if (point_light_was_selected) {
			this->current_light_initial_pos_ = this->current_light->position;
			this->current_light_pos_offset_ = glm::vec3(0.0f);
			this->imgui_light_rgba[0] = this->current_light->color.x;
			this->imgui_light_rgba[1] = this->current_light->color.y;
			this->imgui_light_rgba[2] = this->current_light->color.z;
		}

		ImGui::Checkbox("Flashlight Toggle", &this->is_flashlight_on);
		
		std::string current_light_str = "Light " + std::to_string(this->current_light_selection_);
		ImGui::SeparatorText((current_light_str + " Position").c_str());
		ImGui::SliderFloat("X-Axis", &this->current_light_pos_offset_.x, -1.0f, 1.0f);
		ImGui::SliderFloat("Y-Axis", &this->current_light_pos_offset_.y, -1.0f, 1.0f);
		ImGui::SliderFloat("Z-Axis", &this->current_light_pos_offset_.z, -1.0f, 1.0f);

		ImGui::SeparatorText((current_light_str + " Color").c_str());
		ImGuiColorEditFlags flags = 0;
		flags |= ImGuiColorEditFlags_PickerHueBar;
		flags |= ImGuiColorEditFlags_DisplayRGB;
		ImGui::ColorPicker3("Color Picker", this->imgui_light_rgba, flags);
		this->current_light_color_ = glm::vec3(
			this->imgui_light_rgba[0],
			this->imgui_light_rgba[1],
			this->imgui_light_rgba[2]
		);

		ImGui::End();
	}

	void SetDirectionalLightUni()
	{
		this->objects_program_->SetVec3("uDirLight.direction", this->dir_light_.direction);

		this->objects_program_->SetVec3("uDirLight.ambient", this->dir_light_.GetAmbientColor());
		this->objects_program_->SetVec3("uDirLight.diffuse", this->dir_light_.GetDiffuseColor());
		this->objects_program_->SetVec3("uDirLight.specular", this->dir_light_.GetSpecularColor());
	}

	void SetPointLightsUni()
	{
		for (int i = 0; i < LIGHT_COUNT; ++i) {
			std::string base = "uPointLights[" + std::to_string(i) + "].";
			PointLight& current_light = this->point_lights_[i];
			this->objects_program_->SetVec3(base + "position", current_light.position);

			this->objects_program_->SetVec3(base + "ambient", current_light.GetAmbientColor());
			this->objects_program_->SetVec3(base + "diffuse", current_light.GetDiffuseColor());
			this->objects_program_->SetVec3(base + "specular", current_light.GetSpecularColor());

			this->objects_program_->SetFloat(base + "constant", current_light.constant);
			this->objects_program_->SetFloat(base + "linear", current_light.linear);
			this->objects_program_->SetFloat(base + "quadratic", current_light.quadratic);
		}
	}

	void SetSpotLightUni()
	{
		std::string base = "uFlashlight.";
		this->objects_program_->SetVec3(base + "position", this->flashlight_.position);
		this->objects_program_->SetVec3(base + "direction", this->flashlight_.direction);

		this->objects_program_->SetFloat(base + "innerCutoff", this->flashlight_.inner_cutoff);
		this->objects_program_->SetFloat(base + "outerCutoff", this->flashlight_.outer_cutoff);

		this->objects_program_->SetVec3(base + "ambient", this->flashlight_.GetAmbientColor());
		this->objects_program_->SetVec3(base + "diffuse", this->flashlight_.GetDiffuseColor());
		this->objects_program_->SetVec3(base + "specular", this->flashlight_.GetSpecularColor());

		this->objects_program_->SetFloat(base + "constant", this->flashlight_.constant);
		this->objects_program_->SetFloat(base + "linear", this->flashlight_.linear);
		this->objects_program_->SetFloat(base + "quadratic", this->flashlight_.quadratic);
	}
	
	void RenderGL() override
	{
		float frame_time = 1.0f / (*this->fps_);
		glm::mat4 view_matrix = this->cam_->GetViewMatrix();
		glm::mat4 projection_matrix = this->cam_->GetProjectionMatrix();

		// Update current point light from ImGui
		this->current_light->position = this->current_light_initial_pos_ + this->current_light_pos_offset_ * LIGHT_POSITION_OFFSET_FACTOR;
		this->current_light->color = this->current_light_color_;
		
		// Toggle flashlight
		if (this->is_flashlight_on) {
			this->flashlight_.color = glm::vec3(1.0f);
		} else {
			this->flashlight_.color = glm::vec3(0.0f);
		}

		// Set flashlight position and direction according to cam
		this->flashlight_.position = this->cam_->position;
		this->flashlight_.direction = this->cam_->front;

		// Render light cubes for point lights
		{
			glBindVertexArray(this->light_vao_);
			this->light_program_->Use();

			for (PointLight& point_light : this->point_lights_)
			{
				glm::mat4 light_model_mat = glm::translate(glm::mat4(1.0f), point_light.position);
				light_model_mat = glm::scale(light_model_mat, glm::vec3(0.2f));

				this->light_program_->SetMat4("uModel", light_model_mat);
				this->light_program_->SetMat4("uView", view_matrix);
				this->light_program_->SetMat4("uProjection", projection_matrix);
				
				this->light_program_->SetVec3("uLightColor", point_light.color);

				glDrawArrays(GL_TRIANGLES, 0, 36);
			}

			glBindVertexArray(0);
		}

		// Render objects
		{
			glBindVertexArray(this->obj_vao_);
			this->objects_program_->Use();

			this->objects_program_->SetMat4("uView", view_matrix);
			this->objects_program_->SetMat4("uProjection", projection_matrix);

			// Set light properties
			this->SetDirectionalLightUni();
			this->SetPointLightsUni();
			this->SetSpotLightUni();

			// Set cube textures
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, this->cube_material.diffuse_texture);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, this->cube_material.specular_texture);

			this->objects_program_->SetInt("uMaterial.diffuse", AGL_SAMPLER_TEXTURE0);
			this->objects_program_->SetInt("uMaterial.specular", AGL_SAMPLER_TEXTURE1);
			this->objects_program_->SetFloat("uMaterial.shininess", MATERIAL_SHININESS);

			glm::mat4 model_mat, normal_mat;
			for (glm::vec3& cube_pos : this->cube_positions)
			{
				// Center containers around the same position
				model_mat = glm::translate(glm::mat4(1.0f), cube_pos);
				normal_mat = glm::mat3(glm::transpose(glm::inverse(view_matrix * model_mat)));

				this->objects_program_->SetMat3("uNormalMatrix", normal_mat);
				this->objects_program_->SetMat4("uModel", model_mat);

				glDrawArrays(GL_TRIANGLES, 0, 36);
			}

			glBindTexture(GL_TEXTURE_2D, 0);
		}

		glBindVertexArray(0);
	}
};
