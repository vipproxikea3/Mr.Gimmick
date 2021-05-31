#include "Gimmick.h"
#include <algorithm>
#include <assert.h>
#include "Utils.h"
#include "Game.h"
#include "GimmickDieEffect.h"
#include "PlayScene.h"

CGimmick::CGimmick() : CGameObject()
{
	untouchable = 0;
	stunning = false;
	SetState(GIMMICK_STATE_IDLE);
}

void CGimmick::CalculateSpeed(DWORD dt) {
	vx += ax * dt;

	/*if (abs(vx) > GIMMICK_WALKING_SPEED) {
		vx = nx * GIMMICK_WALKING_SPEED;
	}*/

	if ((vx * nx < 0) && this->state == GIMMICK_STATE_IDLE)
		vx = 0;
}

void CGimmick::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (this->state == GIMMICK_STATE_DIE)
		return;
	CalculateSpeed(dt);

	// Calculate dx, dy 
	CGameObject::Update(dt);

	// Simple fall down
	vy -= GIMMICK_GRAVITY * dt;

	onGround = false;

	onInclinedBrick = false;

	vector<LPGAMEOBJECT> newCoObjects;
	for (UINT i = 0; i < coObjects->size(); i++)
	{
		if (dynamic_cast<CBrick*>(coObjects->at(i))) newCoObjects.push_back(coObjects->at(i));
		else if (dynamic_cast<CConveyor*>(coObjects->at(i))) newCoObjects.push_back(coObjects->at(i));
		else if (dynamic_cast<CSwing*>(coObjects->at(i))) newCoObjects.push_back(coObjects->at(i));
		else if (dynamic_cast<CWorm*>(coObjects->at(i))) newCoObjects.push_back(coObjects->at(i));

		if (dynamic_cast<CInclinedBrick*>(coObjects->at(i))) {
			CInclinedBrick* brick = dynamic_cast<CInclinedBrick*>(coObjects->at(i));
			brick->Collision(this, dy);
		}
	}

	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	// turn off collision when die 
	if (state != GIMMICK_STATE_DIE)
		CalcPotentialCollisions(&newCoObjects, coEvents);

	// reset untouchable timer if untouchable time has passed
	if (GetTickCount64() - untouchable_start > GIMMICK_UNTOUCHABLE_TIME)
	{
		untouchable_start = 0;
		untouchable = 0;
	}

	// reset stunnung timer if stunnung time has passed
	if (GetTickCount64() - stunning_start > GIMMICK_STUNNING_TIME && stunning == true)
	{
		untouchable_start = 0;
		stunning = false;
		this->SetState(GIMMICK_STATE_IDLE);
	}
	// No collision occured, proceed normally
	if (coEvents.size() == 0)
	{
		x += dx;
		if (!onInclinedBrick)
			y += dy;
	}
	else
	{
		float min_tx, min_ty, nx = 0, ny;
		float rdx = 0;
		float rdy = 0;

		float x0 = x;
		float y0 = y;

		// TODO: This is a very ugly designed function!!!!
		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);

		// how to push back Mario if collides with a moving objects, what if Mario is pushed this way into another object?
		//if (rdx != 0 && rdx!=dx)
		//	x += nx*abs(rdx);


		//
		// Collision logic with other objects
		//
		for (UINT i = 0; i < coEventsResult.size(); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];

			if (dynamic_cast<CBrick*>(e->obj)) {
				x = x0 + min_tx * dx + nx * 0.1f;
				if (onInclinedBrick) x = x0 + dx;
				y = y0 + min_ty * dy + ny * 0.1f;

				if (e->nx != 0) vx = 0;
				if (e->ny != 0) vy = 0;

				if (e->ny == 1)
					this->onGround = true;
			}

			if (dynamic_cast<CConveyor*>(e->obj)) {
				x = x0 + min_tx * dx + nx * 0.1f;
				y = y0 + min_ty * dy + ny * 0.1f;

				if (e->nx != 0) vx = 0;
				if (e->ny != 0) vy = 0;

				if (e->ny == 1)
					this->onGround = true;
			}

			if (dynamic_cast<CWorm*>(e->obj)) {
				x = x0 + min_tx * dx + nx * 0.1f;
				y = y0 + min_ty * dy + ny * 0.1f;

				CWorm* worm = dynamic_cast<CWorm*>(e->obj);

				if (e->nx != 0) {
					if (worm->x < this->x)
					{
						this->vx = GIMMICK_DEFLECT_SPEED_X;
						this->nx = 1.0;
					}
					else
					{
						this->vx = -GIMMICK_DEFLECT_SPEED_X;
						this->nx = -1.0;
					}						
					this->SetState(GIMMICK_STATE_STUN);
				}
				if (e->ny != 0) {
					vy = 0;
				}
				if (e->ny == 1)
					this->onGround = true;
			}
			if (dynamic_cast<CSwing*>(e->obj)) {
				CSwing* swing = dynamic_cast<CSwing*>(e->obj);

				if (swing->GetState() == SWING_STATE_OPEN || swing->GetState() == SWING_STATE_STOP)
				{
					x += dx;
					y += dy;
				}
				else
				{
					if (e->ny > 0)
					{
						if (state == GIMMICK_STATE_WALKING_RIGHT || state == GIMMICK_STATE_WALKING_LEFT)
							x = x0 + min_tx * dx + nx * 0.1f;
						else
							x = x0 + swing->dx * 2;
						vy = 0;
						if (x + GIMMICK_BBOX_WIDTH / 2 >= swing->x && x + GIMMICK_BBOX_WIDTH / 2 <= swing->x + SWING_BBOX_WIDTH)
						{
							if (swing->GetState() == SWING_STATE_STAND)
								swing->SetState(SWING_STATE_MOVE);
						}
					}

					y = y0 + min_ty * dy + ny * 0.1f;
				}
			}
		}
	}

	// clean up newCoObjects
	for (UINT i = 0; i < newCoObjects.size(); i++) newCoObjects[i] = nullptr;
	// clean up collision events
	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
}

void CGimmick::Render()
{
	if (this->state == GIMMICK_STATE_DIE)
		return;
	int ani = -1;
	if (this->stunning == true) {
		if (nx > 0)
			ani = GIMMICK_ANI_STUN_RIGHT;
		else
			ani = GIMMICK_ANI_STUN_LEFT;

		animation_set->at(ani)->Render(x - 3.0f, y + 9.0f, 255);
	}
	else {
		if (vy > 0) {
			if (nx > 0)
				ani = GIMMICK_ANI_JUMP_RIGHT;
			else
				ani = GIMMICK_ANI_JUMP_LEFT;
		}
		else {
			if (vx == 0)
			{
				if (nx > 0) ani = GIMMICK_ANI_IDLE_RIGHT;
				else ani = GIMMICK_ANI_IDLE_LEFT;
			}
			else if (vx > 0)
				ani = GIMMICK_ANI_WALKING_RIGHT;
			else ani = GIMMICK_ANI_WALKING_LEFT;
		}

		int alpha = 255;
		if (untouchable) alpha = 128;

		animation_set->at(ani)->Render(x, y + 3.0f, alpha);
	}
	//RenderBoundingBox();
}

void CGimmick::CreateDieEffect() {
	CAnimationSets* animation_sets = CAnimationSets::GetInstance();
	LPANIMATION_SET ani_set = animation_sets->Get(GIMMICKDIEEFFECT_ANI_SET);
	for (int i = 0; i < 16; i++) {
		CGimmickDieEffect* effect = new CGimmickDieEffect(i);
		effect->SetPosition(x, y);
		effect->SetAnimationSet(ani_set);
		((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->PushBackObj(effect);
	}
}

void CGimmick::SetState(int state)
{
	CGameObject::SetState(state);

	switch (state)
	{
	case GIMMICK_STATE_WALKING_RIGHT:
		ax = GIMMICK_ACCELERATION;
		if (vx > GIMMICK_WALKING_SPEED)
			ax = 0;
		nx = 1;
		break;
		break;
	case GIMMICK_STATE_WALKING_LEFT:
		ax = -GIMMICK_ACCELERATION;
		if (vx < -GIMMICK_WALKING_SPEED)
			ax = 0;
		nx = -1;
		break;
	case GIMMICK_STATE_JUMP:
		vy = GIMMICK_JUMP_SPEED_Y;
		break;
	case GIMMICK_STATE_IDLE:
		if (vx > 0)
			ax = -GIMMICK_WALKING_FRICTION;
		else if (vx < 0)
			ax = GIMMICK_WALKING_FRICTION;
		else ax = 0;
		break;
	case GIMMICK_STATE_STUN:
		stunning = true;
		stunning_start = GetTickCount64();
		this->SetState(GIMMICK_STATE_IDLE);
		break;
	case GIMMICK_STATE_DIE:
		CreateDieEffect();
		vx = 0;
		vy = 0;
		ax = 0;
		break;
	}
}

void CGimmick::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x + 0.5f;
	top = y - 0.5f;
	right = left + GIMMICK_BBOX_WIDTH - 0.5f;
	bottom = top - GIMMICK_BBOX_HEIGHT + 0.5f;
}

