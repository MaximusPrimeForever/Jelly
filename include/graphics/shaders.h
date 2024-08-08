#pragma once

#include <string>
#include <glm/detail/type_mat.hpp>
#include <glm/detail/type_vec.hpp>

#include "superglue.h"

typedef struct shader_t
{
	const char* file_path;
	GLuint type;
	GLuint id;
} Shader;

class ShaderProgram
{
private:
	GLuint static CompileShader(const char* shader_filepath, GLenum shader_type);
	GLuint static BuildProgram(Shader* shaders, size_t count, bool shouldCleanUp);
public:
	ShaderProgram(Shader* shaders, size_t count);
	GLuint id;

	// use/activate the shader
	void Use() const;

	// utility uniform functions
	void SetBool(const std::string& name, bool value) const;
	void SetInt(const std::string& name, int value) const;
	void SetFloat(const std::string& name, float value) const;
	void SetVec3(const std::string& name, glm::vec3 value) const;
	void SetMat4(const std::string& name, glm::mat4 value) const;
};

