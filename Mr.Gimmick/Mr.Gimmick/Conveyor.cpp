#include "Conveyor.h"
#include "Utils.h"

void CConveyor::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {}

void CConveyor::Render()
{
	int ani;

	switch (this->direction)
	{
	case 1:
		ani = CONVEYOR_ANI_RIGHT;
		break;
	case -1:
		ani = CONVEYOR_ANI_LEFT;
		break;
	default:
		ani = CONVEYOR_ANI_RIGHT;
		break;
	}

	int alpha = 255;
	animation_set->at(ani)->Render(x, y, alpha);
	RenderBoundingBox();
}

void CConveyor::SetState(int state) {}

void CConveyor::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = l + CONVEYOR_BBOX_WIDTH;
	b = t - CONVEYOR_BBOX_HEIGHT;
}