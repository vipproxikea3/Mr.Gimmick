#include "Brick.h"

void CBrick::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {}

void CBrick::Render()
{
	RenderBoundingBox();
}

void CBrick::SetState(int state) {}

void CBrick::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = l + BRICK_BBOX_WIDTH;
	b = t - BRICK_BBOX_HEIGHT;
}