#pragma once

#include "imgui.h"
#include "settings.h"
#include "graphics/camera.h"
#include "graphics/shaders.h"

class RenderTarget
{
protected:
	Camera* cam_;
	ImFont* font_;
	ShaderProgram* program_;
public:
	RenderTarget(Camera* cam, ShaderProgram* program = nullptr) : cam_(cam), font_(g_app_font), program_(program) {}
	virtual ~RenderTarget() = default;

	virtual void RenderGL() = 0;
	virtual void RenderImGui() = 0;

	void Render()
	{
		this->RenderGL();
		this->RenderImGui();
	}

};
