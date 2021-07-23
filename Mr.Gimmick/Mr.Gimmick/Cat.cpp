#include "Cat.h"
#include "Bird.h"
#include "Gimmick.h"
#include "PlayScene.h"
#include "Utils.h"
#include "algorithm"
#include "Medicine.h"
#include "WaterDie.h"
#include "SpecialBrick.h"

void CCat::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {
	if ((this->state == CAT_STATE_RUN || this->state == CAT_STATE_CHASE) && !CGame::GetInstance()->InCamera(this) && CGame::GetInstance()->InLargeCamera(this) && this->visible == true) {
		this->visible = false;
	}
	if (!visible)
		return;
	CGameObject::Update(dt);
	if (state == CAT_STATE_HIDE)
		return;

	CGimmick* gimmick = ((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
	if (state != CAT_STATE_SLEEP && state != CAT_STATE_WATER_DIE && state != CAT_STATE_HIDE)
	{
		if (this->CheckAABB(gimmick)) {
			if (gimmick->untouchable == 0)
				gimmick->SetState(GIMMICK_STATE_STUN);
		}
	}

	CStar* star = ((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->GetStar();
	if (this->CheckAABB(star) && (star->GetState() == STAR_STATE_WALKING_LEFT || star->GetState() == STAR_STATE_WALKING_RIGHT) && state != CAT_STATE_WATER_DIE && state != CAT_STATE_HIDE)
	{
		sleep_start = 0;
		run_start = 0;
		if (gimmick->x < x)
			nx = 1;
		else
			nx = -1;
		SetState(CAT_STATE_RUN);
		star->SetState(STAR_STATE_EXPLOSIVE);
	}

	if (sleep_start != 0 && GetTickCount64() - sleep_start >= CAT_TIME_SLEEP)
	{
		sleep_start = 0;
		SetState(CAT_STATE_SLEEP);
	}

	if (state == CAT_STATE_IDLE && run_start == 0 && ((gimmick->x - x <= CAT_LIMIT_X + CAT_BBOX_WIDTH && gimmick->x - x >= 0) || (x - gimmick->x <= CAT_LIMIT_X + GIMMICK_BBOX_WIDTH && x - gimmick->x >= 0)) && ((gimmick->y - y <= CAT_LIMIT_Y + GIMMICK_BBOX_HEIGHT && gimmick->y - y >= 0) || (y - gimmick->y <= CAT_LIMIT_Y + CAT_BBOX_HEIGHT && y - gimmick->y >= 0)))
	{
		sleep_start = 0;
		StartRun();
	}
	if (run_start != 0 && GetTickCount64() - run_start >= CAT_TIME_RUN)
	{
		run_start = 0;
		if (gimmick->x > x)
			nx = 1;
		else
			nx = -1;
		SetState(CAT_STATE_CHASE);
	}
	if (state == CAT_STATE_CHASE)
	{
		if (x <= gimmick->x - CAT_LIMIT_RUN_X - CAT_BBOX_WIDTH)
		{
			stop_star = 1;
			nx = 1;
			SetState(CAT_STATE_CHASE);
		}
		else if (x >= gimmick->x + CAT_LIMIT_RUN_X + CAT_BBOX_WIDTH)
		{
			stop_star = 1;
			nx = -1;
			SetState(CAT_STATE_CHASE);
		}
		else
			SetState(CAT_STATE_CHASE);
	}

	if (stop_star == 1 && star->state == STAR_STATE_READY && ((gimmick->x - x <= CAT_LIMIT_X + CAT_BBOX_WIDTH && gimmick->x - x >= 0) || (x - gimmick->x <= CAT_LIMIT_X && x - gimmick->x >= 0)) && ((gimmick->y - y <= CAT_LIMIT_Y && gimmick->y - y >= 0) || (y - gimmick->y <= CAT_LIMIT_Y + CAT_BBOX_HEIGHT && y - gimmick->y >= 0)))
	{
		sleep_start = 0;
		stop_star = 0;
		run_start = 0;
		SetState(CAT_STATE_IDLE);
	}

	if (water_die != 0 && GetTickCount64() - water_die >= CAT_TIME_HIDE)
	{
		water_die = 0;
		SetState(CAT_STATE_HIDE);
	}

	if (lastvx * vx < 0 && state == CAT_STATE_CHASE)
	{
		if (lastvx < 0)
		{
			lastvx += CAT_RUN_SLIDE;
			vx = lastvx;
		}
		else if (lastvx > 0)
		{
			lastvx -= CAT_RUN_SLIDE;
			vx = lastvx ;
		}
	}

	// Simple fall down
	if (state != CAT_STATE_WATER_DIE)
		vy += ay * dt;
	ay = -CAT_GRAVITY;

	vector<LPGAMEOBJECT> newCoObjects;
	for (UINT i = 0; i < coObjects->size(); i++)
	{
		if (dynamic_cast<CBrick*>(coObjects->at(i))) newCoObjects.push_back(coObjects->at(i));
		if (dynamic_cast<CSpecialBrick*>(coObjects->at(i))) newCoObjects.push_back(coObjects->at(i));
		if (dynamic_cast<CWaterDie*>(coObjects->at(i))) newCoObjects.push_back(coObjects->at(i));
	}

	float x0 = x;
	float y0 = y;

	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();
	if (state != CAT_STATE_WATER_DIE)
		CalcPotentialCollisions(&newCoObjects, coEvents);

	if (coEvents.size() == 0)
	{
		x = x0 + dx;
		y = y0 + dy;
	}
	else
	{
		float min_tx, min_ty, nx = 0, ny = 0;
		float rdx = 0;
		float rdy = 0;

		x = x0 + dx;
		y = y0 + dy;

		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);

		for (UINT i = 0; i < coEventsResult.size(); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];

			if (dynamic_cast<CBrick*>(e->obj))
			{
				if (e->ny != 0) {
					y = y0 + min_ty * dy + ny * 0.1f;
					vy = 0;
				}
				if (e->nx != 0) {
					x = x0 + min_tx * dx + nx * 0.1f;
					sleep_start = 0;
					stop_star = 0;
					run_start = 0;
					SetState(CAT_STATE_IDLE);
					//vy = 0;
				}
			}
			if (dynamic_cast<CSpecialBrick*>(e->obj))
			{
				y = y0 + min_ty * dy + ny * 0.1f;
				//x = x0 + min_tx * dx + nx * 0.1f;

				if (e->ny != 0) {
					vy = 0;
				}
			}
			if (dynamic_cast<CWaterDie*>(e->obj))
			{
				SetState(CAT_STATE_WATER_DIE);
			}
		}
	}
	// clean up newCoObjects
	for (UINT i = 0; i < newCoObjects.size(); i++) newCoObjects[i] = nullptr;
	// clean up collision events
	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];

	if(lastvx*vx >=0)
		lastvx = vx;
}

void CCat::Render()
{
	int ani = -1;
	if (state == CAT_STATE_HIDE || !visible)
		return;
	if (state == CAT_STATE_IDLE)
	{
		if (nx > 0)
			ani = CAT_ANI_IDLE_RIGHT;
		else
			ani = CAT_ANI_IDLE_LEFT;
	}
	else if (state == CAT_STATE_SLEEP)
	{
		if (nx > 0)
			ani = CAT_ANI_SLEEP_RIGHT;
		else
			ani = CAT_ANI_SLEEP_LEFT;
	}
	else if (state == CAT_STATE_RUN || state == CAT_STATE_CHASE)
	{
		if (nx > 0)
			ani = CAT_ANI_RUN_RIGHT;
		else
			ani = CAT_ANI_RUN_LEFT;
	}
	else if (state == CAT_STATE_WATER_DIE)
	{
		ani = CAT_ANI_WATER_DIE;
	}
	
	int alpha = 255;
	animation_set->at(ani)->Render(x, y, alpha);
	//RenderBoundingBox();
}

void CCat::SetState(int state) {
	CGameObject::SetState(state);
	if (state == CAT_STATE_HIDE)
		return;
	switch (state)
	{
	case CAT_STATE_IDLE:
		StartSleep();
		vx = 0;
		//vy = 0;
		break;
	case CAT_STATE_SLEEP:
		vx = 0;
		vy = 0;
		break;
	case CAT_STATE_RUN:
		if(nx>0)
			vx = CAT_RUN_SPEED;
		else
			vx = -CAT_RUN_SPEED;
		//vy = 0;
		break;
	case CAT_STATE_CHASE:
		if (nx > 0)
			vx = CAT_RUN_SPEED;
		else
			vx = -CAT_RUN_SPEED;
		//vy = 0;
		break;
	case CAT_STATE_WATER_DIE:
		StartWaterDie();
		sleep_start = 0;
		stop_star = 0;
		run_start = 0;
		vx = 0;
		vy = 0;
	}
}

void CCat::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = x + CAT_BBOX_WIDTH;
	b = y - CAT_BBOX_HEIGHT;
}
