#include "Gimmick.h"
#include <algorithm>
#include <assert.h>
#include "Utils.h"
#include "Game.h"
#include "GimmickDieEffect.h"
#include "PlayScene.h"
#include "Brick.h"
#include "BlackBird.h"
#include "Backup.h"

CGimmick::CGimmick() : CGameObject()
{
	untouchable = 0;
	stunning = false;
	SetState(GIMMICK_STATE_IDLE);
}

void CGimmick::CalculateSpeed(DWORD dt) {
	if (this->state == GIMMICK_STATE_IDLE) {
		if (vx > 0 && vx + ax * dt < 0) {
			vx = 0;
			ax = 0;
		}
		else if (vx < 0 && vx + ax * dt > 0)
		{
			vx = 0;
			ax = 0;
		}
		else {
			vx += ax * dt;
		}
	}
	else {
		vx += ax * dt;
	}

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
	if (this->state == GIMMICK_STATE_DIE && GetTickCount64() - die_start >= GIMMICK_REVIVAL_TIME) {
		this->Revival();
	}

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
	onStar = false;
	facingBrick = false;
	underBrick = false;
	// hướng của gạch nghiêng
	onBoat = false;
	int direction = 0;

	DetectStar();

	vector<LPGAMEOBJECT> newCoObjects;
	for (UINT i = 0; i < coObjects->size(); i++)
	{
		if (coObjects->at(i)->visible == false)
			continue;
		if (dynamic_cast<CBrick*>(coObjects->at(i))) newCoObjects.push_back(coObjects->at(i));
		else if (dynamic_cast<CSpecialBrick*>(coObjects->at(i))) newCoObjects.push_back(coObjects->at(i));
		else if (dynamic_cast<CConveyor*>(coObjects->at(i))) newCoObjects.push_back(coObjects->at(i));
		else if (dynamic_cast<CSwing*>(coObjects->at(i))) newCoObjects.push_back(coObjects->at(i));
		else if (dynamic_cast<CMedicine*>(coObjects->at(i))) newCoObjects.push_back(coObjects->at(i));
		else if (dynamic_cast<CWorm*>(coObjects->at(i))) newCoObjects.push_back(coObjects->at(i));
		else if (dynamic_cast<CBrickPink*>(coObjects->at(i))) newCoObjects.push_back(coObjects->at(i));
		else if (dynamic_cast<CBlackEnemy*>(coObjects->at(i))) newCoObjects.push_back(coObjects->at(i));
		else if (dynamic_cast<CElectricBlackEnemy*>(coObjects->at(i))) newCoObjects.push_back(coObjects->at(i));
		else if (dynamic_cast<CGun*>(coObjects->at(i))) newCoObjects.push_back(coObjects->at(i));
		else if (dynamic_cast<CBullet*>(coObjects->at(i))) newCoObjects.push_back(coObjects->at(i));
		else if (dynamic_cast<CStandBlackEnemy*>(coObjects->at(i))) newCoObjects.push_back(coObjects->at(i));
		else if (dynamic_cast<CCloudEnemy*>(coObjects->at(i))) newCoObjects.push_back(coObjects->at(i));
		else if (dynamic_cast<CBoat*>(coObjects->at(i))) {
			CBoat* Boat = dynamic_cast<CBoat*>(coObjects->at(i));
			if (onTopOf(Boat)) { this->onBoat = true; }
			newCoObjects.push_back(coObjects->at(i));
		}
		else if (dynamic_cast<CCannon*>(coObjects->at(i))) {
			CCannon* cannon = dynamic_cast<CCannon*>(coObjects->at(i));
			if (onTopOf(cannon)) { this->onGround = true; }
			newCoObjects.push_back(coObjects->at(i));
		}
		else if (dynamic_cast<CBoomCannon*>(coObjects->at(i))) {
			newCoObjects.push_back(coObjects->at(i));
		}
		else if (dynamic_cast<CBlackBird*>(coObjects->at(i)))
		{
			if (!((CBlackBird*)(coObjects->at(i)))->DropPlayer())
				newCoObjects.push_back(coObjects->at(i));
		}
		else if (dynamic_cast<CTurle*>(coObjects->at(i))) newCoObjects.push_back(coObjects->at(i));

		if (dynamic_cast<CSewer*>(coObjects->at(i))) newCoObjects.push_back(coObjects->at(i));
		if (dynamic_cast<CInclinedBrick*>(coObjects->at(i))) {
			CInclinedBrick* brick = dynamic_cast<CInclinedBrick*>(coObjects->at(i));
			int tmp = brick->Collision(this, dy);
			if (tmp != 0)
				direction = tmp;
		}
		if (dynamic_cast<CConveyor*>(coObjects->at(i))) {
			CConveyor* conveyor = dynamic_cast<CConveyor*>(coObjects->at(i));
			if (onTopOf(conveyor)) { this->onGround = true; }
		}
		if (dynamic_cast<CBrick*>(coObjects->at(i))) {
			CBrick* brick = dynamic_cast<CBrick*>(coObjects->at(i));
			if (onTopOf(brick)) this->onGround = true;
			if (isUnder(brick, 2.0f)) this->underBrick = true;
			if (onSideOf(brick)) this->facingBrick = true;
		}
		if (dynamic_cast<CSpecialBrick*>(coObjects->at(i))) {
			CSpecialBrick* specialBrick = dynamic_cast<CSpecialBrick*>(coObjects->at(i));
			if (onTopOf(specialBrick) && specialBrick->type == 0) this->onGround = true;
			if (isUnder(specialBrick, 2.0f)) this->underBrick = true;
			if (onSideOf(specialBrick)) this->facingBrick = true;
		}
		if (dynamic_cast<CBlackEnemy*>(coObjects->at(i))) {
			CBlackEnemy* enemy = dynamic_cast<CBlackEnemy*>(coObjects->at(i));
			if (onTopOf(enemy, 7) && enemy->state == BLACKENEMY_STATE_WALK && this->vy < 0) {
				this->onGround = true;
				if (!onEnemy && !stunning) standOn(enemy); //fix loi khi cuoi nhieu quai 1 luc
			}
			/*if (isUnder(enemy)) {
				this->SetState(GIMMICK_STATE_STUN);
				StartUntouchable();
			}*/
		}
		if (dynamic_cast<CBrickPink*>(coObjects->at(i))) {
			CBrickPink* brick = dynamic_cast<CBrickPink*>(coObjects->at(i));
			if (onTopOf(brick, 3.5f)) { this->onGround = true; }
		}
		if (dynamic_cast<CSwing*>(coObjects->at(i))) {
			CSwing* swing = dynamic_cast<CSwing*>(coObjects->at(i));
			if (onTopOf(swing)) this->onGround = true;
		}
		if (dynamic_cast<CGreenBoss*>(coObjects->at(i))) {
			CGreenBoss* enemy = dynamic_cast<CGreenBoss*>(coObjects->at(i));
			if (onTopOf(enemy, 6) && enemy->state == GREENBOSS_STATE_WALK && this->vy < 0) {
				this->onGround = true;
				standOn(enemy); //fix loi khi cuoi nhieu quai 1 luc
			}
		}
		if (dynamic_cast<CElectricBlackEnemy*>(coObjects->at(i))) {
			CElectricBlackEnemy* enemy = dynamic_cast<CElectricBlackEnemy*>(coObjects->at(i));
			if (onTopOf(enemy) && this->vy < 0) {
				if (enemy->state == ELECTRIC_BLACKENEMY_STATE_WALK) {
					this->onGround = true;
					if (!onEnemy && !stunning) standOn(enemy);
				}
				if (enemy->state == ELECTRIC_BLACKENEMY_STATE_STOP) {
					this->onGround = true;
					if (!onEnemy && !stunning) standOn(enemy);
				}
			}
		}
		if (dynamic_cast<CCannon*>(coObjects->at(i))) {
			CCannon* cannon = dynamic_cast<CCannon*>(coObjects->at(i));
			if (onTopOf(cannon) && this->vy < 0) {
				this->onGround = true;
			}
		}
		if (dynamic_cast<CBoomCannon*>(coObjects->at(i))) {
			CBoomCannon* boom_cannon = dynamic_cast<CBoomCannon*>(coObjects->at(i));
			if (onTopOf(boom_cannon, 2.5f) && this->vy < 0) {
				this->onGround = true;
				standOn(boom_cannon);
			}
		}
		if (dynamic_cast<CStandBlackEnemy*>(coObjects->at(i)))
		{
			CStandBlackEnemy* bEnemy = dynamic_cast<CStandBlackEnemy*>(coObjects->at(i));
			if (onTopOf(bEnemy, 5.0f) && this->vy < 0)
			{
				if (bEnemy->state == ENEMY_STATE_STAND)
				{
					this->onGround = true;
					if (!onEnemy) standOn(bEnemy);
				}
			}
		}
		if (dynamic_cast<CTurle*>(coObjects->at(i))) {
			CTurle* turle = dynamic_cast<CTurle*>(coObjects->at(i));
			if (onTopOf(turle)) {
				this->onGround = true;
				standOn(turle);
			}
		}

		if (dynamic_cast<CCloudEnemy*>(coObjects->at(i))) {
			CCloudEnemy* enemy = dynamic_cast<CCloudEnemy*>(coObjects->at(i));
			if (onTopOf(enemy, 7.0f) && this->vy < 0) {
				this->onGround = true;
				standOn(enemy);
			}
		}

		if (dynamic_cast<CBlackBird*>(coObjects->at(i))) {
			CBlackBird* enemy = dynamic_cast<CBlackBird*>(coObjects->at(i));
			if (onTopOf(enemy, 2.0f) && this->vy < 0) {
				this->onGround = true;
			}
		}

		if (dynamic_cast<CSpecialBrick*>(coObjects->at(i))) {
			CSpecialBrick* brick = dynamic_cast<CSpecialBrick*>(coObjects->at(i));
			if (CheckAABB(brick) && brick->type == 0) {
				y = brick->y + 17;
				vy = 0;
			}
		}

		if (dynamic_cast<CWorm*>(coObjects->at(i))) {
			CWorm* worm = dynamic_cast<CWorm*>(coObjects->at(i));
			if (onTopOf(worm)) {
				vy = 0;
				onGround = true;
			}
		}
	}

	if (onInclinedBrick == true && direction != 0) {
		if (direction == -1) {
			this->x -= 0.5;
		}
		if (direction == 1) {
			this->x += 0.5;
		}
		if (direction == -2) {
			this->x -= 1;
		}
		if (direction == 2) {
			this->x += 1;
		}
	}

	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	// turn off collision when die 
	if (state != GIMMICK_STATE_DIE)
		CalcPotentialCollisions(&newCoObjects, coEvents);

	// reset untouchable timer if untouchable time has passed
	if (GetTickCount64() - untouchable_start > GIMMICK_UNTOUCHABLE_TIME && untouchable == 1)
	{
		untouchable_start = 0;
		untouchable = 0;
	}

	// reset stunnung timer if stunnung time has passed
	if (GetTickCount64() - stunning_start > GIMMICK_STUNNING_TIME && stunning == true)
	{
		stunning_start = 0;
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
			if (e->ny < 0) { this->falling = true; jumping = false; } //roi khi dung gach tren dau

			if (dynamic_cast<CTurle*>(e->obj)) {
				CTurle* turle = dynamic_cast<CTurle*>(e->obj);
				if (e->ny != 0) {
					//this->y = y0 + min_ty * dy + e->ny * 0.1f;
					if (turle->state == TURLE_STATE_DIE_RIGHT) {
						this->vy = 0;
						if (state == GIMMICK_STATE_WALKING_RIGHT)
						{
							x = x0 + min_tx * dx - 2.0f;
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
					if (turle->state == TURLE_STATE_DIE_LEFT) {
						this->vy = 0;
						if (state == GIMMICK_STATE_WALKING_RIGHT)
						{
							x = x0 + min_tx * dx + 1.5f;
						}
						if (state == GIMMICK_STATE_WALKING_LEFT)
						{
							x = x0 + min_tx * dx + 2.0f;
						}
						if (state == GIMMICK_STATE_IDLE)
						{
							x = x0 + 2.0f;
						}
					}
					if (turle->state == TURLE_STATE_WALKING_RIGHT 
						|| turle->state == TURLE_STATE_WALKING_LEFT) {
						if (untouchable == 0) {
							SetState(GIMMICK_STATE_STUN);
						}
						this->y = y0 + min_ty * dy;
					}
				}
				if (e->nx < 0) {
					if (turle->state == TURLE_STATE_DIE_RIGHT || turle->state == TURLE_STATE_DIE_LEFT) {
						this->x += 0.3;
						turle->x += 0.3;
					}
				}
				if (e->nx >= 0) {
					if (turle->state == TURLE_STATE_DIE_RIGHT || turle->state == TURLE_STATE_DIE_LEFT) {
						this->x -= 0.3;
						turle->x -= 0.3;
					}
				}

			}

			if (dynamic_cast<CBoat*>(e->obj)) {
				CBoat* Boat = dynamic_cast<CBoat*>(e->obj);
				//x = x0 + min_tx * dx + nx * 0.1f;
				//y = y0 + min_ty * dy + ny * 0.01f;

				if (e->nx != 0 ) {
					vx = 0;
				}
				if (e->ny > 0) {
					vy = 0;
					if(Boat->x <= Boat->GetFinish())
						Boat->run = true;
					//DebugOut(L"\nVao day %d", Boat->run);
					if (Boat->run == true)
					{
						x = x0 + min_tx * (dx + Boat->dx) + Boat->nx *0.7f ;
					}
				}

			}

			if (dynamic_cast<CBrick*>(e->obj)) {
				x = x0 + min_tx * dx + nx * 0.1f;
				if (onInclinedBrick) x = x0 + dx;
				y = y0 + min_ty * dy + ny * 0.1f;

				if (e->nx != 0 && inSewer == false) {
					vx = 0; 
					if (!onInclinedBrick) facingBrick = true;
				}
				if (e->ny != 0) {
					vy = 0;
					if (e->ny > 0) this->onGround = true;
				}

			}

			if (dynamic_cast<CSpecialBrick*>(e->obj)) {
				x = x0 + dx;
				y = y0 + dy;
				
				CSpecialBrick* brick = dynamic_cast<CSpecialBrick*>(e->obj);
				if (e->ny > 0 && brick->type == 0) {
					vy = 0;
					x = x0 + min_tx * dx + nx * 0.1f;
					y = y0 + min_ty * dy + ny * 0.1f;
					this->onGround = true;
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
						x = x0 + BrickPink->dx * 2 + BrickPink->nx * 0.011f;
					if(BrickPink->vy <0)
						y = y0 + min_ty * dy + ny * 0.1f;
					else
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
						}
						if (state == GIMMICK_STATE_WALKING_LEFT)
						{
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
				if (e->ny < 0)
					vy = 0;
			}

			if (dynamic_cast<CWorm*>(e->obj)) {
				

				CWorm* worm = dynamic_cast<CWorm*>(e->obj);
				if (worm->GetState() != WORM_STATE_DIE) {
					x = x0 + min_tx * dx + nx * 0.1f;
					y = y0 + min_ty * dy + ny * 0.1f;
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
						this->y += dy;
					}
					if (e->ny == 1)
						this->onGround = true;
				} else {
					x = x0 + dx;
					y = y0 + dy;
				}

				
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
					onGround = true;
				}
			}

			if (dynamic_cast<CBlackEnemy*>(e->obj)) {

				CBlackEnemy* enemy = dynamic_cast<CBlackEnemy*>(e->obj);

				if (e->ny > 0 && enemy->state == BLACKENEMY_STATE_WALK) {
					vy = 0;
					this->y = y0 + min_ty * dy + ny * 0.3f;
					standOn(enemy); 
				}
				else if(!untouchable && state != BLACKENEMY_STATE_DIE)
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
					if (untouchable == 0) {
						this->SetState(GIMMICK_STATE_STUN);
						StartUntouchable();
					}
				}
			}

			if (dynamic_cast<CElectricBlackEnemy*>(e->obj)) {

				CElectricBlackEnemy* enemy = dynamic_cast<CElectricBlackEnemy*>(e->obj);

				if (e->ny > 0 && enemy->state == ELECTRIC_BLACKENEMY_STATE_SHOCK) {
					if (untouchable == 0) {
						this->SetState(GIMMICK_STATE_STUN);
						StartUntouchable();
					}
				}
				if (e->ny > 0 && enemy->state == ELECTRIC_BLACKENEMY_STATE_WALK) {
					this->vy = 0;
					this->y = y0 + min_ty * dy + ny * 0.3f;
					standOn(enemy);
					enemy->SetState(ELECTRIC_BLACKENEMY_STATE_STOP);
				}
				if (e->ny > 0 && enemy->state == ELECTRIC_BLACKENEMY_STATE_STOP) {
					enemy->vx = 0;
					this->vy = 0;
					standOn(enemy);
					this->y = y0 + min_ty * dy + ny * 0.3f;
				}
				else if (!untouchable && enemy->state != ELECTRIC_BLACKENEMY_STATE_DIE)
				{
					if (enemy->x < this->x)
					{
						this->nx = 1.0;
					}
					else
					{
						this->nx = -1.0;
					}
					if (untouchable == 0) {
						this->SetState(GIMMICK_STATE_STUN);
						StartUntouchable();
					}
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
					vx = 0.25f;
					vy = 0.0f;
				}
				else if (e->nx > 0 && Sewer->type == 1)
				{
					XSewer = Sewer->GetX() - 16;
					YSewer = 0;
					nSewer = -1;
					y = Sewer->GetY() + 0.01;
					vx = -0.2f;
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

			if (dynamic_cast<CGun*>(e->obj)) 
			{

				if (coEventsResult.size() == 1)
				{
					y = y0 + min_ty * dy + ny * 0.1f;
				}
				if (e->nx != 0) 
				{

					if (!((CGun*)(e->obj))->onInclinedBrick)
					{
						x += dx;
						((CGun*)(e->obj))->updateX(dx);
					}
					else 
					{
						x = x0 + min_tx * dx + nx * 0.1f;
					}
				}

				if (e->ny != 0) 
				{
					vy = 0;
					this->onGround = true;
				}
			}

			if (dynamic_cast<CBullet*>(e->obj)) 
			{
				if (e->ny > 0) 
				{
					this->onGround = true;
					float vx1 = 0, vy1 = 0;
					(e->obj)->GetSpeed(vx1, vy1);
					this->vx = vx1;
				}
			}

			if(dynamic_cast<CCannon*>(e->obj))
			{
				CCannon* cannon = dynamic_cast<CCannon*>(e->obj);
				if (e->ny != 0)
				{
					this->onGround = true;
					vy = 0;
				}
				if (e->nx != 0)
				{
					cannon->SetState(CANNON_STATE_MOVE);
					cannon->x += dx;
					this->x += dx;
				}
			}

			if (dynamic_cast<CBlackBird*>(e->obj))
			{
				x = x0 + min_tx * dx + nx * 0.1f;
				y = y0 + min_ty * dy + ny * 0.1f;
				vy = 0;
				this->onGround = true;

				((CBlackBird*)(e->obj))->setOnBird(true);

			}

			if (dynamic_cast<CBoomCannon*>(e->obj))
			{
				CBoomCannon* boom_cannon = dynamic_cast<CBoomCannon*>(e->obj);
				if (e->ny > 0 && boom_cannon->state == BOOM_CANNON_STATE_DIE) {
					vy = 0;
					this->y = y0 + min_ty * dy + ny * 0.3f;
					standOn(boom_cannon);
				}
				if (!untouchable && state != BOOM_CANNON_STATE_DIE)
				{
					if (boom_cannon->x < this->x)
					{
						this->nx = 1.0;
					}
					else
					{
						this->nx = -1.0;
					}
					if (untouchable == 0) {
						this->SetState(GIMMICK_STATE_STUN);
						StartUntouchable();
					}
				}
			}
			
			if (dynamic_cast<CStandBlackEnemy*>(e->obj)) {

				CStandBlackEnemy* enemyB = dynamic_cast<CStandBlackEnemy*>(e->obj);

				if (e->ny > 0) 
				{	
					if (enemyB->state == ENEMY_STATE_STAND)
					{
						vy = 0;
						this->y = y0 + min_ty * dy + 1.0f;
					}
				}

				if (e->ny < 0)
				{
					if (enemyB->state == ENEMY_STATE_STAND)
					{
						this->SetState(GIMMICK_STATE_STUN);
					}
				}

				if (e->nx != 0)
				{
					if (enemyB->x < this->x)
					{
						this->vx = GIMMICK_DEFLECT_SPEED_X;
						this->nx = 1.0;
					}
					else 
					{
						this->vx = -GIMMICK_DEFLECT_SPEED_X;
						this->nx = -1.0;
					}
					if(enemyB->state == ENEMY_STATE_STAND)
						this->SetState(GIMMICK_STATE_STUN);
				}
			}

			if (dynamic_cast<CCloudEnemy*>(e->obj)) {
				CCloudEnemy* cloud = dynamic_cast<CCloudEnemy*>(e->obj);
				if (e->nx != 0)
				{
					if (e->nx != 0)
					{
						if (cloud->x < this->x)
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
				}
				if (e->ny > 0) {
					vy = 0;
					this->y = y0 + min_ty * dy + 0.3f;
					
					standOn(cloud);
				}

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

		animation_set->at(ani)->Render(x - 3.0f, y + 9.0f, 128);
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
					if (this->onGround||this->onBoat) ani = GIMMICK_ANI_IDLE_RIGHT;
					else ani = GIMMICK_ANI_JUMP_RIGHT;
				} 
				else {
					if (this->onGround || this->onBoat) ani = GIMMICK_ANI_IDLE_LEFT;
					else ani = GIMMICK_ANI_JUMP_LEFT;
				}
			}
			else if (vx > 0)
			{ //WALK RIGHT
				if (this->onGround || this->onBoat) ani = GIMMICK_ANI_WALKING_RIGHT;
				else ani = GIMMICK_ANI_JUMP_RIGHT;
			}
			else
			{ //WALK LEFT
				if (this->onGround || this->onBoat) ani = GIMMICK_ANI_WALKING_LEFT;
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
		Sound::GetInstance()->Play("SOUND_Effect_24",0, 1);
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
	{
		Sound::GetInstance()->Play("SOUND_Effect_35", 0, 1);
		if (untouchable == 0) {
			this->SetState(GIMMICK_STATE_IDLE);

			stunning = true;
			stunning_start = GetTickCount64();
			StartUntouchable();
			jumping = false;

			CStar* star = ((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->GetStar();
			star->Shot();
			CBackup::GetInstance()->UpdateLifeStack(CBackup::GetInstance()->lifeStack - 1);
		}
		else {
			this->SetState(GIMMICK_STATE_IDLE);
		}
	}
	break;
	case GIMMICK_STATE_DIE:
		Sound::GetInstance()->Play("SOUND_Effect_78", 0, 1);
		CBackup::GetInstance()->UpdateRest(CBackup::GetInstance()->rest - 1);
		CBackup::GetInstance()->UpdateLifeStack(4);
		this->die_start = GetTickCount64();
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
	if (dynamic_cast<CBlackEnemy*>(object)) //thu nho pham vi ngang cua quai, cho chan that, o chinh giua quai moi cuoi duoc
	{
		l = l + 2; //x pixel
		r = r - 2;
	}
	if (dynamic_cast<CElectricBlackEnemy*>(object)) //thu nho pham vi ngang cua quai, cho chan that, o chinh giua quai moi cuoi duoc
	{
		l = l + 1; //x pixel
		r = r - 1;
	}
	if (dynamic_cast<CStandBlackEnemy*>(object)) //thu nho pham vi ngang cua quai, cho chan that, o chinh giua quai moi cuoi duoc
	{
		l = l + 1; //x pixel
		r = r - 1;
	}
	if (dynamic_cast<CCloudEnemy*>(object)) //thu nho pham vi ngang cua quai, cho chan that, o chinh giua quai moi cuoi duoc
	{
		l = l + 1; //x pixel
		r = r - 1;
	}
	if (r >= ol && l <= or && abs(b - ot) < equal)
		return true;
	return false;
}

bool CGimmick::isUnder(CGameObject* object, float equal)
{
	float ol, ot, or , ob;
	object->GetBoundingBox(ol, ot, or , ob);
	float l, t, r, b;
	GetBoundingBox(l, t, r, b);
	if (dynamic_cast<CBrick*>(object) || dynamic_cast<CConveyor*>(object))
	{
		if (r >= ol && l <= or && (abs(ob - t) < equal))
			return true;
	}
	return false;
}

void CGimmick::DetectStar()
{
	if (state == GIMMICK_STATE_DIE)
		return;
	CScene* scene = CGame::GetInstance()->GetCurrentScene();
	CStar* star = ((CPlayScene*)scene)->GetStar();
	if (star->state == STAR_STATE_WALKING_LEFT || star->state == STAR_STATE_WALKING_RIGHT) {
		if (onTopOf(star, 4.0f) && vy < 0) {
			standOn(star);
		}
	}
}

void CGimmick::Revival() {
	float revival_x, revival_y;
	CScene* scene = CGame::GetInstance()->GetCurrentScene();
	((CPlayScene*)scene)->GetRevivalPosition(revival_x, revival_y);
	this->SetPosition(revival_x, revival_y);
	this->SetSpeed(0, 0);
	this->ax = 0;
	this->ay = 0;
	this->SetState(GIMMICK_STATE_IDLE);
	this->stunning = false;
	this->untouchable = 0;
}

bool CGimmick::onSideOf(CGameObject* object, float equal)
{
	float ol, ot, or , ob;
	object->GetBoundingBox(ol, ot, or , ob);
	float l, t, r, b;
	GetBoundingBox(l, t, r, b);
	if (dynamic_cast<CBrick*>(object))
	{
		if (b <= ot && t >= ob && ((abs(l - or ) < equal && l >= or) || (abs(r - ol) < equal && r <= ol)))
			return true;
	}
	return false;
}

void CGimmick::standOn(CGameObject* object)
{
	if (underBrick) return;

	if (dynamic_cast<CBlackEnemy*>(object))
	{
		CBlackEnemy* enemy = dynamic_cast<CBlackEnemy*>(object);

		onEnemy = true;
		enemy->carry_player = true;

		if (!facingBrick) {
			if (enemy->onFastConveyor)
				this->x += object->dx + 1.0f;
			else if(enemy->onSlowConveyor)
				this->x += object->dx - 1.0f;
			else
				this->x += object->dx;
		}

		if (!jumping) { 
			this->y = object->y + GIMMICK_BBOX_HEIGHT ; //-2 pixel thi bi va cham voi quai khac
			this->vy = 0; 
		}
	}

	if (dynamic_cast<CGreenBoss*>(object))
	{
		CGreenBoss* enemy = dynamic_cast<CGreenBoss*>(object);

		onEnemy = true;
		if (!facingBrick) {
				this->x += object->dx;
		}

		if (!jumping) {
			this->y = object->y + GIMMICK_BBOX_HEIGHT; 
			this->vy = 0;
		}
	}

	if (dynamic_cast<CStar*>(object))
	{
		CStar* star = dynamic_cast<CStar*>(object);

		onStar = true;
		onGround = true;
		if (!facingBrick) {
			this->x += object->dx;
		}

		if (!jumping) {
			this->y = object->y + GIMMICK_BBOX_HEIGHT;
			this->vy = 0;
		}
	}

	if (dynamic_cast<CElectricBlackEnemy*>(object)) {
		CElectricBlackEnemy* enemy = dynamic_cast<CElectricBlackEnemy*>(object);

		this->onEnemy = true;
		this->vy = 0;
		if (!jumping) {
			if (enemy->state == ELECTRIC_BLACKENEMY_STATE_WALK) {
				if (this->onEnemy)
					return;
				else {
					SetState(ELECTRIC_BLACKENEMY_STATE_STOP);
					this->y = object->y + GIMMICK_BBOX_HEIGHT;
				}
			}
		}

	}

	if (dynamic_cast<CBoomCannon*>(object)) {

		this->x += object->dx;
		if (!jumping) {
			this->y = object->y + GIMMICK_BBOX_HEIGHT + 0.5f;
			this->vy = 0;
		}
	}

	if (dynamic_cast<CStandBlackEnemy*>(object)) {
		CStandBlackEnemy* bEnemy = dynamic_cast<CStandBlackEnemy*>(object);

		onEnemy = true;
		this->vy = 0;
		if (state == ENEMY_STATE_STAND) {
			this->y = object->y + GIMMICK_BBOX_HEIGHT -1;
		}
	}
	if (dynamic_cast<CTurle*>(object)) {
		CTurle* turle = dynamic_cast<CTurle*>(object);
		if (turle->state == TURLE_STATE_DIE_RIGHT) {
			this->x -= 0.1f;
			if (!jumping) {
				this->y = object->y + GIMMICK_BBOX_HEIGHT + 0.5f;
				this->vy = 0;
			}
		}
		if (turle->state == TURLE_STATE_DIE_LEFT) {
			this->x += 0.1f;
			if (!jumping) {
				this->y = object->y + GIMMICK_BBOX_HEIGHT + 0.5f;
				this->vy = 0;
			}
		}
	}

	if (dynamic_cast<CCloudEnemy*>(object)) {
		CCloudEnemy* enemy = dynamic_cast<CCloudEnemy*>(object);
		if(!facingBrick) // fix loi xuyen tuong khi cuoi
			this->x += object->dx;
		onEnemy = true;
		enemy->carryPlayer = true;
		this->vy = 0;
		this->y = object->y + GIMMICK_BBOX_HEIGHT;

	}
}

