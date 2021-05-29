#include "Cogwheel.h"

void CCogwheel::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {}

void CCogwheel::Render()
{
	int ani;

	switch (this->type)
	{
	case 1:
		ani = COGWHEEL_ANI_TOP_RIGHT;
		break;
	case 2:
		ani = COGWHEEL_ANI_BOT_RIGHT;
		break;
	case 3:
		ani = COGWHEEL_ANI_BOT_LEFT;
		break;
	case 4:
		ani = COGWHEEL_ANI_TOP_LEFT;
		break;
	default:
		ani = COGWHEEL_ANI_TOP_RIGHT;
		break;
	}

	int alpha = 255;
	animation_set->at(ani)->Render(x, y, alpha);
}

void CCogwheel::SetState(int state) {}

void CCogwheel::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x;
	top = y;
	right = left + COGWHEEL_BBOX_WIDTH;
	bottom = top - COGWHEEL_BBOX_HEIGHT;
}