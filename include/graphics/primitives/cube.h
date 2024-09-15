#pragma once

#include "superglue.h"
#include <glm/gtc/matrix_transform.hpp>

#include "graphics/awesome_gl.h"
#include "graphics/consts.h"
#include "graphics/render_target.h"
#include "graphics/shaders.h"
#include "graphics/material.h"
#include "io/file_io.h"

class Cube
{

public:
	glm::vec3 position;
	glm::vec3 orientation;

	ShaderProgram program;
	glm::mat4 model;

	GLuint vao;
	constexpr GLuint stride = 8;
	PhongMaterial material;

	Cube(
		glm::vec3 position,
		glm::vec3 orientation,
		float vertices[],
		size_t vertices_size,
		const char* vertex_path,
		const char* fragment_path,
		const char* texture_diffuse_path,
		const char* texture_specular_path,
		float shininess
	) : position(position), orientation(orientation)
	{
		this->model = glm::mat4(1.0f);
		this->model = glm::translate(this->model, this->position);

		this->model = glm::rotate(this->model, this->orientation.x, X_VEC_UNIT);
		this->model = glm::rotate(this->model, this->orientation.y, Y_VEC_UNIT);
		this->model = glm::rotate(this->model, this->orientation.z, Z_VEC_UNIT);

		this->material = PhongMaterial(texture_diffuse_path, texture_specular_path, shininess);

		this->InitializeGl(vertices, vertices_size);

		Shader object_shaders[] = {
			Shader{vertex_path, GL_VERTEX_SHADER, 0},
			Shader{fragment_path, GL_FRAGMENT_SHADER, 0},
		};

		this->program = ShaderProgram(
			object_shaders,
			sizeof(object_shaders) / sizeof(Shader)
		);
	}

	void InitializeGl(
		float vertices[],
		size_t vertices_size
	) {
		// Setup obj
		GLuint vbo;
		GLuint vertex_attrb;
		glGenVertexArrays(1, &this->vao);
		glGenBuffers(1, &vbo);

		/// Load vertices data
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices_size, vertices, GL_STATIC_DRAW);

		glBindVertexArray(this->vao);

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

		glBindTexture(GL_TEXTURE_2D, this->material.diffuse_texture);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0);

		glBindTexture(GL_TEXTURE_2D, this->material.specular_texture);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0);

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Render()
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, this->material.diffuse_texture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, this->material.specular_texture);

		this->program.SetInt("uMaterial.diffuse", AGL_SAMPLER_TEXTURE0);
		this->program.SetInt("uMaterial.specular", AGL_SAMPLER_TEXTURE1);
		this->program.SetFloat("uMaterial.shininess", this->material.shininess);
	}
};
