#include "Tube.h"

void CTube::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {}

void CTube::Render()
{
	int ani;

	switch (this->type)
	{
	case 1:
		ani = TUBE_ANI_TYPE_1;
		break;
	case 2:
		ani = TUBE_ANI_TYPE_2;
		break;
	case 3:
		ani = TUBE_ANI_TYPE_3;
		break;
	case 4:
		ani = TUBE_ANI_TYPE_4;
		break;
	case 5:
		ani = TUBE_ANI_TYPE_5;
		break;
	case 6:
		ani = TUBE_ANI_TYPE_6;
		break;
	case 7:
		ani = TUBE_ANI_TYPE_7;
		break;
	case 8:
		ani = TUBE_ANI_TYPE_8;
		break;
	case 9:
		ani = TUBE_ANI_TYPE_9;
		break;
	case 10:
		ani = TUBE_ANI_TYPE_10;
		break;
	default:
		ani = TUBE_ANI_TYPE_1;
		break;
	}

	int alpha = 255;
	animation_set->at(ani)->Render(x, y, alpha);
	//RenderBoundingBox();
}

void CTube::SetState(int state) {}

void CTube::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = l + TUBE_BBOX_WIDTH;
	b = t - TUBE_BBOX_HEIGHT;
}