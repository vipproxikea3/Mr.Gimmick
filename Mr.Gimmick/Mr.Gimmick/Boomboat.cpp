#include "Boomboat.h"
#include "PlayScene.h"
#include "algorithm"
#include "WaterDie.h"
#include "Star.h"
#include "Backup.h"

CBomboat::CBomboat() : CGameObject()
{
	this->score = 410;
	SetState(BOOM_BOAT_STATE_ALIVE);
}

void CBomboat::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x;
	top = y;
	right = x + BOOM_BOAT_BBOX_WIDTH;
	bottom = y - BOOM_BOAT_BBOX_HEIGHT;
}

void CBomboat::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	// Calculate dx, dy 
	CGameObject::Update(dt);

	if (this->state == BOOM_BOAT_STATE_FALLING)
	{
		vy += ay * dt;
		ay -= BOOM_BOAT_GRAVITY;
	}
	else
		ay = 0;

	vector<LPGAMEOBJECT> newCoObjects;
	for (UINT i = 0; i < coObjects->size(); i++)
	{
		if (dynamic_cast<CWaterDie*>(coObjects->at(i))) 
			newCoObjects.push_back(coObjects->at(i));
	}

	//DebugOut(L"\nVao day %d", newCoObjects.size());
	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	if (state != BOOM_BOAT_STATE_DIE)
		CalcPotentialCollisions(&newCoObjects, coEvents);

	if (state == BOOM_BOAT_STATE_EXPLODE)
	{
		if (explode_start == 0)
		{
			StartExplode();
		}
		if (GetTickCount64() - explode_start >= BOOM_BOAT_EXPLODE_TIME)
		{
			SetState(BOOM_BOAT_STATE_DIE);
		}
	}

	CStar* star = ((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->GetStar();
	if (this->CheckAABB(star) && (star->GetState() == STAR_STATE_WALKING_LEFT || star->GetState() == STAR_STATE_WALKING_RIGHT) && this->state == BOOM_BOAT_STATE_FALLING && vy < 0)
	{
		if(x>star->x)
			star->SetState(STAR_STATE_WALKING_LEFT);
		else
			star->SetState(STAR_STATE_WALKING_RIGHT);
		SetState(BOOM_BOAT_STATE_EXPLODE);
	}


	CGimmick* gimmick = ((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->GetPlayer();

	if (this->CheckAABB(gimmick) && this->state == BOOM_BOAT_STATE_FALLING && vy < 0) {
		if (gimmick->untouchable == 0)
			gimmick->SetState(GIMMICK_STATE_STUN);
	}

	if (state == BOOM_BOAT_STATE_ALIVE)
	{
		if (gimmick->y < this->y)
		{
			if (gimmick->x + GIMMICK_BBOX_WIDTH + 50 >= this->x)
			{
				SetState(BOOM_BOAT_STATE_FALLING);
			}
		}
	}

	//DebugOut(L"\nVao day %d", coEvents.size());
	if (coEvents.size() == 0)
	{
		x += dx;
		y += dy;
	}
	else
	{
		float min_tx, min_ty, nx = 0, ny;
		float rdx = 0;
		float rdy = 0;

		float x0 = x;
		float y0 = y;

		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);

		for (UINT i = 0; i < coEventsResult.size(); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];

			if (dynamic_cast<CWaterDie*>(e->obj))
			{
				CWaterDie* waterDie = dynamic_cast<CWaterDie*>(e->obj);
				if (e->ny != 0)
				{
					//DebugOut(L"[INFO] Collisions");
					SetState(BOOM_BOAT_STATE_EXPLODE);
				}
			}
		}
	}

	// clean up newCoObjects
	for (UINT i = 0; i < newCoObjects.size(); i++) newCoObjects[i] = nullptr;
	// clean up collision events
	for (UINT i = 0; i < coEvents.size(); i++)
		delete coEvents[i];
}

void CBomboat::Render()
{
	int ani = -1;

	if (state == BOOM_BOAT_STATE_DIE)
		return;
	else if (state == BOOM_BOAT_STATE_ALIVE)
		ani = BOOM_BOAT_ANI_ALIVE;
	else if (state == BOOM_BOAT_STATE_FALLING)
		ani = BOOM_BOAT_ANI_ALIVE;
	else if (state == BOOM_BOAT_STATE_EXPLODE)
		ani = BOOM_BOAT_ANI_EXPLODE;

	int alpha = 255;

	if (state == BOOM_BOAT_STATE_EXPLODE)
		animation_set->at(ani)->Render(x - 1, y - 5, alpha);
	else
		animation_set->at(ani)->Render(x, y, alpha);
	//RenderBoundingBox();
}

void CBomboat::SetState(int state)
{
	CGameObject::SetState(state);

	switch (state)
	{
	case BOOM_BOAT_STATE_FALLING:
		vy = BOOM_BOAT_BUM_GRAVITY;
		break;
	case BOOM_BOAT_STATE_EXPLODE:
		CBackup* backup = CBackup::GetInstance();
		backup->UpdateScore(backup->score + this->score);
		Sound::GetInstance()->Play("SOUND_Effect_14", 0, 1);
		vy = 0;
		break;
	}
}