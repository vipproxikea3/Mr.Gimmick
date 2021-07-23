#include "Worm.h"
#include "Brick.h"
#include "Conveyor.h"
#include "InclinedBrick.h"
#include "Utils.h"
#include "Gimmick.h"
#include "Game.h"
#include "Star.h"
#include "PlayScene.h"
#include "Backup.h"

CWorm::CWorm() : CGameObject()
{
	//SetState(WORM_STATE_DIE);
}

CWorm::CWorm(int l) : CGameObject()
{
	length = l;
	SetState(WORM_STATE_WALKING);
	nx = 1;
	this->score = 90;
}

void CWorm::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (this->GetState() == WORM_STATE_DIE && !CGame::GetInstance()->InCamera(this) && CGame::GetInstance()->InLargeCamera(this) && this->visible == true) {
		this->visible = false;
	}
	if (!visible)
		return;

	CStar* star = ((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->GetStar();

	if (this->CheckAABB(star) && (star->GetState() == STAR_STATE_WALKING_LEFT || star->GetState() == STAR_STATE_WALKING_RIGHT) && this->GetState() == WORM_STATE_WALKING) {
		CBackup* backup = CBackup::GetInstance();
		backup->UpdateScore(backup->score + this->score);
		this->SetState(WORM_STATE_DIE);
	}

	if (firstLocation)
	{
		float tmp = y;
		GetPosition(leftLimit, tmp);
		rightLimit = leftLimit + (length - 1) * 16;
		firstLocation = false;
	}
	// Calculate dx, dy 
	CGameObject::Update(dt);

	// Simple fall down
	vy -= WORM_GRAVITY * dt;

	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();
	// turn off collision when die 
	if (state != WORM_STATE_DIE)
		CalcPotentialCollisions(coObjects, coEvents);

	if (state == WORM_STATE_DIE)
	{
		x += dx;
		y += dy;
	}
	else
	{
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

		if ((x <= leftLimit && vx < 0) || (x >= rightLimit && vx > 0))
			vx = -vx;
	}
	
	//// clean up collision events
	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
}

void CWorm::Render()
{
	if (!visible)
		return;

	int ani = -1;
	if (state == WORM_STATE_DIE)
	{
		if (nx > 0)
			ani = WORM_ANI_DIE_RIGHT;
		else
			ani = WORM_ANI_DIE_LEFT;
	}
	else
	{
		if (vx > 0)
		{
			ani = WORM_ANI_RIGHT;
		}
		else
		{
			ani = WORM_ANI_LEFT;
		}
	}

	int alpha = 255;
	animation_set->at(ani)->Render(x, y, alpha);
}

void CWorm::SetState(int state)
{
	CGameObject::SetState(state);

	switch (state)
	{
	case WORM_STATE_WALKING:
		vx = WORM_WALKING_SPEED;
		break;
	case WORM_STATE_DIE:
		vx = -nx*0.025;
		vy = WORM_DIE_DEFLECT_SPEED;
		break;
	}
}

void CWorm::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = l + WORM_BBOX_WIDTH;
	b = t - WORM_BBOX_HEIGHT;
}