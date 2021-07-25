#include "StandBlackEnemy.h"
#include "Brick.h"
#include "Utils.h"
#include "Gimmick.h"
#include "Game.h"
#include "Star.h"
#include "PlayScene.h"
#include "Backup.h"

CStandBlackEnemy::CStandBlackEnemy() : CGameObject()
{
	this->score = 140;
	SetState(ENEMY_STATE_STAND);
}


void CStandBlackEnemy::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{

	CStar* star = ((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->GetStar();

	if (this->CheckAABB(star) && (star->GetState() == STAR_STATE_WALKING_LEFT || star->GetState() == STAR_STATE_WALKING_RIGHT) && this->GetState() == ENEMY_STATE_STAND)
		this->SetState(ENEMY_STATE_DIE);

	// Calculate dx, dy 
	CGameObject::Update(dt);

	// Simple fall down
	vy -= ENEMY_GRAVITY * dt;

	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();
	// turn off collision when die 
	if (state != ENEMY_STATE_DIE)
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
			}
			x = x0 + min_tx * dx + nx * 0.1f;
		}
	}


	//// clean up collision events
	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
}

void CStandBlackEnemy::Render()
{

	int ani = -1;
	if (state == ENEMY_STATE_DIE)
	{
		ani = ENEMY_ANI_DIE;
	}
	else
	{
		ani = ENEMY_ANI_STAND;
	}

	int alpha = 255;
	animation_set->at(ani)->Render(x, y, alpha);
	RenderBoundingBox();
}

void CStandBlackEnemy::SetState(int state)
{
	CGameObject::SetState(state);

	switch (state)
	{
	case ENEMY_STATE_STAND:
		vx = 0;
		vy = 0;
		break;
	case ENEMY_STATE_DIE:
		CBackup* backup = CBackup::GetInstance();
		backup->UpdateScore(backup->score + this->score);
		vx = nx * 0.05;
		vy = ENEMY_DIE_DEFLECT_SPEED;
		break;
	}
}

void CStandBlackEnemy::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = l + ENEMY_BBOX_WIDTH;
	b = t - ENEMY_BBOX_HEIGHT;
}