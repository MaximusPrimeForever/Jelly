#pragma once
#include <map>
#include <array>
#include <glm/glm.hpp>

constexpr float DEFAULT_LIGHT_AMBIENT_FACTOR = 0.1f;
constexpr float DEFAULT_LIGHT_DIFFUSE_FACTOR = 0.5f;
constexpr float DEFAULT_LIGHT_SPECULAR_FACTOR = 1.0f;

constexpr float DEFAULT_SPOTLIGHT_INNER_CUTOFF = 10.0f;
constexpr float DEFAULT_SPOTLIGHT_OUTER_CUTOFF = 14.0f;

inline std::map<float, std::array<float, 3>> POINT_LIGHT_VALUES = {
	{7.0f,	{1.0f, 0.7f, 1.8f} },
	{13.0f, {1.0f, 0.35f, 0.44f}},
	{20.0f, {1.0f, 0.22f, 0.20f}},
	{32.0f, {1.0f, 0.14f, 0.07f}},
	{50.0f, {1.0f, 0.09f, 0.032f}},
	{65.0f, {1.0f, 0.07f, 0.017f}},
	{100.0f, {1.0f, 0.045f, 0.0075f}},
	{160.0f, {1.0f, 0.027f, 0.0028f}},
	{200.0f, {1.0f, 0.022f, 0.0019f}},
	{325.0f, {1.0f, 0.014f, 0.0007f}},
	{600.0f, {1.0f, 0.007f, 0.0002f}},
	{3250.0f, {1.0f, 0.0014f, 0.000007f}}
};

const glm::vec3 X_VEC_UNIT = glm::vec3(1.0f, 0.0f, 0.0f);
const glm::vec3 Y_VEC_UNIT = glm::vec3(0.0f, 1.0f, 0.0f);
const glm::vec3 Z_VEC_UNIT = glm::vec3(0.0f, 0.0f, 1.0f);
