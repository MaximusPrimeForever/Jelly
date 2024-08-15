#pragma once

#include "camera.h"

class RenderTarget
{
protected:
	Camera* cam_;
public:
	RenderTarget(Camera* cam) : cam_(cam) {};
	virtual void Render() {};
	virtual ~RenderTarget() = default;
};
