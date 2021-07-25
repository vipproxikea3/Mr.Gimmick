#include "SpecialBrick.h"

void CSpecialBrick::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {
}

void CSpecialBrick::Render()
{
	//RenderBoundingBox();
}

void CSpecialBrick::SetState(int state) {}

void CSpecialBrick::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = l + this->w;
	b = t - this->h;
}