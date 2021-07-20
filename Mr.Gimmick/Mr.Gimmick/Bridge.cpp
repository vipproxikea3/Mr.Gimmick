#include "Bridge.h"

void CBridge::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {}

void CBridge::Render()
{
	int ani;

	switch (this->type)
	{
	case 1:
		ani = BRIDGE_ANI_TYPE_1_TOP;
		break;
	case 2:
		ani = BRIDGE_ANI_TYPE_2_TOP;
		break;
	case 3:
		ani = BRIDGE_ANI_TYPE_3_TOP;
		break;
	case 4:
		ani = BRIDGE_ANI_TYPE_4_TOP;
		break;
	case 5:
		ani = BRIDGE_ANI_TYPE_1_DOWN;
		break;
	case 6:
		ani = BRIDGE_ANI_TYPE_2_DOWN;
		break;
	case 7:
		ani = BRIDGE_ANI_TYPE_3_DOWN;
		break;
	case 8:
		ani = BRIDGE_ANI_TYPE_4_DOWN;
		break;
	default:
		ani = BRIDGE_ANI_TYPE_1_TOP;
		break;
	}

	int alpha = 255;
	animation_set->at(ani)->Render(x, y, alpha);
	//RenderBoundingBox();
}

void CBridge::SetState(int state) {}

void CBridge::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = l + BRIDGE_BBOX_WIDTH;
	b = t - BRIDGE_BBOX_HEIGHT;
}