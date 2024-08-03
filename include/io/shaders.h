#pragma once

#include "superglue.h"

GLuint CompileShader(const char* shader_filepath, GLenum shader_type);
GLuint LinkProgram();
