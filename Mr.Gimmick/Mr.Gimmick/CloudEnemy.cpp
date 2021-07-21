#include "CloudEnemy.h"
#include "Brick.h"
#include "Utils.h"
#include "Gimmick.h"
#include "Game.h"
#include "Star.h"
#include "PlayScene.h"



CCloudEnemy::CCloudEnemy() : CGameObject()
{
	SetState(CLOUD_STATE_WALK);
}

void CCloudEnemy::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{

	CStar* star = ((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->GetStar();

	if (this->CheckAABB(star) && (star->GetState() == STAR_STATE_WALKING_LEFT || star->GetState() == STAR_STATE_WALKING_RIGHT) && (this->GetState() == CLOUD_STATE_WALK || this->GetState() == CLOUD_STATE_IDLE))
		star->SetState(STAR_STATE_EXPLOSIVE);

	CGameObject::Update(dt);

	// Simple fall down
	vy -= WORM_GRAVITY * dt;



	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();
	// turn off collision when die 
	if (state != WORM_STATE_DIE)
		CalcPotentialCollisions(coObjects, coEvents);




	if (coEvents.size() == 0)
	{
		x += dx;
		y += dy;
	}
	else
	{
		float min_tx, min_ty, nx = 0, ny = 0;
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
				y = y0 + min_ty * dy + ny * 0.1f;
				if (e->ny != 0)
				{
					vy = 0;
				}
				if (e->nx != 0)
				{
					vx = -vx;
				}
			}
			x = x0 + min_tx * dx + nx * 0.1f;
		}
	}



	//// clean up collision events
	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];

}

void CCloudEnemy::Render()
{

	int ani = -1;
	if (state == CLOUD_STATE_IDLE)
	{
		if (nx > 0)
			ani = CLOUD_ANI_IDLE_RIGHT;
		else
			ani = CLOUD_ANI_IDLE_LEFT;
	}
	else
	{
		if (vx > 0)
		{
			ani = CLOUD_ANI_GO_RIGHT;
		}
		else
		{
			ani = CLOUD_ANI_GO_LEFT;
		}
	}

	int alpha = 255;
	animation_set->at(ani)->Render(x, y, alpha);
}

void CCloudEnemy::SetState(int state)
{
	CGameObject::SetState(state);

	switch (state)
	{
	case CLOUD_STATE_IDLE:
		vx = 0;
		vy = 0;
		break;
	case CLOUD_STATE_WALK:
		vx = 0.025f;
	case CLOUD_STATE_ATTACK:

		break;
	case CLOUD_STATE_CARRY:

		break;
	case CLOUD_STATE_STUN:
		break;
	}
}

void CCloudEnemy::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = l + CLOUD_BBOX_WIDTH;
	b = t - CLOUD_BBOX_HEIGHT;
}

