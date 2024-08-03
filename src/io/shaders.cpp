#include <iostream>
#include <string>
#include "io/file_io.h"
#include "io/shaders.h"

#include "imgui_impl_opengl3_loader.h"

GLuint CompileShader(const char* shader_filepath, GLenum shader_type)
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
