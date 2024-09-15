#pragma once

#include <glm/glm.hpp>
#include "consts.h"


class PhongLight
{
public:
	glm::vec3 position;
	glm::vec3 color;
	float ambient;
	float diffuse;
	float specular;

	PhongLight() : position(glm::vec3()), color(glm::vec3()), ambient(0.0f), diffuse(0.0f), specular(0.0f) {}

	PhongLight(glm::vec3 position, glm::vec3 color, float ambient, float diffuse, float specular) : position(position), color(color), ambient(ambient), diffuse(diffuse), specular(specular) {}

	glm::vec3 GetAmbientColor() const { return this->color * this->ambient; }
	glm::vec3 GetDiffuseColor() const { return this->color * this->diffuse; }
	glm::vec3 GetSpecularColor() const { return this->color * this->specular; }
};

class DirectionalLight : public PhongLight
{
public:
	glm::vec3 direction;
	
	DirectionalLight() : direction(glm::vec3()), PhongLight() {}
	DirectionalLight(glm::vec3 color, glm::vec3 direction, float ambient, float diffuse, float specular) : PhongLight(glm::vec3(0.f), color, ambient, diffuse, specular), direction(direction) {}

	DirectionalLight(glm::vec3 color, glm::vec3 direction) : PhongLight(glm::vec3(0.f), color, DEFAULT_LIGHT_AMBIENT_FACTOR, DEFAULT_LIGHT_DIFFUSE_FACTOR, DEFAULT_LIGHT_SPECULAR_FACTOR), direction(direction) {}
};

class PointLight : public PhongLight
{
public:
	float constant;
	float linear;
	float quadratic;

	PointLight() : constant(0.0f), linear(0.0f), quadratic(0.0f), PhongLight() {}
	PointLight(glm::vec3 position, glm::vec3 color, float ambient, float diffuse, float specular, float constant, float linear, float quadratic) : PhongLight(position, color, ambient, diffuse, specular), constant(constant), linear(linear), quadratic(quadratic) {}

	PointLight(glm::vec3 position, glm::vec3 color, float distance) : PhongLight(position, color, DEFAULT_LIGHT_AMBIENT_FACTOR, DEFAULT_LIGHT_DIFFUSE_FACTOR, DEFAULT_LIGHT_SPECULAR_FACTOR)
	{
		auto light_settings = POINT_LIGHT_VALUES.find(distance);
		if (light_settings != POINT_LIGHT_VALUES.end())
		{
			this->constant = light_settings->second[0];
			this->linear = light_settings->second[1];
			this->quadratic = light_settings->second[2];
		} else {
			auto it_upper = POINT_LIGHT_VALUES.lower_bound(distance);

			// Interpolate light values based on distance
			if (it_upper == POINT_LIGHT_VALUES.begin())
			{
				float diff = distance / it_upper->first;

				this->constant = it_upper->second[0];
				this->linear = it_upper->second[1] * diff;
				this->quadratic = it_upper->second[2] * diff * diff;
			}
			if (it_upper == POINT_LIGHT_VALUES.end())
			{
				float diff = distance / std::prev(it_upper)->first;
				this->constant = it_upper->second[0];
				this->linear = it_upper->second[1] * diff;
				this->quadratic = it_upper->second[2] * diff * diff;
			}
			else {
				auto it_lower = std::prev(it_upper);

				this->constant = glm::mix(it_upper->second[0], it_lower->second[0],  0.5);
				this->linear = glm::mix(it_upper->second[1], it_lower->second[1], 0.5);
				this->quadratic = glm::mix(it_upper->second[2], it_lower->second[2], 0.5);
			}
		}

	}
};

class SpotLight : public PointLight
{
public:
	glm::vec3 direction;
	float inner_cutoff;
	float outer_cutoff;

	SpotLight() : direction(glm::vec3()), inner_cutoff(0.0f), outer_cutoff(0.0f), PointLight() {}

	SpotLight(glm::vec3 position, glm::vec3 direction, glm::vec3 color, float ambient, float diffuse, float specular, float constant, float linear, float quadratic, float inner_cutoff, float outer_cutoff) : PointLight(position, color, ambient, diffuse, specular, constant, linear, quadratic), direction(direction), inner_cutoff(glm::cos(glm::radians(inner_cutoff))), outer_cutoff(glm::cos(glm::radians(outer_cutoff))) {}

	SpotLight(glm::vec3 position, glm::vec3 direction, glm::vec3 color, float distance) : PointLight(position, color, distance), direction(direction), inner_cutoff(glm::cos(glm::radians(DEFAULT_SPOTLIGHT_INNER_CUTOFF))), outer_cutoff(glm::cos(glm::radians(DEFAULT_SPOTLIGHT_OUTER_CUTOFF))) {}

	SpotLight(glm::vec3 position, glm::vec3 direction, glm::vec3 color, float distance, float inner_cutoff, float outer_cutoff) : PointLight(position, color, distance), direction(direction), inner_cutoff(glm::cos(glm::radians(inner_cutoff))), outer_cutoff(glm::cos(glm::radians(outer_cutoff))) {}
};
