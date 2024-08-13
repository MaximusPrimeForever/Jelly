#include "graphics/camera.h"

#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>

#include "settings.h"


Camera::Camera(glm::vec3 position, glm::vec3 world_up, float yaw, float pitch):
	front(glm::vec3(0.0f, 0.0f, -1.0f)),
	vfov(CAMERA_DEFAULT_VERTICAL_FOV),
	velocity(CAMERA_DEFAULT_VELOCITY),
	look_sensitivity(MOUSE_DEFAULT_SENSITIVITY)
{
	this->position = position;
	this->up = glm::vec3(0.0f, 1.0f, 0.0f);
	this->world_up = world_up;
	this->yaw = yaw;
	this->pitch = pitch;

	this->UpdateCameraVectors();
}

void Camera::ProcessAxisLockedMovement(CAMERA_DIRECTION direction, float delta_time)
{
	float new_velocity = (this->velocity * delta_time);
	switch (direction)
	{
	case CAMERA_DIRECTION::FORWARD:
		this->position += this->front * new_velocity;
		break;
	case CAMERA_DIRECTION::BACKWARD:
		this->position -= this->front * new_velocity;
		break;
	case CAMERA_DIRECTION::RIGHT:
		this->position += this->right * new_velocity;
		break;
	case CAMERA_DIRECTION::LEFT:
		this->position -= this->right * new_velocity;
		break;
	case CAMERA_DIRECTION::UP:
		this->position += this->world_up * new_velocity;
		break;
	case CAMERA_DIRECTION::DOWN:
		this->position -= this->world_up * new_velocity;
		break;
	}
}

void Camera::ProcessAxisFreeMovement(float yaw_offset, float pitch_offset, bool constrain_pitch)
{
	this->yaw += yaw_offset * this->look_sensitivity;
	this->pitch += pitch_offset * this->look_sensitivity;

	if (constrain_pitch) {
		this->pitch = std::clamp(
			this->pitch, 
			CAMERA_BOTTOM_PITCH_LIMIT, 
			CAMERA_TOP_PITCH_LIMIT
		);
	}

	this->UpdateCameraVectors();
}

void Camera::SetVerticalFov(float new_vfov)
{
	this->vfov = std::clamp(new_vfov, CAMERA_MIN_VERTICAL_FOV, CAMERA_MAX_VERTICAL_FOV);
}

// returns the view matrix calculated using Euler Angles and the LookAt Matrix
glm::mat4 Camera::GetViewMatrix() const
{
	return glm::lookAt(this->position, this->position + this->front, this->up);
}

glm::mat4 Camera::GetProjectionMatrix() const
{
	return glm::perspective(
		glm::radians(this->vfov),
		ASPECT_RATIO,
		CAMERA_MIN_DRAW_DISTANCE,
		CAMERA_MAX_DRAW_DISTANCE
	);
}

void Camera::UpdateCameraVectors()
{
	// calculate the new Front vector
	glm::vec3 new_front = glm::vec3(
		glm::cos(glm::radians(this->yaw)) * glm::cos(glm::radians(this->pitch)),
		glm::sin(glm::radians(this->pitch)),
		glm::sin(glm::radians(this->yaw)) * glm::cos(glm::radians(this->pitch))
	);
	this->front = glm::normalize(new_front);

	// also re-calculate the Right and Up vector
	this->right = glm::normalize(glm::cross(this->front, this->world_up));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	this->up = glm::normalize(glm::cross(this->right, this->front));
}
