#pragma once
#include <string>
#include "superglue.h"

std::string read_file_contents(const char* path);

bool LoadTextureFromFile(const char* filename, GLuint* out_texture, bool flip_vertically = false, bool generate_mipmaps = true, int* out_width = nullptr, int* out_height = nullptr);
