#pragma once

#include <glm/glm.hpp>

class Camera
{
private:

public:
	glm::vec3 position;
	glm::vec3 target;
	glm::vec3 right;
	glm::vec3 up;
	float vfov;

	glm::mat4 look_at;
	glm::mat4 projection;

	Camera(glm::vec3 position, glm::vec3 target, float vfov);
	void GenerateLookAtMat();
	void SetPosition(glm::vec3 new_position, bool update_look_at = true);
	void SetTarget(glm::vec3 new_target, bool update_look_at = true);
	void SetTargetRelToPos(glm::vec3 new_target, bool update_look_at = true);
	void AddPositionDelta(glm::vec3 position_delta, bool update_look_at = true);
	void SetParameters(glm::vec3 new_position, glm::vec3 new_target);
	void SetVFov(float vfov);
};