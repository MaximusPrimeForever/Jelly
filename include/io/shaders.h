#pragma once

#include "superglue.h"

typedef struct shader_t
{
	const char* file_path;
	GLuint type;
	GLuint id;
} Shader;

GLuint CompileShader(const char* shader_filepath, GLenum shader_type);
GLuint BuildProgram(Shader* shaders, size_t count, bool shouldCleanUp);
