#pragma once

class RenderTarget
{
public:
	virtual void Render();
};

inline void RenderTarget::Render() {};