#pragma once

#include <graphics/shaders.h>
#include <graphics/render_target.h>

#include <stb_image.h>
#include <graphics/camera.h>
#include <graphics/awesome_gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "settings.h"


const glm::vec3 LIGHT_COLOR = glm::vec3(1.0f, 1.0f, 1.0f);

class LetThereBeLight : RenderTarget
{
private:
	float* fps_;
	GLuint obj_vao_;
	GLuint light_vao_;
	ShaderProgram* objects_program_ = nullptr;
	ShaderProgram* light_program_ = nullptr;

	glm::vec3 obj_position_ = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 light_pos_ = glm::vec3(1.2f, 1.0f, -2.0f);

	glm::mat4 obj_model_mat_;
	glm::mat4 light_model_mat_;

public:
	glm::vec3 light_shift;
	glm::vec4 light_color = glm::vec4(LIGHT_COLOR, 1.0);

	LetThereBeLight(Camera* cam, float* fps) : RenderTarget(cam), fps_(fps)
	{
		GLuint vbo[2];
		float vertices[] = {
			-0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f,  0.5f, -0.5f,
			 0.5f,  0.5f, -0.5f,
			-0.5f,  0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,

			-0.5f, -0.5f,  0.5f,
			 0.5f, -0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,
			-0.5f, -0.5f,  0.5f,

			-0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,

			 0.5f,  0.5f,  0.5f,
			 0.5f,  0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,

			-0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f,  0.5f,
			 0.5f, -0.5f,  0.5f,
			-0.5f, -0.5f,  0.5f,
			-0.5f, -0.5f, -0.5f,

			-0.5f,  0.5f, -0.5f,
			 0.5f,  0.5f, -0.5f,
			 0.5f,  0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f, -0.5f,
		};

		Shader object_shaders[] = {
			Shader{".\\shaders\\let_there_be_light\\vertex_objects.glsl", GL_VERTEX_SHADER, 0},
			Shader{".\\shaders\\let_there_be_light\\frag_objects.glsl", GL_FRAGMENT_SHADER, 0},
		};
		this->objects_program_ = new ShaderProgram(object_shaders, sizeof(object_shaders) / sizeof(Shader));

		Shader light_shaders[] = {
			Shader{".\\shaders\\let_there_be_light\\vertex_light.glsl", GL_VERTEX_SHADER, 0},
			Shader{".\\shaders\\let_there_be_light\\frag_light.glsl", GL_FRAGMENT_SHADER, 0},
		};
		this->light_program_ = new ShaderProgram(light_shaders, sizeof(light_shaders) / sizeof(Shader));

		// Setup obj
		glGenVertexArrays(1, &this->obj_vao_);
		glBindVertexArray(this->obj_vao_);

		glGenBuffers(1, &vbo[0]);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(0));
		glEnableVertexAttribArray(0);

		this->objects_program_->Use();

		this->obj_model_mat_ = glm::translate(glm::mat4(1.0f), this->obj_position_);
		this->objects_program_->SetVec3("object_color", glm::vec3(1.0f, 0.5f, 0.31f));

		// Setup light
		glGenVertexArrays(1, &this->light_vao_);
		glBindVertexArray(this->light_vao_);

		glGenBuffers(1, &vbo[1]);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(0));
		glEnableVertexAttribArray(0);

		this->light_program_->Use();
		this->light_model_mat_ = glm::translate(glm::mat4(1.0f), this->light_pos_);
		this->light_model_mat_ = glm::scale(this->light_model_mat_, glm::vec3(0.2f));
	}

	void Render() override
	{
		float frame_time = 1.0f / (*this->fps_);

		// Render objects
		{
			glBindVertexArray(this->obj_vao_);
			this->objects_program_->Use();

			this->objects_program_->SetVec3("light_color", this->light_color);
			this->objects_program_->SetMat4("model", this->obj_model_mat_);
			this->objects_program_->SetMat4("view", this->cam->GetViewMatrix());
			this->objects_program_->SetMat4("projection", this->cam->GetProjectionMatrix());

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		// Render light
		{
			glBindVertexArray(this->light_vao_);
			this->light_program_->Use();

			this->objects_program_->SetVec3("light_color", this->light_color);
			this->light_program_->SetVec3("offset", this->light_shift);
			this->light_program_->SetMat4("model", this->light_model_mat_);
			this->light_program_->SetMat4("view", this->cam->GetViewMatrix());
			this->light_program_->SetMat4("projection", this->cam->GetProjectionMatrix());

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glBindVertexArray(0);
	}
};
