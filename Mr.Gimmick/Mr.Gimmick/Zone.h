#pragma once

class CZone
{
public:
	float l, t, r, b, w, h, revival_x, revival_y;
	CZone(float l, float t, float r, float b, float revival_x, float revival_y)
	{
		this->l = l;
		this->t = t;
		this->r = r;
		this->b = b;
		this->w = r - l;
		this->h = t - b;
		this->revival_x = revival_x;
		this->revival_y = revival_y;
	}
	~CZone() {}
};
