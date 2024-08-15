#pragma once
#include <string>
#include "superglue.h"

std::string read_file_contents(const char* path);

bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height, bool flip_vertically = false, bool generate_mipmaps = true);
