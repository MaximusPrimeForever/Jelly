#pragma once

#include <glm/glm.hpp>

#include "settings.h"

class Camera
{
private:

public:
	glm::vec3 position;
	glm::vec3 target;
	glm::vec3 right;
	glm::vec3 up;

	float vfov;
	float pitch = 0.0f;
	float yaw = 0.0f;
	float velocity = 0.0f;

	glm::mat4 look_at;
	glm::mat4 projection;

	Camera(glm::vec3 position, glm::vec3 target, float vfov, float velocity);
	void GenerateLookAtMat();
	glm::vec3 GenerateNormDirection() const;

	void SetPosition(glm::vec3 new_position, bool use_as_delta = false, bool update_look_at = true);
	void SetTarget(glm::vec3 new_target, bool use_as_delta = false, bool update_look_at = true);
	void SetTargetRelToPos(glm::vec3 new_target, bool update_look_at = true);
	void SetParameters(glm::vec3 new_position, glm::vec3 new_target);

	void SetVFov(float vfov);
	void SetPitch(float new_pitch, bool use_as_delta = false, bool update_look_at = true);
	void SetYaw(float new_yaw, bool use_as_delta = false, bool update_look_at = true);
};
