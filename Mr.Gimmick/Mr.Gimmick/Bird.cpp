#include "Bird.h"
#include "Gimmick.h"
#include "PlayScene.h"
#include "Utils.h"
#include "algorithm"
#include "Medicine.h"
#include "WaterDie.h"

/*int CSewer::Collision(LPGAMEOBJECT object, float dy) {
	if (!this->CheckAABB(object))
		return 0;
	if (object->x + 8 < this->x || object->x + 8 > this->x + 16)
		return 0;
	if (dynamic_cast<CGimmick*>(object) && ((CGimmick*)object)->jumping)
		return 0;

	float lx = this->x;
	float ly = this->y - this->ly;
	float rx = this->x + INCLINEDBRICK_BBOX_WIDTH;
	float ry = this->y - this->ry;

	float ox = object->x + 8.0f;

	float oy = (((ox - lx) * (ry - ly)) / (rx - lx)) + ly;

	if (object->y - 16 + dy < oy + 2.0f) {
		if (((ly > ry && lx + 8.0f > ox) && isT == -1) || ((ly < ry && rx - 8.0f < ox) && isT == 1))
			object->y = oy + 18.0f;
		else
			object->y = oy + 16.0f;
		/*if(object->vy < 0)*/ /*object->vy = 0.0f;
		object->onInclinedBrick = true;

		if (dynamic_cast<CGimmick*>(object)) {
			CGimmick* gimmick = dynamic_cast<CGimmick*>(object);
			gimmick->onGround = true;
			gimmick->falling = false;
		}
	}
	return this->direction;
}*/

void CBird::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {
	CGameObject::Update(dt);
	if (state == BIRD_STATE_HIDE)
		return;
	// Simple fall down
	if(state!=BIRD_STATE_WATER_DIE)
		vy += ay * dt;
	ay = -BIRD_GRAVITY;

	vector<LPGAMEOBJECT> newCoObjects;
	for (UINT i = 0; i < coObjects->size(); i++)
	{
		if (dynamic_cast<CBrick*>(coObjects->at(i))) newCoObjects.push_back(coObjects->at(i));
		if (dynamic_cast<CWaterDie*>(coObjects->at(i))) newCoObjects.push_back(coObjects->at(i));
	}

	float x0 = x;
	float y0 = y;

	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();
	if(state!=BIRD_STATE_DIE && state!=BIRD_STATE_WATER_DIE)
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
				y = y0 + min_ty * dy + ny * 0.1f;
				x = x0 + min_tx * dx + nx * 0.1f;

				if (e->ny != 0) {
					vy = 0;
					vx = 0;
					if (laststate == BIRD_STATE_FLY)
						SetState(BIRD_STATE_IDLE);
				}
			}
			if (dynamic_cast<CWaterDie*>(e->obj))
			{
				if (e->ny != 0)
				{
					see_jump_start = 0;
					nodnod_start = 0;
					jump_start = 0;
					jump = 0;
					SetState(BIRD_STATE_WATER_DIE);
				}
			}
		}
	}
	CGimmick* gimmick = ((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->GetPlayer();

	if (this->state != BIRD_STATE_FLY && this->state != BIRD_STATE_DIE && this->state != BIRD_STATE_WATER_DIE )
	{
		if (x - gimmick->x <= BIRD_LIMIT_FLY_X + GIMMICK_BBOX_WIDTH && x - gimmick->x >0 &&gimmick->y - y <= BIRD_LIMIT_FLY_Y)
		{
			nx = 1;
			see_jump_start = 0;
			nodnod_start = 0;
			jump_start = 0;
			jump = 0;
			SetState(BIRD_STATE_FLY);
		}
		else if (gimmick->x - x <= BIRD_LIMIT_FLY_X + BIRD_BBOX_WIDTH && gimmick->x - x >0 && gimmick->y - y <= BIRD_LIMIT_FLY_Y)
		{
			nx = -1;
			see_jump_start = 0;
			nodnod_start = 0;
			jump_start = 0;
			jump = 0;
			SetState(BIRD_STATE_FLY);
		}
	}

	if (see_jump_start != 0 && GetTickCount64() - see_jump_start >= BIRD_TIME_SEE_JUMP)
	{
		see_jump_start = 0;
		SetState(BIRD_STATE_NODNOD);
	}
	if (nodnod_start != 0 && GetTickCount64() - nodnod_start >= BIRD_TIME_NODNOD)
	{
		nodnod_start = 0;
		if (nx > 0)
			nx = -1;
		else
			nx = 1;
		SetState(BIRD_STATE_JUMP);
	}
	if (jump_start != 0 && GetTickCount64() - jump_start >= BIRD_TIME_JUMP)
	{
		if (jump < 2)
		{
			jump_start = 0;
			if (nx > 0)
				nx = -1;
			else
				nx = 1;
			SetState(BIRD_STATE_JUMP);
		}
		else
		{
			see_jump_start = jump_start;
			jump_start = 0;
			SetState(BIRD_STATE_IDLE);
		}
	}

	CStar* star = ((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->GetStar();
	if (this->CheckAABB(star) && (star->GetState() == STAR_STATE_WALKING_LEFT || star->GetState() == STAR_STATE_WALKING_RIGHT) && state != BIRD_STATE_DIE && state != BIRD_STATE_WATER_DIE)
	{
		see_jump_start = 0;
		nodnod_start = 0;
		jump_start = 0;
		jump = 0;
		SetState(BIRD_STATE_DIE);
		if (type == 1)
		{
			CreateReward();
		}
		((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->AttackBird();
	}
	if (die_start != 0 && GetTickCount64() - die_start >= BIRD_TIME_DIE)
	{
		die_start = 0;
		SetState(BIRD_STATE_HIDE);
	}
	if(die_water_start !=0 && GetTickCount64() - die_water_start >= BIRD_TIME_WATER_DIE)
	{
		die_water_start = 0;
		SetState(BIRD_STATE_HIDE);
	}
	// clean up newCoObjects
	for (UINT i = 0; i < newCoObjects.size(); i++) newCoObjects[i] = nullptr;
	// clean up collision events
	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
	laststate = state;
}

void CBird::Render()
{
	int ani = -1;
	if (state == BIRD_STATE_HIDE)
		return;
	if (state == BIRD_STATE_IDLE || state == BIRD_STATE_JUMP)
	{
		if (nx > 0)
			ani = BIRD_ANI_IDLE_RIGHT;
		else
			ani = BIRD_ANI_IDLE_LEFT;
	}
	else if (state == BIRD_STATE_NODNOD)
	{
		if (nx > 0)
			ani = BIRD_ANI_NODNOD_RIGHT;
		else
			ani = BIRD_ANI_NODNOD_LEFT;
	}
	else if (state == BIRD_STATE_FLY)
	{
		if (nx > 0)
			ani = BIRD_ANI_FLY_RIGHT;
		else
			ani = BIRD_ANI_FLY_LEFT;
	}
	else if (state == BIRD_STATE_DIE)
	{
		if (nx > 0)
			ani = BIRD_ANI_DIE_RIGHT;
		else
			ani = BIRD_ANI_DIE_LEFT;
	}
	else if (state == BIRD_STATE_WATER_DIE)
	{
		ani = BIRD_ANI_WATER_DIE;
	}
	int alpha = 255;
	animation_set->at(ani)->Render(x, y, alpha);
	//RenderBoundingBox();
}

void CBird::SetState(int state) {
	CGameObject::SetState(state);
	if (state == BIRD_STATE_HIDE)
		return;
	switch (state)
	{
	case BIRD_STATE_IDLE:
		StartSeeJump();
		jump = 0;
		vx = 0;
		vy = 0;
		break;
	case BIRD_STATE_JUMP:
		StartJump();
		jump++;
		if (nx > 0)
			vx = BIRD_VX_JUMP;
		else
			vx = -BIRD_VX_JUMP;
		vy = BIRD_VY_JUMP;
		break;
	case BIRD_STATE_NODNOD:
		StartNodNod();
		vx = 0;
		vy = 0;
		break;
	case BIRD_STATE_FLY:
		if(nx>0)
			vx = BIRD_VX_FLY;
		else
			vx = -BIRD_VX_FLY;
		vy = BIRD_VY_FLY;
		break;
	case BIRD_STATE_DIE:
		StartDie();
		vy = BIRD_VY_JUMP;
		break;
	case BIRD_STATE_WATER_DIE:
		StartDieWater();
		vy = 0;
		vx = 0;
		break;
	}
}

void CBird::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = x + BIRD_BBOX_WIDTH;
	b = y - BIRD_BBOX_HEIGHT;
}
void CBird::CreateReward()
{
	CMedicine* reward = new CMedicine(MEDICINE_ANI_PINK);
	//Vi tri cho quai
	reward->SetPosition(x,y - BIRD_BBOX_HEIGHT+ MEDICINE_PINK_BBOX_HEIGHT+2);

	//Animation set cho quai
	CAnimationSets* animation_sets = CAnimationSets::GetInstance();
	LPANIMATION_SET ani_set = animation_sets->Get(MEDICINE_ANISET_ID);
	reward->SetAnimationSet(ani_set);

	//them vao list object cua playscene
	CScene* scene = CGame::GetInstance()->GetCurrentScene();
	((CPlayScene*)scene)->PushBackObj(reward);
}