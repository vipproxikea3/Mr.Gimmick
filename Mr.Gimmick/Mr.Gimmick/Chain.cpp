#include "Chain.h"

void CChain::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {}

void CChain::Render()
{
	int ani;

	switch (this->type)
	{
	case 1:
		ani = CHAIN_ANI_TOP_RIGHT;
		break;
	case 2:
		ani = CHAIN_ANI_BOT_RIGHT;
		break;
	case 3:
		ani = CHAIN_ANI_BOT_LEFT;
		break;
	case 4:
		ani = CHAIN_ANI_TOP_LEFT;
		break;
	case 5:
		ani = CHAIN_ANI_DOWN;
		break;
	case 6:
		ani = CHAIN_ANI_UP;
		break;
	default:
		ani = CHAIN_ANI_TOP_RIGHT;
		break;
	}

	int alpha = 255;
	animation_set->at(ani)->Render(x, y, alpha);
}

void CChain::SetState(int state) {}

void CChain::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x;
	top = y;
	right = x + CHAIN_BBOX_WIDTH;
	bottom = y + CHAIN_BBOX_HEIGHT;
}