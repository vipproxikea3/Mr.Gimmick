#include "CogwheelSmall.h"

void CCogwheelSmall::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {}

void CCogwheelSmall::Render()
{
	int ani = COGWHEELSMALL_ANI_SPIN;

	int alpha = 255;
	animation_set->at(ani)->Render(x, y, alpha);
}

void CCogwheelSmall::SetState(int state) {}

void CCogwheelSmall::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x;
	top = y;
	right = x + COGWHEELSMALL_BBOX_WIDTH;
	bottom = y + COGWHEELSMALL_BBOX_HEIGHT;
}