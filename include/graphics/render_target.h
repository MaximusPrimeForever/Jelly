#pragma once

#include "camera.h"

class RenderTarget
{
protected:
	Camera* cam;
public:
	RenderTarget(Camera* cam) : cam(cam) {};
	virtual void Render() {};
	virtual ~RenderTarget() = default;
};
