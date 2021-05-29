#include "GimmickDieEffect.h"

void CGimmickDieEffect::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {
	// Calculate dx, dy 
	CGameObject::Update(dt);

	x += dx;
	y += dy;
}

void CGimmickDieEffect::Render()
{
	int ani = GIMMICKDIEEFFECT_ANI_ACTIVE;

	int alpha = 255;
	animation_set->at(ani)->Render(x, y, alpha);
}

void CGimmickDieEffect::SetState(int state) {}

void CGimmickDieEffect::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x;
	top = y;
	right = left + GIMMICKDIEEFFECT_BBOX_WIDTH;
	bottom = top - GIMMICKDIEEFFECT_BBOX_HEIGHT;
}