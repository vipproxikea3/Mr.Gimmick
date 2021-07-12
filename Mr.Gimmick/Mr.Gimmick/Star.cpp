#include "Star.h"

CStar::CStar() : CGameObject()
{
}

void CStar::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x;
	top = y;
	right = x + STAR_BBOX_WIDTH;
	bottom = y - STAR_BBOX_HEIGHT;
}

void CStar::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	
}

void CStar::Render()
{
	int ani = STAR_ANI_ACTIVE;

	int alpha = 255;

	animation_set->at(ani)->Render(x, y, alpha);

	//RenderBoundingBox();
}

void CStar::SetState(int state)
{
	CGameObject::SetState(state);

	switch (state)
	{
	case STAR_STATE_ACTIVE:
		break;
	case STAR_STATE_DISABLE:
		break;
	}
}