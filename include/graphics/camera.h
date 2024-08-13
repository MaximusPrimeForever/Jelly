#pragma once

#include <glm/glm.hpp>

#include "settings.h"

typedef enum camera_direction
{
	FORWARD = 0,
	BACKWARD,
	RIGHT,
	LEFT,
	UP,
	DOWN
} CAMERA_DIRECTION;

class Camera
{
private:
	void UpdateCameraVectors();
public:
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 world_up;

	float vfov;
	float pitch = 0.0f;
	float yaw = 0.0f;
	float velocity = 0.0f;

	/*float yaw_offset = 0.0f;
	float pitch_offset = 0.0f;*/
	float look_sensitivity;

	Camera(glm::vec3 position = glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3 world_up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = CAMERA_DEFAULT_YAW, float pitch = CAMERA_DEFAULT_PITCH);

	glm::mat4 GetViewMatrix() const;
	glm::mat4 GetProjectionMatrix() const;

	void ProcessAxisLockedMovement(CAMERA_DIRECTION direction, float delta_time);
	void ProcessAxisFreeMovement(float yaw_offset, float pitch_offset, bool constrain_pitch = true);
	void SetVerticalFovDelta(float delta_fov);
};
