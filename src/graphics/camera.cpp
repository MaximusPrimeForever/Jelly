#include "graphics/camera.h"

#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>

#include "settings.h"


Camera::Camera(glm::vec3 position, glm::vec3 target, float vfov, float velocity)
{
	this->position = position;
	this->target = target;
	this->up = glm::vec3(0.0f, 1.0f, 0.0f);
	this->vfov = glm::radians(vfov);
	this->velocity = velocity;

	this->GenerateLookAtMat();
	this->projection = glm::perspective(
		this->vfov,
		ASPECT_RATIO,
		0.1f,
		100.0f
	);
}

glm::vec3 Camera::GenerateNormDirection() const
{
	return glm::normalize(glm::vec3(
		glm::cos(glm::radians(this->yaw)) * glm::cos(glm::radians(this->pitch)),
		glm::sin(glm::radians(this->pitch)),
		glm::sin(glm::radians(this->yaw)) * glm::cos(glm::radians(this->pitch))
	));
}


void Camera::GenerateLookAtMat()
{
	this->right = glm::normalize(glm::cross(this->up, this->position - this->target));
	this->look_at = glm::lookAt(
		this->position,
		this->target,
		this->up
	);
}

void Camera::SetVFov(float new_vfov)
{
	this->vfov = glm::radians(new_vfov);
	this->projection = glm::perspective(
		this->vfov,
		ASPECT_RATIO,
		0.1f,
		100.0f
	);
}

void Camera::SetPitch(float new_pitch, bool use_as_delta, bool update_look_at)
{
	// Limit pitch to avoid weird camera movements
	new_pitch = std::clamp(new_pitch, CAMERA_BOTTOM_PITCH_LIMIT, CAMERA_TOP_PITCH_LIMIT);
	if (use_as_delta) this->pitch += new_pitch;
	else this->pitch = new_pitch;

	if (update_look_at) {
		this->target = this->GenerateNormDirection();
		this->GenerateLookAtMat();
	}
}

void Camera::SetYaw(float new_yaw, bool use_as_delta, bool update_look_at)
{
	if (use_as_delta) this->yaw += new_yaw;
	else this->yaw = new_yaw;

	if (update_look_at) {
		this->target = this->GenerateNormDirection();
		this->GenerateLookAtMat();
	}
}

void Camera::SetPosition(glm::vec3 new_position, bool use_as_delta, bool update_look_at)
{
	if (use_as_delta) this->position += new_position;
	else this->position = new_position;

	if (update_look_at) this->GenerateLookAtMat();
}

void Camera::SetTarget(glm::vec3 new_target, bool use_as_delta, bool update_look_at)
{
	if (use_as_delta) this->target += new_target;
	else this->target = new_target;

	if (update_look_at) this->GenerateLookAtMat();
}

void Camera::SetParameters(glm::vec3 new_position, glm::vec3 new_target)
{
	this->position = new_position;
	this->target = new_target;
	this->GenerateLookAtMat();
}

void Camera::SetTargetRelToPos(glm::vec3 new_target, bool update_look_at)
{
	this->SetTarget(this->position + new_target, update_look_at);
}
