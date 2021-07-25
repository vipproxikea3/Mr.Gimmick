#include "BlackEnemy.h"
#include "Utils.h"
#include "InclinedBrick.h"
#include "Backup.h"

CBlackEnemy::CBlackEnemy(int direction, int level)
{
	this->score = 120;
	if (level == 0)
	{
		SetState(BLACKENEMY_STATE_WALK);
		if (direction == 1) //chinh huong di ban dau
			ax = BLACKENEMY_ACCELERATION; // moi vo di sang Phai 1
		else
			ax = -BLACKENEMY_ACCELERATION; // moi vo di sang trai -1
	}
	else
	{
		SetState(BLACKENEMY_STATE_FLY);
		if (direction == 1) //chinh huong di ban dau
			ax = BLACKENEMY_FLY_ACCELERATION; // moi vo di sang Phai 1
		else
			ax = -BLACKENEMY_FLY_ACCELERATION; // moi vo di sang trai -1
	}
	

	this->canTurnAround = false; //tranh truong hop moi vao da quay dau
}

void CBlackEnemy::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (this->GetState() == BLACKENEMY_STATE_DIE && !CGame::GetInstance()->InCamera(this) && CGame::GetInstance()->InLargeCamera(this) && this->visible == true) {
		this->visible = false;
	}
	if (!visible)
		return;

	CGameObject::Update(dt, coObjects);

	vx += ax * dt;
	vy -= BLACKENEMY_GRAVITY * dt;

	onInclinedBrick = false;
	detectedStraighRoad = false;
	onGround = false;
	facingBrick = false;
	onFastConveyor = false;
	onSlowConveyor = false;

	CalculateSpeed();
	DetectPlayer();
	Transform();
	SpecialCollisionWithPlayer();
	DetectStar();

	vector<LPGAMEOBJECT> newCoObjects;
	for (UINT i = 0; i < coObjects->size(); i++)
	{
		if (dynamic_cast<CBrick*>(coObjects->at(i))) newCoObjects.push_back(coObjects->at(i));
		else if (dynamic_cast<CConveyor*>(coObjects->at(i))) newCoObjects.push_back(coObjects->at(i));
		else if (dynamic_cast<CSwing*>(coObjects->at(i))) newCoObjects.push_back(coObjects->at(i));
		else if (dynamic_cast<CBrickPink*>(coObjects->at(i))) newCoObjects.push_back(coObjects->at(i));

		if (dynamic_cast<CInclinedBrick*>(coObjects->at(i))) {
			CInclinedBrick* brick = dynamic_cast<CInclinedBrick*>(coObjects->at(i));
			if (onTopOf(brick)) this->detectedStraighRoad = true;
			if (state != BLACKENEMY_STATE_DIE) brick->Collision(this, dy); //fix loi chet roi van leo doc duoc
		}
		if (dynamic_cast<CConveyor*>(coObjects->at(i))) {
			CConveyor* conveyor = dynamic_cast<CConveyor*>(coObjects->at(i));
			if (onTopOf(conveyor)) { this->onGround = true; this->detectedStraighRoad = true;}
		}
		if (dynamic_cast<CBrick*>(coObjects->at(i))) {
			CBrick* brick = dynamic_cast<CBrick*>(coObjects->at(i));
			if (onTopOf(brick)) this->onGround = true;
			if (onSideOf(brick)) { this->facingBrick = true;
			}
		}
		if (dynamic_cast<CSewer*>(coObjects->at(i))) {
			CSewer* sewer = dynamic_cast<CSewer*>(coObjects->at(i));
			if (CheckAABB(sewer) && sewer->type != SEWER_TYPE_1 && sewer->type != SEWER_TYPE_2)
				visible = false;
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
			

			if (state == BLACKENEMY_STATE_BULLET) state = BLACKENEMY_STATE_WALK;
			if (e->ny > 0) onGround = true;

			if (dynamic_cast<CBrick*>(e->obj))
			{
				CBrick *brick = dynamic_cast<CBrick*>(e->obj);
				x = x0 + min_tx * dx + nx * 0.1f;
				y = y0 + min_ty * dy + ny * 0.1f;
				if (onInclinedBrick) x = x0 + dx;
				if (e->nx != 0)
				{
					vx = 0;
					if (state == BLACKENEMY_STATE_WALK) {
						if (this->onGround && InJumpablePosition())
							Jump();
					}
				}
				if (e->ny > 0)
				{
					vy = 0;
					if (((l < ol - 3 && vx < 0) || (r > or + 3 && vx > 0)) && InJumpablePosition() && !detectedStraighRoad) Jump(); //bien detectedStraighRoad phat hien duong thang khong cho nhay
				}
				if (e->ny < 0 && !facingBrick && !onGround)
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
					if (conveyor->GetDirection() > 0) {
						x += 1.0f;
						onFastConveyor = true;
					}
					else {
						x -= 1.0f;
						onSlowConveyor = true;
					}
				}
			}
			if (dynamic_cast<CSewer*>(e->obj))
			{
				CSewer* sewer = dynamic_cast<CSewer*>(e->obj);
				if (sewer->type != SEWER_TYPE_1 && sewer->type != SEWER_TYPE_2) {
					visible = false;
				}
			}
		}
	}
	// clean up newCoObjects
	for (UINT i = 0; i < newCoObjects.size(); i++) newCoObjects[i] = nullptr;
	// clean up collision events
	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
}

void CBlackEnemy::Render()
{
	if (!visible)
		return;

	int ani = 0;
	if (state == BLACKENEMY_STATE_WALK || state == BLACKENEMY_STATE_BULLET) { //==============WALK
		if (ax > 0)
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
		if (ax > 0) {
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
	else if (state == BLACKENEMY_STATE_DIE) { //============== DIE
		if (nx > 0)
			ani = BLACKENEMY_ANI_DIE_RIGHT;
		else
			ani = BLACKENEMY_ANI_DIE_LEFT;
	}
	animation_set->at(ani)->Render(x, y);

	//RenderBoundingBox();
}

void CBlackEnemy::SetState(int state)
{
	CGameObject::SetState(state);
	Sound::GetInstance()->Stop("SOUND_Effect_8");
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
		Sound::GetInstance()->Play("SOUND_Effect_8", 1);
		if (nx > 0) 
			ax = BLACKENEMY_FLY_ACCELERATION;
		else
			ax = -BLACKENEMY_FLY_ACCELERATION;
		break;
	case BLACKENEMY_STATE_BULLET:
		ax = BLACKENEMY_ACCELERATION;
		break;
	case BLACKENEMY_STATE_DIE:
		ax = 0;
		if (CheckSideOfStar() == -1){
			vx = BLACKENEMY_DEFLECT_SPEED_X;
		}
		else {
			vx = -BLACKENEMY_DEFLECT_SPEED_X;
		}
		vy = BLACKENEMY_DEFLECT_SPEED_Y;
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
	// ===== BULLET:
	if (state == BLACKENEMY_STATE_BULLET && vx > BLACK_ENEMY_BULLET_SPEED)
		vx = BLACK_ENEMY_BULLET_SPEED;
	if (state == BLACKENEMY_STATE_BULLET && vx < -BLACK_ENEMY_BULLET_SPEED)
		vx = -BLACK_ENEMY_BULLET_SPEED;
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
		if (!player->onEnemy) //ko dc quay dau tren khong trung
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
		this->y += 1; // day len 1 chut tranh loi rot xuyen gach
	} //BAT DAU BIEN HINH

	if (state == BLACKENEMY_STATE_TRANSFORM && this->transforming && GetTickCount64() - this->transform_start >= BLACKENEMY_TRANSFORM_TIME)
	{
		SetState(BLACKENEMY_STATE_FLY);
		this->transform_start = 0;
		this->transforming = false;
	}

}

bool CBlackEnemy::onSideOf(CGameObject* object, float equal)
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

bool CBlackEnemy::isUnder(CGameObject* object, float equal)
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

bool CBlackEnemy::InJumpablePosition()
{
	CScene* scene = CGame::GetInstance()->GetCurrentScene();
	CGimmick* player = ((CPlayScene*)scene)->GetPlayer();

	if (y - BLACKENEMY_HEIGHT <= player->y)
		return true;
	return false;
}

void CBlackEnemy::DetectStar()
{
	CScene* scene = CGame::GetInstance()->GetCurrentScene();
	CStar* star = ((CPlayScene*)scene)->GetStar();

	if (CheckAABB(star) && state != BLACKENEMY_STATE_DIE && star->state != STAR_STATE_HIDE 
		&& star->state != STAR_STATE_EXPLOSIVE && star->state != STAR_STATE_PENDING && star->state != STAR_STATE_READY)
	{
		SetState(BLACKENEMY_STATE_DIE);

		CBackup* backup = CBackup::GetInstance();
		backup->UpdateScore(backup->score + this->score);
	}
}

int CBlackEnemy::CheckSideOfStar() // -1 left, 1 right dung de xac dinh huong die
{
	CScene* scene = CGame::GetInstance()->GetCurrentScene();
	CStar* star = ((CPlayScene*)scene)->GetStar();
	if (star->x < x)
		return -1;
	else
		return 1;
}

void CBlackEnemy::SpecialCollisionWithPlayer()
{
	CScene* scene = CGame::GetInstance()->GetCurrentScene();
	CGimmick* player = ((CPlayScene*)scene)->GetPlayer();

	if (player->state != GIMMICK_STATE_DIE && player->getUntouchable() != 1
		&& state != BLACKENEMY_STATE_DIE)
	{
		if (IsCollidingWithPlayer())
		{
			if (x < player->x)
			{
				//player->vx = GIMMICK_DEFLECT_SPEED_X;
				player->nx = 1.0;
			}
			else
			{
				//player->vx = -GIMMICK_DEFLECT_SPEED_X;
				player->nx = -1.0;
			}

			player->SetState(GIMMICK_STATE_STUN);
			player->StartUntouchable();
		}
	}
}

bool CBlackEnemy::IsCollidingWithPlayer()
{
	CScene* scene = CGame::GetInstance()->GetCurrentScene();
	CGimmick* player = ((CPlayScene*)scene)->GetPlayer();

	float l, t, r, b;
	GetBoundingBox(l, t, r, b);
	t -= 5;

	float ol, ot, or , ob;
	player->GetBoundingBox(ol, ot, or , ob);

	if (or >= l && ol <= r
		&& ot >= b && ob <= t)
		return true;
	return false;
}