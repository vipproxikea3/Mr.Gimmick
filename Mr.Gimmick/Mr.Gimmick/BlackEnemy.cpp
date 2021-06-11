#include "BlackEnemy.h"
#include "Utils.h"
#include "InclinedBrick.h"
CBlackEnemy::CBlackEnemy()
{
	SetState(BLACKENEMY_STATE_WALK);
	this->canTurnAround = false; //tranh truong hop moi vao da quay dau
}

void CBlackEnemy::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CGameObject::Update(dt, coObjects);

	vx += ax * dt;
	vy -= BLACKENEMY_GRAVITY * dt;

	onInclinedBrick = false;
	onGround = false;

	CalculateSpeed();
	DetectPlayer();
	Transform();

	vector<LPGAMEOBJECT> newCoObjects;
	for (UINT i = 0; i < coObjects->size(); i++)
	{
		if (dynamic_cast<CBrick*>(coObjects->at(i))) newCoObjects.push_back(coObjects->at(i));
		else if (dynamic_cast<CConveyor*>(coObjects->at(i))) newCoObjects.push_back(coObjects->at(i));
		else if (dynamic_cast<CSwing*>(coObjects->at(i))) newCoObjects.push_back(coObjects->at(i));
		else if (dynamic_cast<CBrickPink*>(coObjects->at(i))) newCoObjects.push_back(coObjects->at(i));

		if (dynamic_cast<CInclinedBrick*>(coObjects->at(i))) {
			CInclinedBrick* brick = dynamic_cast<CInclinedBrick*>(coObjects->at(i));
			brick->Collision(this, dy);
		}
		if (dynamic_cast<CConveyor*>(coObjects->at(i))) {
			CConveyor* conveyor = dynamic_cast<CConveyor*>(coObjects->at(i));
			if (onTopOf(conveyor)) { this->onGround = true; }
		}
		if (dynamic_cast<CBrick*>(coObjects->at(i))) {
			CBrick* brick = dynamic_cast<CBrick*>(coObjects->at(i));
			if (onTopOf(brick)) this->onGround = true;
		}
	}

	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	if (state != BLACKENEMY_STATE_DIE)
		CalcPotentialCollisions(&newCoObjects, coEvents);

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

		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);

		float l, t, r, b;
		GetBoundingBox(l, t, r, b);
		for (UINT i = 0; i < coEventsResult.size(); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];
			float ol, ot, or , ob;
			e->obj->GetBoundingBox(ol, ot, or , ob);
			

			if (e->ny > 0) onGround = true;

			if (dynamic_cast<CBrick*>(e->obj))
			{
				CBrick *brick = dynamic_cast<CBrick*>(e->obj);
				x = x0 + min_tx * dx + nx * 0.1f;
				//y = y0 + min_ty * dy + ny * 0.1f;
				y += min_ty * dy + ny * 0.1f;
				if (onInclinedBrick) x = x0 + dx;
				if (e->nx != 0)
				{
					vx = 0;
					if (state == BLACKENEMY_STATE_WALK) {
						if (this->onGround)
							Jump();
					}
				}
				if (e->ny > 0)
				{
					vy = 0;
					if ((l < ol && vx < 0) || (r > or && vx > 0)) Jump();
				}
				if (e->ny < 0)
					vy = -BLACKENEMY_GRAVITY;
			}
			if (dynamic_cast<CConveyor*>(e->obj))
			{
				x = x0 + min_tx * dx + nx * 0.1f;
				y = y0 + min_ty * dy + ny * 0.1f;
				if (onInclinedBrick) x = x0 + dx;
				CConveyor* conveyor = dynamic_cast<CConveyor*>(e->obj);
				if (e->ny < 0) {
					if(state != BLACKENEMY_STATE_FLY)
						vy = -BLACKENEMY_GRAVITY;
				}
				if (e->ny > 0) {
					if (conveyor->GetDirection() > 0)
						x += 0.5f;
					else
						x -= 0.5f;
				}
			}
		}
	}
	// clean up newCoObjects
	for (UINT i = 0; i < newCoObjects.size(); i++) newCoObjects[i] = nullptr;
	// clean up collision events
	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
	DebugOut(L"%d \n", jump_count);
}

void CBlackEnemy::Render()
{
	int ani = 0;
	if (state == BLACKENEMY_STATE_WALK) { //==============WALK
		if (vx > 0)
			ani = BLACKENEMY_ANI_WALK_RIGHT;
		else
			ani = BLACKENEMY_ANI_WALK_LEFT;
	}
	else if (state == BLACKENEMY_STATE_TRANSFORM) {  //==============TRANSFORM
		if (vx > 0)
			ani = BLACKENEMY_ANI_FLY_RIGHT;
		else
			ani = BLACKENEMY_ANI_FLY_LEFT;
	}
	else if (state == BLACKENEMY_STATE_FLY) { //==============FLY
		if (vx > 0) {
			if (this->onGround)
				ani = BLACKENEMY_ANI_FLY_ONGROUND_RIGHT;
			else
				ani = BLACKENEMY_ANI_FLY_RIGHT;
		}
		else {
			if (this->onGround)
				ani = BLACKENEMY_ANI_FLY_ONGROUND_LEFT;
			else
				ani = BLACKENEMY_ANI_FLY_LEFT;
		}
	}

	animation_set->at(ani)->Render(x, y);

	//RenderBoundingBox();
}

void CBlackEnemy::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case BLACKENEMY_STATE_WALK:
		ax = -BLACKENEMY_ACCELERATION; // moi vo di sang Phai
		break;
	case BLACKENEMY_STATE_TRANSFORM:
		if (ax > 0)
			nx = 1;
		else
			nx = -1; //Xac dinh huong de chuyen sang trang thai FLY
		ax = 0;
		vx = 0;
		break;
	case BLACKENEMY_STATE_FLY: 
		if (nx > 0) 
			ax = BLACKENEMY_FLY_ACCELERATION;
		else
			ax = -BLACKENEMY_FLY_ACCELERATION;
		break;
	}
}

void CBlackEnemy::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	if (state == BLACKENEMY_STATE_FLY) t = y - 1;
		else t = y;
	r = x + BLACKENEMY_WIDTH;
	b = y - BLACKENEMY_HEIGHT;
}

void CBlackEnemy::Jump()
{
	CScene* scene = CGame::GetInstance()->GetCurrentScene();
	CGimmick* player = ((CPlayScene*)scene)->GetPlayer();

	vy = BLACKENEMY_JUMP_SPEED;
	player->onEnemy = true; 
	
	if (this->carry_player && vx == 0)
		jump_count += 1;
}

bool CBlackEnemy::onTopOf(CGameObject* object)
{
	float ol, ot, or , ob;
	object->GetBoundingBox(ol, ot, or , ob);
	float l, t, r, b;
	GetBoundingBox(l, t, r, b);
	if (r >= ol && l <= or && abs(b - ot) < 1)
		return true;
	return false;
}

void CBlackEnemy::CalculateSpeed()
{
	CScene* scene = CGame::GetInstance()->GetCurrentScene();
	CGimmick* player = ((CPlayScene*)scene)->GetPlayer();

	// ===== WALK:
	if (state == BLACKENEMY_STATE_WALK && vx > BLACKENEMY_WALK_SPEED)
		vx = BLACKENEMY_WALK_SPEED;
	if (state == BLACKENEMY_STATE_WALK && vx < -BLACKENEMY_WALK_SPEED)
		vx = -BLACKENEMY_WALK_SPEED;
	// ===== FLY:
	//SPEED X:
	if (state == BLACKENEMY_STATE_FLY && vx > BLACKENEMY_FLY_SPEED)
		vx = BLACKENEMY_FLY_SPEED;
	if (state == BLACKENEMY_STATE_FLY && vx < -BLACKENEMY_FLY_SPEED)
		vx = -BLACKENEMY_FLY_SPEED;
	//SPEED Y:
	if (state == BLACKENEMY_STATE_FLY) {
		if (this->y < player->y) vy = BLACKENEMY_FLY_SPEED_Y;
		if (this->y > player->y) vy = -BLACKENEMY_FLY_SPEED_Y;
		if (this->y == player->y) vy = 0; //fix loi giat giat
	}
}

void CBlackEnemy::TurnAroundSlowly()
{
	if (state == BLACKENEMY_STATE_WALK)
	{
		if (vx > 0)
			ax = -BLACKENEMY_ACCELERATION;
		if (vx < 0)
			ax = BLACKENEMY_ACCELERATION;
	}
	if (state == BLACKENEMY_STATE_FLY)
	{
		if (vx > 0)
			ax = -BLACKENEMY_FLY_ACCELERATION;
		if (vx < 0)
			ax = BLACKENEMY_FLY_ACCELERATION;
	}
}

void CBlackEnemy::TurnAroundImmediately()
{
	if (state == BLACKENEMY_STATE_WALK)
	{
		if (vx > 0) {
			vx = 0;
			ax = -BLACKENEMY_ACCELERATION;
		}
		if (vx < 0) {
			vx = 0;
			ax = BLACKENEMY_ACCELERATION;
		}
	}
	if (state == BLACKENEMY_STATE_FLY)
	{
		if (vx > 0) {
			vx = 0;
			ax = -BLACKENEMY_FLY_ACCELERATION;
		}
		if (vx < 0) {
			vx = 0;
			ax = BLACKENEMY_FLY_ACCELERATION;
		}
	}
	
}

void CBlackEnemy::DetectPlayer()
{
	CScene* scene = CGame::GetInstance()->GetCurrentScene();
	CGimmick* player = ((CPlayScene*)scene)->GetPlayer();

	//Reset dem neu ngang hang hoac o duoi gimmick:
	if (this->y >= player->y) {
		turn_count = 0;
		jump_count = 0;
	}
	// Duoi theo gimmick:
	if (state == BLACKENEMY_STATE_WALK)
	{
		if (!player->onEnemy)
		{
			if (this->canTurnAround && this->x > player->x + PLAYER_MAX_RANGE_WALK && this->ax > 0)
			{
				TurnAroundSlowly();
				this->canTurnAround = false;

				if (this->y < player->y) //Neu Gimmick o tren cao bat dau dem
					turn_count += 1;
			}
			if (this->canTurnAround && this->x < player->x - PLAYER_MAX_RANGE_WALK && this->ax < 0)
			{
				TurnAroundSlowly();
				this->canTurnAround = false;

				if (this->y < player->y)
					turn_count += 1;
			}
		}
		else 
		{ // Dang bi gimmick cuoi thi quay dau theo huong gimmick lai'
			if (player->x > this->x + 5) //So pixel chenh lech de lai' theo huong
			{
				this->ax = BLACKENEMY_ACCELERATION;
			}
			if (player->x < this->x - 5)
			{
				this->ax = -BLACKENEMY_ACCELERATION;
			}
		}
		

	}

	if (state == BLACKENEMY_STATE_FLY)
	{
		if (this->canTurnAround && this->x > player->x + PLAYER_MAX_RANGE_FLY + 16)
		{
			TurnAroundSlowly();
			this->canTurnAround = false;
		}
		if (this->canTurnAround && this->x < player->x - PLAYER_MAX_RANGE_FLY)
		{
			TurnAroundSlowly();
			this->canTurnAround = false;
		}
	}
	
	//reset quay dau
	if (!player->onEnemy) {
		if (!this->canTurnAround && (this->x >= player->x - (PLAYER_MAX_RANGE_FLY - 1)) && (this->x <= player->x + (PLAYER_MAX_RANGE_FLY - 1) /*+ 16*/)) //reset turn around, can dam bao so +- < PLAYER_MAX_RANGE_FLY
			this->canTurnAround = true;
	}
	
	if (!player->onEnemy) //Reset bien carry_player neu gimmick khong cuoi quai (vi gimmick 1 luc chi cuoi~ 1 con)
		carry_player = false;
}

void CBlackEnemy::Transform()
{
	if ((turn_count == TURN_COUNT_TO_TRANSFORM || jump_count >= JUMP_COUNT_TO_TRANSFORM )&& state == BLACKENEMY_STATE_WALK) {
		SetState(BLACKENEMY_STATE_TRANSFORM);
		this->transform_start = GetTickCount64();
		this->transforming = true;
	} //BAT DAU BIEN HINH

	if (state == BLACKENEMY_STATE_TRANSFORM && this->transforming && GetTickCount64() - this->transform_start >= BLACKENEMY_TRANSFORM_TIME)
	{
		SetState(BLACKENEMY_STATE_FLY);
		this->transform_start = 0;
		this->transforming = false;
	}

}