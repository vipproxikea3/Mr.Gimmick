#include "Gimmick.h"
#include <algorithm>
#include <assert.h>
#include "Utils.h"
#include "Game.h"
#include "GimmickDieEffect.h"
#include "PlayScene.h"
#include "Brick.h"

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

	if ((vx * nx < 0) && this->state == GIMMICK_STATE_IDLE && inSewer == false)
		vx = 0;

	//JUMP:
	if (vy > GIMMICK_JUMP_SPEED_Y_MAX)
	{
		ay = -GIMMICK_GRAVITY;
		falling = true;
		jumping = false;
	}
}

void CGimmick::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	DebugOut(L"\nVao day %d", vy);

	if (this->state == GIMMICK_STATE_DIE)
		return;
	// Set in sewer
	/*if (onGround)
	{
		if (nx * vx < 0 && inSewer == true)
			inSewer = true; 
		else 
			inSewer = false;
	}// set if onground not inSewer
	if (equalinSewer == true)
	{
		inSewer = true;
	}//if in sewer3 insewer*/
	//equalinSewer = false; // check in sewer 3
	if (inSewer == true)
	{
		if (YSewer != 0)
		{
			if (nSewer == 1 && (y>YSewer+ 0.25))
			{
				inSewer = false;
			}
			if (nSewer == -1 && (y < YSewer- 0.25))
			{
				inSewer = false;
			}
		}
		if (XSewer != 0)
		{
			if (nSewer == 1 && (x > XSewer+ 0.25))
			{
				inSewer = false;
			}
			if (nSewer == -1 && (x < XSewer-0.25))
			{
				inSewer = false;
			}
		}
	}
	CalculateSpeed(dt);

	// Calculate dx, dy 
	CGameObject::Update(dt);

	// Simple fall down
	
	if(inSewer==false) //|| equalinSewer == true
		vy += ay * dt;
	ay = -GIMMICK_GRAVITY;

	onInclinedBrick = false;
	onGround = false;
	onEnemy = false;

	vector<LPGAMEOBJECT> newCoObjects;
	for (UINT i = 0; i < coObjects->size(); i++)
	{
		if (dynamic_cast<CBrick*>(coObjects->at(i))) newCoObjects.push_back(coObjects->at(i));
		else if (dynamic_cast<CConveyor*>(coObjects->at(i))) newCoObjects.push_back(coObjects->at(i));
		else if (dynamic_cast<CSwing*>(coObjects->at(i))) newCoObjects.push_back(coObjects->at(i));
		else if (dynamic_cast<CMedicine*>(coObjects->at(i))) newCoObjects.push_back(coObjects->at(i));
		else if (dynamic_cast<CWorm*>(coObjects->at(i))) newCoObjects.push_back(coObjects->at(i));
		else if (dynamic_cast<CBrickPink*>(coObjects->at(i))) newCoObjects.push_back(coObjects->at(i));
		else if (dynamic_cast<CBlackEnemy*>(coObjects->at(i))) newCoObjects.push_back(coObjects->at(i));

		if (dynamic_cast<CSewer*>(coObjects->at(i))) newCoObjects.push_back(coObjects->at(i));
		if (dynamic_cast<CInclinedBrick*>(coObjects->at(i))) {
			CInclinedBrick* brick = dynamic_cast<CInclinedBrick*>(coObjects->at(i));
			brick->Collision(this, dy);
		}
		if (dynamic_cast<CConveyor*>(coObjects->at(i))) {
			CConveyor* conveyor = dynamic_cast<CConveyor*>(coObjects->at(i));
			if (onTopOf(conveyor)) { this->onGround = true;}
		}
		if (dynamic_cast<CBrick*>(coObjects->at(i))) {
			CBrick* brick = dynamic_cast<CBrick*>(coObjects->at(i));
			if (onTopOf(brick)) this->onGround = true;
		}
		if (dynamic_cast<CBlackEnemy*>(coObjects->at(i))) {
			CBlackEnemy* enemy = dynamic_cast<CBlackEnemy*>(coObjects->at(i));
			if (onTopOf(enemy, 7) && enemy->state == BLACKENEMY_STATE_WALK) { 
				this->onGround = true;
				standOn(enemy);
			}
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
		float min_tx, min_ty, nx, ny;
		float rdx = 0;
		float rdy = 0;

		float x0 = x;
		float y0 = y;

		float tempy = vy;
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
			if (e->ny > 0) this->falling = false;
			if (e->ny < 0) this->falling = true; //roi khi dung gach tren dau

			if (dynamic_cast<CBrick*>(e->obj)) {
				x = x0 + min_tx * dx + nx * 0.1f;
				if (onInclinedBrick) x = x0 + dx;
				y = y0 + min_ty * dy + ny * 0.1f;
				if (e->nx != 0&&inSewer==false) { vx = 0; }
				if (e->ny != 0 ) {
					vy = 0;
					if (e->ny > 0) this->onGround = true;
					if (e->ny < 0) onEnemy = false;
				}

			}

			if (dynamic_cast<CBrickPink*>(e->obj)) {
				CBrickPink* BrickPink = dynamic_cast<CBrickPink*>(e->obj);

				x = x0 + min_tx * dx + nx * 0.1f;
				y = y0 + min_ty * dy + ny * 0.1f;

				if (e->ny < 0) vy = 0;
				if (e->ny > 0)
				{
					vy = BrickPink->vy;
					if (state == GIMMICK_STATE_WALKING_RIGHT || state == GIMMICK_STATE_WALKING_LEFT)
						x = x0 + min_tx * (dx+BrickPink->dx) + BrickPink->nx * 0.01f;
					else
						x = x0 + BrickPink->dx * 2 + BrickPink->nx * 0.01f;
					y = y0 + min_ty * dy + ny * 0.1f;
					this->onGround = true;
					vy = 0;
				}
				if (e->nx != 0) {
					vx = 0;
				}
			}

			if (dynamic_cast<CConveyor*>(e->obj)) {
				y = y0 + min_ty * dy + ny * 0.1f;
				CConveyor* conveyor = dynamic_cast<CConveyor*>(e->obj);
				if (e->ny > 0)
				{
					vy = 0;
					if (conveyor->GetDirection() == RIGHT_CONVEYOR) //right conveyor
					{
						if (state == GIMMICK_STATE_WALKING_RIGHT)
						{
							x = x0 + min_tx * dx + 1.5f;
							//DebugOut(L"\nGimmick is touching right y");
						}
						if (state == GIMMICK_STATE_WALKING_LEFT)
						{
							//DebugOut(L"\nGimmick is touching left y");
							x = x0 + min_tx * dx + 2.0f;
						}
						if (state == GIMMICK_STATE_IDLE)
						{
							x = x0 + 2.0f;
						}
					}
					if (conveyor->GetDirection() == LEFT_CONVEYOR) //left conveyor
					{
						if (state == GIMMICK_STATE_WALKING_RIGHT)
						{
							x = x0 + min_tx * dx - 2.0f;
							//DebugOut(L"\nGimmick is touching left y");
						}
						if (state == GIMMICK_STATE_WALKING_LEFT)
						{
							x = x0 + min_tx * dx - 1.5f;
						}
						if (state == GIMMICK_STATE_IDLE)
						{
							x = x0 - 2.0f;
						}
					}
				}
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

			if (dynamic_cast<CMedicine*>(e->obj))
			{
				CMedicine* medicine = dynamic_cast<CMedicine*>(e->obj);
				if (e->nx != 0)
				{
					medicine->SetState(MEDICINE_STATE_DISAPPEAR);
				}
				if (e->ny != 0)
				{
					medicine->SetState(MEDICINE_STATE_DISAPPEAR);
				}
			}

			if (dynamic_cast<CBlackEnemy*>(e->obj)) {

				CBlackEnemy* enemy = dynamic_cast<CBlackEnemy*>(e->obj);
				float l, t, r, b;
				GetBoundingBox(l, t, r, b);
				float ol, ot, or , ob;
				e->obj->GetBoundingBox(ol, ot, or , ob);

				if (e->ny > 0 && enemy->state == BLACKENEMY_STATE_WALK) {
					vy = 0;
					this->y = y0 + min_ty * dy + ny * 0.3f;
					standOn(enemy); 
				}
				else 
				{
					if (enemy->x < this->x)
					{
						//this->vx = GIMMICK_DEFLECT_SPEED_X;
						this->nx = 1.0;
					}
					else
					{
						//this->vx = -GIMMICK_DEFLECT_SPEED_X;
						this->nx = -1.0;
					}
					this->SetState(GIMMICK_STATE_STUN);
					StartUntouchable();

				}
			}

			if (dynamic_cast<CSewer*>(e->obj)) {
				CSewer* Sewer = dynamic_cast<CSewer*>(e->obj);
				if (e->nx != 0 || e->ny != 0)
				{
					equalinSewer = false;
					inSewer = true;
				}
				if (e->nx < 0 && Sewer->type == 1)
				{
					XSewer = Sewer->GetX()+16;
					YSewer = 0;
					nSewer = 1; 
					y = Sewer->GetY() + 0.01;
					vx = 0.2f;
					vy = 0.0f;
				}
				else if (e->nx > 0 && Sewer->type == 1)
				{
					XSewer = Sewer->GetX() - 16;
					YSewer = 0;
					nSewer = -1;
					y = Sewer->GetY() + 0.01;
					vx = -0.13f;
					vy = 0.0f;
				}
				if (e->ny < 0 && Sewer->type == 2) {
					XSewer = 0;
					YSewer = Sewer->GetY()+ 16;
					nSewer = 1;
					x = Sewer->GetX()-0.01;
					vy = 0.2f;
					vx = 0.0f;
				}
				else if (e->ny > 0 && Sewer->type == 2)
				{
					XSewer = 0;
					YSewer = Sewer->GetY() - 16;
					nSewer = -1;
					x = Sewer->GetX();
					vy = -0.2f;
					vx = 0.0f;
				}
				if (e->nx < 0 && Sewer->type == 3)
				{
					XSewer = 0;
					YSewer = Sewer->GetY() - 16;
					nSewer = -1;
					equalinSewer = true;
					vx = 0.1f;
					vy = -0.2f;
				}
				else if (e->ny < 0 && Sewer->type == 3)
				{
					XSewer = Sewer->GetX() - 16;
					YSewer = 0;
					nSewer = -1;
					equalinSewer = true;
					x = Sewer->GetX()+1;
					vx = -0.15f;
					vy = 0.1f;
				}
				if (e->nx < 0 && Sewer->type == 6)
				{
					XSewer = 0;
					YSewer = Sewer->GetY() + 16;
					nSewer = 1;
					equalinSewer = true;
					vx = 0.1f;
					vy = 0.3f;
				}
				else if (e->ny > 0 && Sewer->type == 6)
				{
					XSewer = Sewer->GetX() - 16;
					YSewer = 0;
					nSewer = -1;
					equalinSewer = true;
					x = Sewer->GetX() - 0.01;
					vx = -0.2f;
					vy = -0.2f;
				}
				if (e->nx > 0 && Sewer->type == 4)
				{
					XSewer = 0;
					YSewer = Sewer->GetY() - 16;
					nSewer = -1;
					equalinSewer = true;
					vx = -0.1f;
					vy = -0.2f;
				}
				else if (e->ny < 0 && Sewer->type == 4)
				{
					XSewer = Sewer->GetX() + 16;
					YSewer = 0;
					nSewer = 1;
					equalinSewer = true;
					x = Sewer->GetX() - 0.01;
					vx = 0.3f;
					vy = 0.2f;
				}
				x = x + min_tx * dx;
				y = y + min_ty * dy;
			}
		}
		if (equalinSewer && tempy != 0)
			vy = tempy;
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
	if (this->inSewer)
	{
		if (nx > 0)
			ani = GIMMICK_ANI_IN_SEWER_RIGHT;
		else
			ani = GIMMICK_ANI_IN_SEWER_LEFT;
		animation_set->at(ani)->Render(x , y - 1.0f , 255);
	}
	else if (this->stunning == true) 
	{
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
			{ // IDLE AND FALL
				if (nx > 0) {
					if (this->onGround) ani = GIMMICK_ANI_IDLE_RIGHT;
					else ani = GIMMICK_ANI_JUMP_RIGHT;
				} 
				else {
					if (this->onGround) ani = GIMMICK_ANI_IDLE_LEFT;
					else ani = GIMMICK_ANI_JUMP_LEFT;
				}
			}
			else if (vx > 0)
			{ //WALK RIGHT
				if (this->onGround) ani = GIMMICK_ANI_WALKING_RIGHT;
				else ani = GIMMICK_ANI_JUMP_RIGHT;
			}
			else
			{ //WALK LEFT
				if (this->onGround) ani = GIMMICK_ANI_WALKING_LEFT;
				else ani = GIMMICK_ANI_JUMP_LEFT;
			}
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
	case GIMMICK_STATE_IN_SEWER:
		break;
	case GIMMICK_STATE_WALKING_RIGHT:
		ax = GIMMICK_ACCELERATION;
		if (vx > GIMMICK_WALKING_SPEED)
			ax = 0;
		nx = 1;
		break;
	case GIMMICK_STATE_WALKING_LEFT:
		ax = -GIMMICK_ACCELERATION;
		if (vx < -GIMMICK_WALKING_SPEED)
			ax = 0;
		nx = -1;
		break;
	case GIMMICK_STATE_JUMP:
		jumping = true;
		if (vy == 0 || this->onInclinedBrick || this->onGround) 
			vy = GIMMICK_JUMP_SPEED_Y_MIN;
		ay = GIMMICK_JUMP_ACCELERATION;
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

bool CGimmick::onTopOf(CGameObject* object, float equal)
{
	float ol, ot, or , ob;
	object->GetBoundingBox(ol, ot, or , ob);
	float l, t, r, b;
	GetBoundingBox(l, t, r, b);
	if (dynamic_cast<CBlackEnemy*>(object))
	{
		l = l + 2;
		r = r - 2;
	}
	if (r >= ol && l <= or && abs(b - ot) < equal)
		return true;
	return false;
}

void CGimmick::standOn(CGameObject* object)
{
	onEnemy = true;
	
	if (dynamic_cast<CBlackEnemy*>(object))
	{
		((CBlackEnemy*)object)->carry_player = true;
		this->x += object->dx;
		if (!jumping) { 
			this->y = object->y + GIMMICK_BBOX_HEIGHT;
			this->vy = 0; 
		}
	}
	
}

