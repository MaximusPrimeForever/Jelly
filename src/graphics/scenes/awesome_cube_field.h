#pragma once

#include <graphics/shaders.h>
#include <graphics/render_target.h>

#include <graphics/camera.h>
#include <graphics/awesome_gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "settings.h"


class AwesomeCubeField : RenderTarget
{
private:
	float* fps;
	GLuint vao;
	GLuint texture[2];
	ShaderProgram* program = NULL;

	glm::mat4 model;

public:
	float mix_value;

	AwesomeCubeField(Camera* cam, float* fps) : RenderTarget(cam), fps(fps), mix_value(0.2f)
	{
		GLuint vbo, vbo_face;
		float vertices[] = {
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
		};

		Shader shader_paths[] = {
			Shader{".\\shaders\\awesome_cube_field\\vertex_aws_field.glsl", GL_VERTEX_SHADER, 0},
			Shader{".\\shaders\\awesome_cube_field\\frag_aws_field.glsl", GL_FRAGMENT_SHADER, 0},
		};
		this->program = new ShaderProgram(shader_paths, sizeof(shader_paths) / sizeof(Shader));

		glGenVertexArrays(1, &this->vao);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &vbo_face);
		glGenTextures(2, this->texture);

		glBindVertexArray(this->vao);

		// Begin Wooden cube
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		// Vertices
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// Texture
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 3));
		glEnableVertexAttribArray(1);

		if (!LoadTextureFromFile(".\\textures\\wood_box.jpg", &this->texture[0], true)) {
			throw std::exception("Failed to load image.");
		}

		// End wooden cube
		// Start awesome face

		if (!LoadTextureFromFile(".\\textures\\awesomeface.png", &this->texture[1], true)) {
			throw std::exception("Failed to load image.");
		}

		// End awesome face

		this->program->Use();
		this->program->SetInt("tex0_data", AGL_SAMPLER_TEXTURE0);
		this->program->SetInt("tex1_data", AGL_SAMPLER_TEXTURE1);

		this->model = glm::mat4(1.0f);
	}

	void RenderImGui() override {}

	void RenderGL() override
	{
		this->program->Use();
		float frame_time = 1.0f / (*this->fps);

		glm::vec3 cube_positions[] = {
			glm::vec3(-2.0f,  -1.0f,  -1.0f),
			glm::vec3(2.0f,  5.0f, -15.0f),
			glm::vec3(-1.5f, -2.2f, -2.5f),
			glm::vec3(-3.8f, -2.0f, -12.3f),
			glm::vec3(2.4f, -0.4f, -3.5f),
			glm::vec3(-1.7f,  3.0f, -7.5f),
			glm::vec3(1.3f, -2.0f, -2.5f),
			glm::vec3(1.5f,  2.0f, -2.5f),
			glm::vec3(1.5f,  0.2f, -1.5f),
			glm::vec3(-1.3f,  1.0f, -1.5f)
		};

		glBindVertexArray(this->vao);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, this->texture[0]);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, this->texture[1]);

		this->program->SetMat4("view", this->cam_->GetViewMatrix());
		this->program->SetMat4("projection", this->cam_->GetProjectionMatrix());

		for (unsigned int i = 0; i < 10; i++)
		{
			this->model = glm::mat4(1.0f);
			this->model = glm::translate(this->model, cube_positions[i]);

			float angle = 20.0f * (static_cast<float>(i) + 1.0f) * static_cast<float>(glfwGetTime());
			this->model = glm::rotate(this->model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));

			this->program->SetMat4("model", this->model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);

		glBindVertexArray(0);
	}
};
