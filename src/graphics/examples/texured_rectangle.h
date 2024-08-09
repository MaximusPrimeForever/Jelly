#pragma once

#include <graphics/shaders.h>
#include <graphics/camera.h>
#include <graphics/render_target.h>

#include <stb_image.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "graphics/awesome_gl.h"


class TexturedRectangle : RenderTarget
{
private:
	GLuint vao;
	GLuint texture;
	ShaderProgram* program;
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;
public:
	float shift_x;
	float shift_y;
	float shift_z;

	TexturedRectangle(Camera* cam): RenderTarget(cam)
	{
		float vertices[] = {
		// positions          // colors           // texture coords
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
		};
		unsigned int indices[] = {
			0, 1, 3, // first triangle
			1, 2, 3  // second triangle
		};

		Shader shader_paths[] = {
			Shader{".\\shaders\\textured_rectangle\\vertex_tex_rect.glsl", GL_VERTEX_SHADER, 0},
			Shader{".\\shaders\\textured_rectangle\\frag_tex_rect.glsl", GL_FRAGMENT_SHADER, 0},
		};
		this->program = new ShaderProgram(shader_paths, sizeof(shader_paths) / sizeof(Shader));
		this->shift_x = 0.0f;
		this->shift_y = 0.0f;
		this->shift_z = 0.0f;

		GLuint vbo, ebo;
		glGenVertexArrays(1, &this->vao);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);

		// begin setup
		glBindVertexArray(this->vao);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		// Vertices
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// Color
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 3));
		glEnableVertexAttribArray(1);

		// Texture
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 6));
		glEnableVertexAttribArray(2);

		int width, height, nrChannels;
		unsigned char* data = stbi_load(".\\textures\\wood_box.jpg", &width, &height, &nrChannels, 0);

		glGenTextures(1, &this->texture);
		glBindTexture(GL_TEXTURE_2D, this->texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(data);
		this->program->Use();
		this->program->SetInt("tex0_data", AGL_SAMPLER_TEXTURE0);

		this->model = glm::mat4(1.0f);
		this->view = glm::mat4(1.0f);

		this->model = glm::rotate(
			this->model, 
			glm::radians(-55.0f), 
			glm::vec3(1.0, 0.0, 0.0)
		);
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

		glm::vec3 shift_vector = glm::rotate(
			glm::vec3(this->shift_x, this->shift_y, this->shift_z), 
			glm::radians(55.0f), 
			glm::vec3(1.0, 0.0, 0.0)
		);
		glm::mat4 move = glm::translate(this->model, shift_vector);

		this->program->SetMat4("model", move);
		this->program->SetMat4("view", this->view);
		this->program->SetMat4("projection", this->projection);

		glBindVertexArray(this->vao);
		glBindTexture(GL_TEXTURE_2D, this->texture);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);
	}
};
