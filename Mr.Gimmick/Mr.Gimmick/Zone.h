#pragma once

class CZone
{
public:
	float l, t, r, b, w, h;
	CZone(float l, float t, float r, float b)
	{
		this->l = l;
		this->t = t;
		this->r = r;
		this->b = b;
		this->w = r - l;
		this->h = t - b;
	}
	~CZone() {}
};
