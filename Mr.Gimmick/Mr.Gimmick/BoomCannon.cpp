#include "BoomCannon.h"
#include "Brick.h"
#include "Cannon.h"
#include "Gimmick.h"
#include "Star.h"

CBoomCannon::CBoomCannon()
{
	SetState(BOOM_CANNON_STATE_MOVE);
}

void CBoomCannon::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (!visible)
		return;

	CGameObject::Update(dt);
	if (state == BOOM_CANNON_STATE_NORMAL) {
		vy = -BOOM_CANNON_GRAVITY * dt;
	}
	
	if (!CGame::GetInstance()->InCamera(this) 
		&& CGame::GetInstance()->InLargeCamera(this) 
		&& this->visible == true) {
		this->visible = false;
	}
	if (!visible)
		return;
	
	CGimmick* gimmick = ((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
	CStar* star = ((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->GetStar();

	/*if (this->CheckAABB(star) && (star->GetState() == STAR_STATE_WALKING_LEFT || star->GetState() == STAR_STATE_WALKING_RIGHT))
		this->SetState(BULLET_STATE_EXPLODE);*/

	/*if (this->CheckAABB(gimmick) && this->state == BOOM_CANNON_STATE_MOVE)
		gimmick->SetState(GIMMICK_STATE_STUN);*/

	vector<LPGAMEOBJECT> newCoObjects;
	for (UINT i = 0; i < coObjects->size(); i++)
	{
		if (dynamic_cast<CBrick*>(coObjects->at(i))) newCoObjects.push_back(coObjects->at(i));
	}

	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();
	CalcPotentialCollisions(&newCoObjects, coEvents);

	if (coEvents.size() == 0)
	{
		x += dx;
		y += dy;
	}
	else
	{
		float min_tx, min_ty, nx, ny;
		float rdx = 0;
		float rdy = 0;

		float x0 = x;
		float y0 = y;

		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);



		for (UINT i = 0; i < coEventsResult.size(); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];

			if (dynamic_cast<CBrick*>(e->obj))
			{
				//x = x0 + min_tx * dx + nx * 0.1f;
				//y = y0 + min_ty * dy + ny * 0.1f;

				if (e->ny != 0) vy = 0;
				if (e->nx != 0) {
					SetState(BOOM_CANNON_STATE_NORMAL);
					vx = 0.075f;
					vy = -BOOM_CANNON_GRAVITY * dt;
				}
			}
		}
	}
	timeDelete -= dt;
	if (timeDelete <= 0)
	{
		SetState(BOOM_CANNON_STATE_DIE);
	}
	else {
		if (state == BOOM_CANNON_STATE_NORMAL) {
			timePreDelete -= dt;
			if (timePreDelete <= 0)
			{
				isDelete = true;
				visible = false;
			}
		}
	}
}

void CBoomCannon::SetState(int state)
{
	CGameObject::SetState(state);

	switch (state)
	{
	case BOOM_CANNON_STATE_MOVE:
		nx = -1;
		break;
	case BOOM_CANNON_STATE_NORMAL:
		vy = -BOOM_CANNON_GRAVITY * dt;
		break;
	}
}

void CBoomCannon::Render()
{
	if (!visible)
		return;

	int alpha = 255;
	animation_set->at(0)->Render(x, y, alpha);

	//RenderBoundingBox();
}
void CBoomCannon::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	if (!visible)
		return;
	l = x;
	t = y - 1.0f;
	r = l + BOOM_CANNON_BOX_WIDTH;
	b = t - BOOM_CANNON_BOX_HEIGHT + 4.0f;
}
