#include "GreenBoss.h"
#include "Utils.h"
#include "Backup.h"

CGreenBoss::CGreenBoss()
{
	this->score = 1800;
	SetState(GREENBOSS_STATE_WALK);
	ax = GREENBOSS_ACCELERATION; // di sang phai

	this->canTurnAround = false; //tranh truong hop moi vao da quay dau
}

void CGreenBoss::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CGameObject::Update(dt, coObjects);

	vx += ax * dt;
	vy -= GREENBOSS_GRAVITY * dt;

	CalculateSpeed();
	DetectPlayer();
	DetectStar();
	CalculateStun();
	SpecialCollisionWithPlayer();

	onGround = false;

	vector<LPGAMEOBJECT> newCoObjects;
	for (UINT i = 0; i < coObjects->size(); i++)
	{
		if (dynamic_cast<CBrick*>(coObjects->at(i))) newCoObjects.push_back(coObjects->at(i));
		if (dynamic_cast<CBrick*>(coObjects->at(i))) {
			CBrick* brick = dynamic_cast<CBrick*>(coObjects->at(i));
			if (onTopOf(brick, 1.0f)) this->onGround = true;
			if (onSideOf(brick) && onGround) {
				float ol, ot, or , ob;
				brick->GetBoundingBox(ol, ot, or , ob);
				if (ob <= x && ot > x && x + GREENBOSS_BBOX_HEIGHT < ob)
					vx = -0.8f;
				Jump();
			}
		}
	}

	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	if (state != GREENBOSS_STATE_DIE)
		CalcPotentialCollisions(&newCoObjects, coEvents);

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
				CBrick* brick = dynamic_cast<CBrick*>(e->obj);
				x = x0 + min_tx * dx + nx * 0.3f;
				y = y0 + min_ty * dy + ny * 0.1f;
				if (e->nx != 0)
				{
					vx = 0;
					if (onGround) Jump();
					if (ob <= t && ot > t && b < ob ) {
						ax = -ax;
						vx = -0.8f ;
						canTurnAround = true;
					}
				}
				if (e->ny > 0)
				{
					onGround = true;
					vy = 0;
					if (((l < ol - 3 && vx < 0) || (r > or +3 && vx > 0))) { Jump(); }
				}
			}
		}
	}
	// clean up newCoObjects
	for (UINT i = 0; i < newCoObjects.size(); i++) newCoObjects[i] = nullptr;
	// clean up collision events
	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
}

void CGreenBoss::Render()
{
	int ani = 0;
	if (state == GREENBOSS_STATE_WALK) { //==============WALK
		if (onGround) // tren mat dat
		{
			if (ax > 0)
				ani = GREENBOSS_ANI_WALK_RIGHT;
			else
				ani = GREENBOSS_ANI_WALK_LEFT;
		}
		else //nhay
		{
			if (vy > 0)
			{
				if (ax > 0)
					ani = GREENBOSS_ANI_JUMP_RIGHT_UP;
				else
					ani = GREENBOSS_ANI_JUMP_LEFT_UP;
			}
			else
			{
				if (ax > 0)
					ani = GREENBOSS_ANI_JUMP_LEFT_DOWN;
				else
					ani = GREENBOSS_ANI_JUMP_LEFT_DOWN;
			}
		}
		
	}
	else if (state == GREENBOSS_STATE_DIE) {
		if (nx < 0)
			ani = GREENBOSS_ANI_DIE_LEFT;
		else
			ani = GREENBOSS_ANI_DIE_RIGHT;
	}

	animation_set->at(ani)->Render(x, y + 4, alphaRender);

	//RenderBoundingBox();
}

void CGreenBoss::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case GREENBOSS_STATE_WALK:
		ax = -GREENBOSS_ACCELERATION; // moi vo di sang Phai
		break;
	case GREENBOSS_STATE_DIE:
		CBackup* backup = CBackup::GetInstance();
		backup->UpdateScore(backup->score + this->score);
		ax = 0;
		if (CheckSideOfStar() == -1) {
			vx = GREENBOSS_DEFLECT_SPEED_X;
			nx = 1;
		}
		else {
			vx = -GREENBOSS_DEFLECT_SPEED_X;
			nx = -1;
		}
		vy = GREENBOSS_DEFLECT_SPEED_Y;
		break;
	
	}
}

void CGreenBoss::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = x + GREENBOSS_BBOX_WIDTH;
	b = y - GREENBOSS_BBOX_HEIGHT;
}

void CGreenBoss::Jump()
{
	vy = GREENBOSS_JUMP_SPEED;
}

void CGreenBoss::CalculateSpeed()
{
	CScene* scene = CGame::GetInstance()->GetCurrentScene();
	CGimmick* player = ((CPlayScene*)scene)->GetPlayer();

	// ===== WALK:
	if (state == GREENBOSS_STATE_WALK && vx > GREENBOSS_WALK_SPEED)
		vx = GREENBOSS_WALK_SPEED;
	if (state == GREENBOSS_STATE_WALK && vx < -GREENBOSS_WALK_SPEED)
		vx = -GREENBOSS_WALK_SPEED;
}

void CGreenBoss::CalculateStun()
{
	if (!is_stunning) 
		alphaRender = 255;
	else {
		if (((GetTickCount64() - stun_start) / 100) % 2 == 1) //nhap nhay lien tuc moi 100ms
			alphaRender = 100; //muc do nhap nhay
		else
			alphaRender = 255;
	}

	if (is_stunning && GetTickCount64() - stun_start >= GREENBOSS_STUN_TIME) {
		is_stunning = false;
		stun_start = 0;
	}
}

void CGreenBoss::TurnAroundSlowly()
{
	if (state == GREENBOSS_STATE_WALK)
	{
		if (vx > 0)
			ax = -GREENBOSS_ACCELERATION;
		if (vx < 0)
			ax = GREENBOSS_ACCELERATION;
	}
}

void CGreenBoss::DetectPlayer()
{
	CScene* scene = CGame::GetInstance()->GetCurrentScene();
	CGimmick* player = ((CPlayScene*)scene)->GetPlayer();

	// Duoi theo gimmick:
	if (state == GREENBOSS_STATE_WALK)
	{
		if (this->canTurnAround && this->x > player->x + PLAYER_MAX_RANGE_WALK_ON_GREENBOSS && this->ax > 0)
		{
			TurnAroundSlowly();
			this->canTurnAround = false;
		}
		if (this->canTurnAround && this->x < player->x - PLAYER_MAX_RANGE_WALK_ON_GREENBOSS && this->ax < 0)
		{
			TurnAroundSlowly();
			this->canTurnAround = false;
		}
	}

	//reset quay dau
	if (!this->canTurnAround && (this->x >= player->x - (PLAYER_MAX_RANGE_FLY - 1)) && (this->x <= player->x + (PLAYER_MAX_RANGE_FLY - 1) )) //reset turn around, can dam bao so +- < PLAYER_MAX_RANGE_FLY
		this->canTurnAround = true;
}


void CGreenBoss::DetectStar()
{
	CScene* scene = CGame::GetInstance()->GetCurrentScene();
	CStar* star = ((CPlayScene*)scene)->GetStar();

	if (state != GREENBOSS_STATE_DIE)
	{
		if (onGround && star->state != STAR_STATE_HIDE && star->state != STAR_STATE_EXPLOSIVE  //ne dan
			&& star->state != STAR_STATE_PENDING && star->state != STAR_STATE_READY)
		{
			if ((x > star->x && x <= star->x + GREENBOSS_STAR_DODGE_RANGE) || (x < star->x && x >= star->x - GREENBOSS_STAR_DODGE_RANGE)) //o gan sao moi nhay
				Jump();
		}

		// Trung dan :
		if (CheckAABB(star) && star->state != STAR_STATE_HIDE 
			&& star->state != STAR_STATE_EXPLOSIVE && star->state != STAR_STATE_PENDING && star->state != STAR_STATE_READY)
		{
			star->SetState(STAR_STATE_EXPLOSIVE);

			if (life_counts > 1) {
				is_stunning = true;
				stun_start = GetTickCount64();
				life_counts -= 1;
			}
			else
			{
				SetState(GREENBOSS_STATE_DIE);
			}
		}
	}
	

	
}

int CGreenBoss::CheckSideOfStar() // -1 left, 1 right dung de xac dinh huong die
{
	CScene* scene = CGame::GetInstance()->GetCurrentScene();
	CStar* star = ((CPlayScene*)scene)->GetStar();
	if (star->x < x)
		return -1;
	else
		return 1;
}

bool CGreenBoss::onTopOf(CGameObject* object, float equal)
{
	float ol, ot, or , ob;
	object->GetBoundingBox(ol, ot, or , ob);
	float l, t, r, b;
	GetBoundingBox(l, t, r, b);
	if (r >= ol && l <= or && abs(b - ot) < equal)
		return true;
	return false;
}

bool CGreenBoss::onSideOf(CGameObject* object, float equal)
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

void CGreenBoss::SpecialCollisionWithPlayer()
{
	CScene* scene = CGame::GetInstance()->GetCurrentScene();
	CGimmick* player = ((CPlayScene*)scene)->GetPlayer();

	if (player->state != GIMMICK_STATE_DIE && player->getUntouchable() != 1
		&& state != GREENBOSS_STATE_DIE)
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

bool CGreenBoss::IsCollidingWithPlayer()
{
	CScene* scene = CGame::GetInstance()->GetCurrentScene();
	CGimmick* player = ((CPlayScene*)scene)->GetPlayer();

	float l, t, r, b;
	GetBoundingBox(l, t, r, b);
	t -= 7;

	float ol, ot, or , ob;
	player->GetBoundingBox(ol, ot, or , ob);

	if (or >= l && ol <= r
		&& ot >= b && ob <= t)
		return true;
	return false;
}
