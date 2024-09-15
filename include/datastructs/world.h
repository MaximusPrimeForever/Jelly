#pragma once

#include <vector>
#include <memory>

#include "imgui.h"
#include "settings.h"
#include "graphics/camera.h"
#include "graphics/render_target.h"
#include "graphics/shaders.h"
#include "graphics/phong_light.h"

constexpr size_t g_object_count = 100;
constexpr size_t g_light_count = 100;


class World
{
protected:
	Camera* cam_;
	ImFont* font_;

	std::vector<std::unique_ptr<RenderTarget>> objects_;

	DirectionalLight dir_light;
	std::vector<PointLight> point_lights_;
	std::vector<SpotLight> spot_lights_;

	ShaderProgram* light_program_;

public:
	World(Camera* cam, ShaderProgram* program = nullptr) : cam_(cam), font_(g_app_font), light_program_(program)
	{
		this->dir_light = DirectionalLight(
			glm::vec3(1.0f),
			glm::vec3(-0.2f, -1.0f, -0.3f),
			DEFAULT_LIGHT_AMBIENT_FACTOR,
			DEFAULT_LIGHT_DIFFUSE_FACTOR,
			DEFAULT_LIGHT_SPECULAR_FACTOR
		);
	}
	virtual ~World() = default;

	void AddObject(RenderTarget* rt)
	{
		if (this->objects_.size() >= g_object_count) {
			throw std::exception("ERROR: Cannot add more objects!");
		}

		this->objects_.push_back(std::make_unique<RenderTarget>(*rt));
	}

	void AddLight(PointLight light)
	{
		if (this->point_lights_.size() >= g_light_count) {
			throw std::exception("ERROR: Cannot add more point lights!");
		}

		this->point_lights_.push_back(light);
	}

	void AddLight(SpotLight light)
	{
		if (this->spot_lights_.size() >= g_light_count) {
			throw std::exception("ERROR: Cannot add more spot lights!");
		}

		this->spot_lights_.push_back(light);
	}

	void Render()
	{
		// Write dir light data to program

		// Write point light data to program

		// Write spotlight data to program
	}
};
