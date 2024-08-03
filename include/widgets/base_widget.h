#pragma once

#include "imgui.h"

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
