#pragma once

#include <graphics/shaders.h>
#include <graphics/render_target.h>

#include <graphics/camera.h>
#include <graphics/awesome_gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "io/file_io.h"


class LitContainerParty : RenderTarget
{
	// Constants
	const float LIGHT_AMBIENT_FACTOR = 0.1f;
	const float LIGHT_DIFFUSE_FACTOR = 0.5f;
	const float LIGHT_SPECULAR_FACTOR = 1.0f;

	const float LIGHT_POSITION_OFFSET_FACTOR = 5.0f;
	const glm::vec3 LIGHT_COLOR = glm::vec3(1.0f, 1.0f, 1.0f);

	const glm::vec3 MATERIAL_COLOR = glm::vec3(1.0f, 0.5f, 0.31f);
	const float MATERIAL_SHININESS = 32.0f;

private:
	// Members
	float* fps_;
	GLuint obj_vao_;
	GLuint light_vao_;
	ShaderProgram* objects_program_ = nullptr;
	ShaderProgram* light_program_ = nullptr;

	float imgui_light_rgba[3] = {LIGHT_COLOR.r, LIGHT_COLOR.g, LIGHT_COLOR.b};
	glm::vec3 light_color = LIGHT_COLOR;
	glm::vec3 light_pos_offset_ = glm::vec3(0.0f);
	glm::vec3 light_pos_ = glm::vec3(0.2f, 1.0f, 0.3f);

	glm::vec3 obj_position_ = glm::vec3(0.0f, 0.0f, 2.0f);
	glm::mat3 obj_normal_mat_;
	glm::mat4 obj_model_mat_;
	glm::mat4 light_model_mat_;

	GLuint obj_texture_diffuse_;
	GLuint obj_texture_specular_;
public:

	LitContainerParty(Camera* cam, float* fps) : RenderTarget(cam), fps_(fps)
	{
		constexpr GLuint stride = 8;

		GLuint vbo;
		GLuint vertex_attrb;

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
			Shader{".\\shaders\\lit_container_party\\vertex_objects.glsl", GL_VERTEX_SHADER, 0},
			Shader{".\\shaders\\lit_container_party\\frag_objects.glsl", GL_FRAGMENT_SHADER, 0},
		};
		this->objects_program_ = new ShaderProgram(object_shaders, sizeof(object_shaders) / sizeof(Shader));

		Shader light_shaders[] = {
			Shader{".\\shaders\\lit_container_party\\vertex_light.glsl", GL_VERTEX_SHADER, 0},
			Shader{".\\shaders\\lit_container_party\\frag_light.glsl", GL_FRAGMENT_SHADER, 0},
		};
		this->light_program_ = new ShaderProgram(light_shaders, sizeof(light_shaders) / sizeof(Shader));

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

		if (!LoadTextureFromFile("textures\\steel_box_diffuse.png", &this->obj_texture_diffuse_)) {
			throw std::exception("Failed to load image.");
		}
		if (!LoadTextureFromFile("textures\\steel_box_specular.png", &this->obj_texture_specular_)) {
			throw std::exception("Failed to load image.");
		}

		glBindTexture(GL_TEXTURE_2D, this->obj_texture_diffuse_);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0);
		glBindTexture(GL_TEXTURE_2D, 0);

		// Setup light
		glGenVertexArrays(1, &this->light_vao_);
		glBindVertexArray(this->light_vao_);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		vertex_attrb = 0;
		glVertexAttribPointer(vertex_attrb, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), static_cast<void*>(0));
		glEnableVertexAttribArray(vertex_attrb);
	}

	void RenderImGui() override
	{
		ImGui::Begin("Lit Container Party Settings");

		ImGui::SeparatorText("Light Position");
		ImGui::SliderFloat("X-Axis", &this->light_pos_offset_.x, -1.0f, 1.0f);
		ImGui::SliderFloat("Y-Axis", &this->light_pos_offset_.y, -1.0f, 1.0f);
		ImGui::SliderFloat("Z-Axis", &this->light_pos_offset_.z, -1.0f, 1.0f);

		ImGui::SeparatorText("Light Color Picker");
		ImGuiColorEditFlags flags;
		flags |= ImGuiColorEditFlags_PickerHueBar;
		flags |= ImGuiColorEditFlags_DisplayRGB;
		ImGui::ColorPicker3("Light Color", this->imgui_light_rgba, flags);
		this->light_color = glm::vec3(
			this->imgui_light_rgba[0],
			this->imgui_light_rgba[1],
			this->imgui_light_rgba[2]
		);

		ImGui::End();
	}

	void RenderGL() override
	{
		float frame_time = 1.0f / (*this->fps_);
		glm::vec3 temp_light_pos = this->light_pos_ + this->light_pos_offset_ * LIGHT_POSITION_OFFSET_FACTOR;
		glm::vec3 light_ambient = this->light_color * LIGHT_AMBIENT_FACTOR;
		glm::vec3 light_diffuse = this->light_color * LIGHT_DIFFUSE_FACTOR;
		glm::vec3 light_specular = this->light_color * LIGHT_SPECULAR_FACTOR;

		glm::mat4 view_matrix = this->cam_->GetViewMatrix();
		glm::mat4 projection_matrix = this->cam_->GetProjectionMatrix();

		// Render light cube
		{
			glBindVertexArray(this->light_vao_);
			this->light_program_->Use();

			this->light_model_mat_ = glm::translate(glm::mat4(1.0f), temp_light_pos);
			this->light_model_mat_ = glm::scale(this->light_model_mat_, glm::vec3(0.2f));

			this->light_program_->SetVec3("uLightColor", this->light_color);
			this->light_program_->SetMat4("uModel", this->light_model_mat_);
			this->light_program_->SetMat4("uView", view_matrix);
			this->light_program_->SetMat4("uProjection", projection_matrix);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		// Render objects
		{
			glBindVertexArray(this->obj_vao_);
			this->objects_program_->Use();

			this->objects_program_->SetVec3("uLightPos", temp_light_pos);
			this->objects_program_->SetMat4("uView", view_matrix);
			this->objects_program_->SetMat4("uProjection", projection_matrix);

			// Set light properties
			/*
			this->objects_program_->SetVec3("uDirLight.direction", temp_light_pos);
			this->objects_program_->SetVec3("uDirLight.ambient", light_ambient);
			this->objects_program_->SetVec3("uDirLight.diffuse", light_diffuse);
			this->objects_program_->SetVec3("uDirLight.specular", light_specular);
			*/

			this->objects_program_->SetVec3("uPointLight.ambient", light_ambient);
			this->objects_program_->SetVec3("uPointLight.diffuse", light_diffuse);
			this->objects_program_->SetVec3("uPointLight.specular", light_specular);

			this->objects_program_->SetFloat("uPointLight.constant", 1.0f);
			this->objects_program_->SetFloat("uPointLight.linear", 0.09f);
			this->objects_program_->SetFloat("uPointLight.quadratic", 0.032f);

			// Set cube textures
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, this->obj_texture_diffuse_);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, this->obj_texture_specular_);

			this->objects_program_->SetInt("uMaterial.diffuse", AGL_SAMPLER_TEXTURE0);
			this->objects_program_->SetInt("uMaterial.specular", AGL_SAMPLER_TEXTURE1);
			this->objects_program_->SetFloat("uMaterial.shininess", MATERIAL_SHININESS);

			const glm::vec3 cube_positions[] = {
				glm::vec3(-2.0f,  -1.0f,  -1.0f),
				glm::vec3(2.0f,  5.0f, -10.0f),
				glm::vec3(-1.5f, -2.2f, -2.5f),
				glm::vec3(-3.8f, -2.0f, -8.3f),
				glm::vec3(2.4f, -0.4f, -3.5f),
				glm::vec3(-1.7f,  3.0f, -5.5f),
				glm::vec3(1.3f, -2.0f, -2.5f),
				glm::vec3(1.5f,  2.0f, -2.5f),
				glm::vec3(1.5f,  0.2f, -1.5f),
				glm::vec3(-1.3f,  1.0f, -1.5f)
			};

			glm::mat4 model_mat, normal_mat;
			for (unsigned int i = 0; i < sizeof(cube_positions) / sizeof(glm::vec3); i++)
			{
				float angle = 20.0f * (static_cast<float>(i) + 1.0f) * static_cast<float>(glfwGetTime());

				// Center containers around the same position
				model_mat = glm::translate(glm::mat4(1.0f), this->obj_position_);
				model_mat = glm::translate(model_mat, cube_positions[i]);
				model_mat = glm::rotate(model_mat, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));

				normal_mat = glm::mat3(glm::transpose(glm::inverse(view_matrix * model_mat)));
				
				this->objects_program_->SetMat3("uNormalMatrix", normal_mat);
				this->objects_program_->SetMat4("uModel", model_mat);

				glDrawArrays(GL_TRIANGLES, 0, 36);
			}

			// Clear textures
			for (int i = 0; i < 2; ++i)
			{
				glActiveTexture(GL_TEXTURE0 + i);
				glBindTexture(GL_TEXTURE_2D, 0);
			}
		}

		glBindVertexArray(0);
	}
};
