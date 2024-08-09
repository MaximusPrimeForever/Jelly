#include "graphics/camera.h"

#include <glm/gtc/matrix_transform.hpp>

#include "settings.h"


Camera::Camera(glm::vec3 position, glm::vec3 target, float vfov)
{
	this->position = position;
	this->target = target;
	this->up = glm::vec3(0.0f, 1.0f, 0.0f);
	this->vfov = glm::radians(vfov);

	this->GenerateLookAtMat();
	this->projection = glm::perspective(
		this->vfov,
		ASPECT_RATIO,
		0.1f,
		100.0f
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


void Camera::GenerateLookAtMat()
{
	this->right = glm::normalize(glm::cross(this->up, this->position - this->target));
	this->look_at = glm::lookAt(
		this->position,
		this->target,
		this->up
	);
}

void Camera::SetPosition(glm::vec3 new_position, bool update_look_at)
{
	this->position = new_position;
	if (update_look_at) this->GenerateLookAtMat();
}

void Camera::SetTarget(glm::vec3 new_target, bool update_look_at)
{
	this->target = new_target;
	if (update_look_at) this->GenerateLookAtMat();
}

void Camera::SetParameters(glm::vec3 new_position, glm::vec3 new_target)
{
	this->position = new_position;
	this->target = new_target;
	this->GenerateLookAtMat();
}

void Camera::AddPositionDelta(glm::vec3 position_delta, bool update_look_at)
{
	this->SetPosition(this->position + position_delta, update_look_at);
}

void Camera::SetTargetRelToPos(glm::vec3 new_target, bool update_look_at)
{
	this->SetTarget(this->position + new_target, update_look_at);
}
