#pragma once

#include <exception>

#include "superglue.h"
#include "io/file_io.h"

class Material
{
public:
	Material() = default;
	virtual ~Material() = default;
};

class PhongMaterial : Material
{
public:
	GLuint diffuse_texture;
	GLuint specular_texture;
	float shininess;

	PhongMaterial() : diffuse_texture(0), specular_texture(0), shininess(0.0f), Material() { }

	PhongMaterial(const char* diffuse_tex_path, const char* spec_tex_path, float shininess) : shininess(shininess), Material()
	{
		if (!LoadTextureFromFile(diffuse_tex_path, &this->diffuse_texture)) {
			throw std::exception("Failed to load image.");
		}
		if (!LoadTextureFromFile(spec_tex_path, &this->specular_texture)) {
			throw std::exception("Failed to load image.");
		}
	}
};