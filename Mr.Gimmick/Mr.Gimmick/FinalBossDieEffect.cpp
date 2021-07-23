#include "FinalBossDieEffect.h"

CFinalBossDieEffect::CFinalBossDieEffect(float init_x, float init_y)
{
	x = init_x;
	y = init_y;
	render_time = GetTickCount64();
}
void CFinalBossDieEffect::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {
	// Calculate dx, dy 
	CGameObject::Update(dt);

	if (visible && GetTickCount64() - render_time >= 2000)
		visible = false;

	x += dx;
	y += dy;
}

void CFinalBossDieEffect::Render()
{
	if (!visible)
		return;

	int ani = FINALBOSSDIEEFFECT_ANI_WHITE;
	int ani2 = FINALBOSSDIEEFFECT_ANI_WHITE2;
	animation_set->at(ani)->Render(x+5, y-10);
	animation_set->at(ani2)->Render(x+12, y-30);
	animation_set->at(ani)->Render(x+25, y-50);
	animation_set->at(ani2)->Render(x+10, y-63);
	animation_set->at(ani)->Render(x+20, y-33);
	animation_set->at(ani2)->Render(x+30, y-23);
	animation_set->at(ani2)->Render(x + 20, y - 5);
	animation_set->at(ani)->Render(x + 17, y - 23);
	animation_set->at(ani2)->Render(x + 8, y - 45);

	//RenderBoundingBox();
}

void CFinalBossDieEffect::SetState(int state) {}

void CFinalBossDieEffect::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x;
	top = y;
	right = left + FINALBOSSDIEEFFECT_BBOX_WIDTH;
	bottom = top - FINALBOSSDIEEFFECT_BBOX_HEIGHT;
}
