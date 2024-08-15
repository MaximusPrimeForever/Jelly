#include <string>
#include <iostream>
#include "io\file_io.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

std::string read_file_contents(const char* path) {
	std::string s;
	auto fp = std::fopen(path, "rb");

	std::fseek(fp, 0u, SEEK_END);
	auto size = std::ftell(fp);
	std::fseek(fp, 0u, SEEK_SET);
	s.resize(size);

	std::fread(&s[0], 1u, size, fp);
	std::fclose(fp);

	return s;
}

// Simple helper function to load an image into a OpenGL texture with common settings
bool LoadTextureFromFile(const char* filename, GLuint* out_texture, bool flip_vertically, bool generate_mipmaps, int* out_width, int* out_height)
{
    stbi_set_flip_vertically_on_load(flip_vertically);

    // Load from file
    int image_width = 0;
    int image_height = 0;
    int num_channels = 0;
    std::string filename_str = std::string(filename);

    GLuint image_channels = GL_RGB;
    int desired_channels = 3;
    if (filename_str.ends_with(".png"))
    {
        image_channels = GL_RGBA;
        desired_channels = 4;
    }
    if (filename_str.ends_with(".jpg") || filename_str.ends_with(".jpeg"))
    {
        image_channels = GL_RGB;
        desired_channels = 3;
    }

    unsigned char* image_data = stbi_load(filename, &image_width, &image_height, &num_channels, desired_channels);
    if (image_data == NULL)
        return false;

    if (num_channels != desired_channels) {
        std::cout << "Desired channels: " << num_channels << '\n';
        std::cout << "Found " << num_channels << " channels in file: " << filename_str << '\n';
        return false;
    }

    // Create a OpenGL texture identifier
    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Upload pixels into texture
    glTexImage2D(GL_TEXTURE_2D, 0, image_channels, image_width, image_height, 0, image_channels, GL_UNSIGNED_BYTE, image_data);
    stbi_image_free(image_data);

    if (generate_mipmaps) glGenerateMipmap(GL_TEXTURE_2D);

    *out_texture = image_texture;
    if (out_width != nullptr) *out_width = image_width;
    if (out_height != nullptr) *out_height = image_height;

    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_set_flip_vertically_on_load(false);

    return true;
}