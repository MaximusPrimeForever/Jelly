#pragma once

#include <graphics/shaders.h>
#include <graphics/render_target.h>

#include <stb_image.h>
#include "graphics/awesome_gl.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class AwesomeRectangle : RenderTarget
{
private:
	GLuint vao;
	GLuint texture[2];
	ShaderProgram* program;
	glm::mat4 trans_matrix;
public:
	float mix_value;

	AwesomeRectangle()
	{
		float vertices[] = {
		// positions          // colors           // texture coords
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
		};
		float awesome_face_tex_coords[] = {
			2.0f, 2.0f,   // top right
			2.0f, 0.0f,   // bottom right
			0.0f, 0.0f,   // bottom left
			0.0f, 2.0f    // top left 
		};

		unsigned int indices[] = {
			0, 1, 3, // first triangle
			1, 2, 3  // second triangle
		};

		Shader shader_paths[] = {
			Shader{".\\shaders\\awesome_rectangle\\vertex_aws_rect.glsl", GL_VERTEX_SHADER, 0},
			Shader{".\\shaders\\awesome_rectangle\\frag_aws_rect.glsl", GL_FRAGMENT_SHADER, 0},
		};
		this->program = new ShaderProgram(shader_paths, sizeof(shader_paths) / sizeof(Shader));

		GLuint vbo, vbo_face, ebo;
		glGenVertexArrays(1, &this->vao);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);
		glGenBuffers(1, &vbo_face);

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

		glBindBuffer(GL_ARRAY_BUFFER, vbo_face);
		glBufferData(GL_ARRAY_BUFFER, sizeof(awesome_face_tex_coords), awesome_face_tex_coords, GL_STATIC_DRAW);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*) 0);
		glEnableVertexAttribArray(3);

		int width, height, nrChannels;
		glGenTextures(2, this->texture);

		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load(".\\textures\\wood_box.jpg", &width, &height, &nrChannels, 0);
		glBindTexture(GL_TEXTURE_2D, this->texture[0]);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(data);
		
		data = stbi_load(".\\textures\\awesomeface.png", &width, &height, &nrChannels, 0);
		stbi_set_flip_vertically_on_load(false);
		glBindTexture(GL_TEXTURE_2D, this->texture[1]);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(data);

		this->program->Use();
		this->program->SetInt("tex0_data", AGL_SAMPLER_TEXTURE0);
		this->program->SetInt("tex1_data", AGL_SAMPLER_TEXTURE1);

		this->trans_matrix = glm::mat4(1.0f);
		this->trans_matrix = glm::translate(
			this->trans_matrix,
			glm::vec3(0.5, -0.5, 0.0)
		);
	}

	void Render() override
	{
		this->program->Use();

		this->program->SetFloat("mix_value", this->mix_value);
		this->trans_matrix = glm::rotate(
			this->trans_matrix,
			glm::sin((float)glfwGetTime()) * 0.25f,
			glm::vec3(0.0, 0.0, 1.0)
		);

		glUniformMatrix4fv(
			glGetUniformLocation(this->program->id, "trans"),
			1,
			GL_FALSE,
			glm::value_ptr(this->trans_matrix)
		);

		glBindVertexArray(this->vao);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, this->texture[0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, this->texture[1]);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);
	}
};