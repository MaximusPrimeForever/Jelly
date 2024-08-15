#pragma once

#include <graphics/shaders.h>
#include <graphics/render_target.h>

#include <graphics/camera.h>
#include <graphics/awesome_gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "settings.h"
#include "io/file_io.h"


class AwesomeCube : RenderTarget
{
private:
	float* fps;
	GLuint vao;
	GLuint texture[2];
	ShaderProgram* program = NULL;

	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;

public:
	float mix_value;

	AwesomeCube(Camera* cam, float* fps): RenderTarget(cam)
	{
		this->mix_value = 0.5;
		this->fps = fps;

		GLuint vbo, vbo_face;
		int width, height, nrChannels;
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
			Shader{".\\shaders\\awesome_cube\\vertex_aws_cube.glsl", GL_VERTEX_SHADER, 0},
			Shader{".\\shaders\\awesome_cube\\frag_aws_cube.glsl", GL_FRAGMENT_SHADER, 0},
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

		if (!LoadTextureFromFile(".\\textures\\wood_box.jpg", &this->texture[0], &width, &height, true)) {
			throw std::exception("Failed to load image.");
		}

		// End wooden cube
		// Start awesome face

		if (!LoadTextureFromFile(".\\textures\\awesomeface.png", &this->texture[1], &width, &height, true)) {
			throw std::exception("Failed to load image.");
		}

		// End awesome face

		this->program->Use();
		this->program->SetInt("tex0_data", AGL_SAMPLER_TEXTURE0);
		this->program->SetInt("tex1_data", AGL_SAMPLER_TEXTURE1);

		this->model = glm::mat4(1.0f);
		this->view = glm::mat4(1.0f);

		this->view = glm::translate(this->view, glm::vec3(0.0f, 0.0f, -3.0f));
		this->projection = glm::perspective(
			glm::radians(45.0f),
			ASPECT_RATIO,
			0.1f,
			100.0f
		);
	}

	void Render() override
	{
		this->program->Use();
		float frame_time = 1.0f / (*this->fps);

		this->model = glm::rotate(
			this->model,
			(float) (glm::radians(50.0f * frame_time)),
			glm::vec3(0.5f, 1.0f, 0.0f)
		);

		this->program->SetMat4("model", this->model);
		this->program->SetMat4("view", this->cam_->GetViewMatrix());
		this->program->SetMat4("projection", this->cam_->GetProjectionMatrix());

		glBindVertexArray(this->vao);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, this->texture[0]);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, this->texture[1]);

		glDrawArrays(GL_TRIANGLES, 0, 36);


		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);

		glBindVertexArray(0);
	}
};
