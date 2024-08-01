#pragma once

#include <glad/glad.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

class BaseWidget
{
protected:
	double x_pos;
	double y_pos;
	ImFont* font;
public:
	BaseWidget(double x_pos, double y_pos, ImFont* font);
	virtual void Show();
};

inline BaseWidget::BaseWidget(double x_pos, double y_pos, ImFont* font)
	: x_pos{ x_pos }, y_pos{ y_pos }, font{ font } {}

inline void BaseWidget::Show() {};
