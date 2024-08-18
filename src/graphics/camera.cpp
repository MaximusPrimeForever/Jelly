#include "graphics/camera.h"

#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

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
	glm::vec3 position_delta = glm::vec3(0.0f);
	switch (direction)
	{
		case CAMERA_DIRECTION::FORWARD:
			position_delta = this->front * new_velocity;
			break;
		case CAMERA_DIRECTION::BACKWARD:
			position_delta = -this->front * new_velocity;
			break;
		case CAMERA_DIRECTION::RIGHT:
			position_delta = this->right * new_velocity;
			break;
		case CAMERA_DIRECTION::LEFT:
			position_delta = -this->right * new_velocity;
			break;
		case CAMERA_DIRECTION::UP:
			position_delta = this->world_up * new_velocity;
			break;
		case CAMERA_DIRECTION::DOWN:
			position_delta = -this->world_up * new_velocity;
			break;
	}

	this->position += position_delta;
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

// Currently rotates around 0,0,0
void Camera::ProcessRotationAroundOrigin(float yaw_offset, float pitch_offset, bool constrain_pitch)
{
	// Rotate camera vertically around camera-right axis
	glm::vec3 new_position = glm::rotate(
		this->position,
		glm::radians(pitch_offset * MOUSE_DEFAULT_SENSITIVITY),
		this->right
	);

	new_position = glm::rotate(
		new_position,
		glm::radians(-yaw_offset * MOUSE_DEFAULT_SENSITIVITY),
		this->world_up
	);
	this->position = new_position;
	this->ProcessAxisFreeMovement(yaw_offset, pitch_offset, constrain_pitch);
}

void Camera::SetDistanceDelta(float distance_delta, float delta_time)
{
	if (abs(distance_delta) < 0.00001f) return;

	CAMERA_DIRECTION dir = distance_delta > 0.0f ? CAMERA_DIRECTION::FORWARD : CAMERA_DIRECTION::BACKWARD;
	this->ProcessAxisLockedMovement(dir, delta_time * MOUSE_SCROLL_SENSITIVITY);
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
