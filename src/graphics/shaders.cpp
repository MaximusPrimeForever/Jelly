#include <iostream>
#include <string>
#include "io/file_io.h"
#include "graphics/shaders.h"

#include <format>
#include <glm/gtc/type_ptr.hpp>

#include "superglue.h"

ShaderProgram::ShaderProgram(Shader* shaders, size_t count)
{
	this->id = ShaderProgram::BuildProgram(shaders,count, true);
}

void ShaderProgram::Use() const
{
	glUseProgram(this->id);
}

void ShaderProgram::Invalidate() const
{
	glUseProgram(0);
}

void ShaderProgram::SetBool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(this->id, name.c_str()), (int)value);
}
void ShaderProgram::SetInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(this->id, name.c_str()), value);
}
void ShaderProgram::SetFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(this->id, name.c_str()), value);
}

void ShaderProgram::SetVec2(const std::string& name, glm::vec2 value) const
{
	glUniform2fv(
		glGetUniformLocation(this->id, name.c_str()),
		1,
		glm::value_ptr(value)
	);
}

void ShaderProgram::SetVec3(const std::string& name, glm::vec3 value) const
{
	glUniform3fv(
		glGetUniformLocation(this->id, name.c_str()),
		1,
		glm::value_ptr(value)
	);
}

void ShaderProgram::SetMat3(const std::string& name, glm::mat3 value) const
{
	glUniformMatrix3fv(
		glGetUniformLocation(this->id, name.c_str()),
		1,
		GL_FALSE,
		glm::value_ptr(value)
	);
}

void ShaderProgram::SetMat4(const std::string& name, glm::mat4 value) const
{
	glUniformMatrix4fv(
		glGetUniformLocation(this->id, name.c_str()),
		1,
		GL_FALSE,
		glm::value_ptr(value)
	);
}

GLuint ShaderProgram::CompileShader(const char* shader_filepath, GLenum shader_type)
{
	std::string file_contents = read_file_contents(shader_filepath);
	const char* shaderCode = file_contents.c_str();
	GLuint shader = glCreateShader(shader_type);
	glShaderSource(shader, 1, &shaderCode, NULL);
	glCompileShader(shader);

	int compilationSuccess = 0;

	glGetShaderiv(shader, GL_COMPILE_STATUS, &compilationSuccess);
	if (!compilationSuccess) {
		char compilationOutput[1024];
		glGetShaderInfoLog(shader, sizeof(compilationOutput), NULL, compilationOutput);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << compilationOutput << std::endl;

		return NULL;
	}
	return shader;
}

GLuint ShaderProgram::BuildProgram(Shader* shaders, size_t count, bool shouldCleanUp)
{
	GLuint shader_program = glCreateProgram();

	// Compile shaders and add to program
	for (size_t i = 0; i < count; ++i)
	{
		Shader* current_shader = &shaders[i];
		GLuint shader_id = CompileShader(current_shader->file_path, current_shader->type);
		if (!shader_id)
		{
			throw std::exception(std::format(
				"Failed to compile {} shader ({})!", std::to_string(current_shader->type), current_shader->file_path).c_str()
			);
		}
		glAttachShader(shader_program, shader_id);
		current_shader->id = shader_id;
	}

	// Link shaders
	GLint linkSuccess;
	glLinkProgram(shader_program);
	glGetProgramiv(shader_program, GL_LINK_STATUS, &linkSuccess);
	if (!linkSuccess) {
		char compilationOutput[1024];
		glGetProgramInfoLog(shader_program, sizeof(compilationOutput), NULL, compilationOutput);
		std::cout << "Failed to link shaders. Reason: " << compilationOutput << '\n';
		throw std::exception("Failed to link shaders.");
	}

	if (shouldCleanUp)
	{
		for (size_t i = 0; i < count; ++i)
		{
			Shader* current_shader = &shaders[i];
			glDeleteShader(current_shader->id);
		}
	}

	return shader_program;
}