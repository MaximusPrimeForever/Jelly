#pragma once

#include <graphics/shaders.h>
#include <graphics/render_target.h>

#include <graphics/camera.h>
#include <graphics/awesome_gl.h>

#include <glm/gtc/matrix_transform.hpp>

#include "io/file_io.h"

#define CELL_COUNT (20.0f)

class XzGrid : RenderTarget
{
private:
	GLuint grid_vao_;
	GLuint texture;
	ShaderProgram* grid_program_;

	glm::mat4 grid_model_mat_;

public:
	XzGrid(Camera* cam) : RenderTarget(cam)
	{
		GLuint vbo, ebo;
		float vertices[] = {
			-1.0, -1.0, 0.0,		0.0,
			-1.0,  1.0, 0.0,		CELL_COUNT,
			 1.0, -1.0, CELL_COUNT, 0.0,
			 1.0,  1.0, CELL_COUNT, CELL_COUNT
		};
		GLuint indices[] = {
			0,1,2,
			3,2,1
		};

		Shader grid_shaders[] = {
			Shader{".\\shaders\\xz_grid\\vertex_xz_grid.glsl", GL_VERTEX_SHADER, 0},
			Shader{".\\shaders\\xz_grid\\frag_xz_grid.glsl", GL_FRAGMENT_SHADER, 0},
		};
		this->grid_program_ = new ShaderProgram(grid_shaders, sizeof(grid_shaders) / sizeof(Shader));

		// Setup obj
		glGenVertexArrays(1, &this->grid_vao_);
		glBindVertexArray(this->grid_vao_);

		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		/// Vertices
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), static_cast<void*>(0));
		glEnableVertexAttribArray(0);

		/// Texture coords
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		glEnableVertexAttribArray(1);

		/// Elements
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		if (!LoadTextureFromFile(".\\textures\\grid_cell.png", &this->texture)) {
			throw std::exception("Failed to load image.");
		}
		glBindTexture(GL_TEXTURE_2D, this->texture);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glBindTexture(GL_TEXTURE_2D, 0);

		this->grid_program_->Use();
		this->grid_model_mat_ = glm::mat4(1.0f);
		this->grid_model_mat_ = glm::scale(this->grid_model_mat_, glm::vec3(2.0f));
		this->grid_program_->SetMat4("model", this->grid_model_mat_);
		this->grid_program_->SetInt("tex", AGL_SAMPLER_TEXTURE0);
	}

	void RenderImGui() override {}

	void RenderGL() override
	{
		// Render objects
		{
			this->grid_program_->Use();

			this->grid_program_->SetMat4("view", this->cam_->GetViewMatrix());
			this->grid_program_->SetMat4("projection", this->cam_->GetProjectionMatrix());

			glBindVertexArray(this->grid_vao_);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, this->texture);

			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		glBindVertexArray(0);
	}
};
