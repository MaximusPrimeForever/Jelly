#pragma once

#include "imgui.h"
#include "camera.h"
#include "settings.h"

class RenderTarget
{
protected:
	Camera* cam_;
	ImFont* font_;
public:
	RenderTarget(Camera* cam) : cam_(cam), font_(g_app_font) {}
	virtual ~RenderTarget() = default;

	virtual void RenderGL() = 0;
	virtual void RenderImGui() = 0;

	void Render()
	{
		this->RenderGL();
		this->RenderImGui();
	}

};
