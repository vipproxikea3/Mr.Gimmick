#include "ElectricBlackEnemy.h"
#include "Sewer.h"

CElectricBlackEnemy::CElectricBlackEnemy() {
	this->appear = false;
	SetState(ELECTRIC_BLACKENEMY_STATE_STOP);
}

void CElectricBlackEnemy::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (this->GetState() == ELECTRIC_BLACKENEMY_STATE_DIE && !CGame::GetInstance()->InCamera(this) && CGame::GetInstance()->InLargeCamera(this) && this->visible == true) {
		this->visible = false;
	}
	if (!visible)
		return;

	CGameObject::Update(dt, coObjects);

	if (appear)
	{
		vx += ax * dt;
		vy -= ELECTRIC_BLACKENEMY_GRAVITY * dt;
		CalculateSpeed();
		DetectPlayer();
		SpecialCollisionWithPlayer();
		DetectStar();
	}

	vector<LPGAMEOBJECT> newCoObjects;
	for (UINT i = 0; i < coObjects->size(); i++)
	{
		if (dynamic_cast<CBrick*>(coObjects->at(i))) newCoObjects.push_back(coObjects->at(i));

		if (dynamic_cast<CSewer*>(coObjects->at(i))) {
			CSewer* sewer = dynamic_cast<CSewer*>(coObjects->at(i));
			if (CheckAABB(sewer) && sewer->type != SEWER_TYPE_1 && sewer->type != SEWER_TYPE_2) {
				visible = false;
			}
		}
	}

	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	if (this->appear && this->state != ELECTRIC_BLACKENEMY_STATE_DIE) {
		CalcPotentialCollisions(&newCoObjects, coEvents);
	}

	CGimmick* gimmick = ((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->GetPlayer();

	if (!this->appear) {
		if (gimmick->x - x <= DISTANCE_X && y - gimmick->y <= DISTANCE_Y) {
			this->appear = true;
			SetState(ELECTRIC_BLACKENEMY_STATE_FALL);
			ax = ELECTRIC_BLACKENEMY_ACCELERATION;
		}
	}
	
	if (this->state == ELECTRIC_BLACKENEMY_STATE_WALK && !this->shocking) {
		if (GetTickCount64() - start_shock >= TIME_BETWEEN_TWO_SHOCKING) {
			this->shocking = true;
			SetState(ELECTRIC_BLACKENEMY_STATE_SHOCK);
			StartShock();
		}
	}

	if (this->shocking) {
		if (GetTickCount64() - start_shock >= TIME_SHOCKING) {
			this->shocking = false;
			if (this->shocking_large)
				this->shocking_large = false;
			SetState(ELECTRIC_BLACKENEMY_STATE_WALK);

		}
	}

	if (coEvents.size() == 0)
	{
		if(this->state != ELECTRIC_BLACKENEMY_STATE_FALL)
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

			if (dynamic_cast<CBrick*>(e->obj))
			{
				CBrick* brick = dynamic_cast<CBrick*>(e->obj);
				if (e->ny > 0) {
					this->vy = 0;
					x = x0 + min_tx * dx + nx * 0.2f;
					y = y0 + min_ty * dy + ny * 0.2f;
					if (this->state != ELECTRIC_BLACKENEMY_STATE_DIE && this->state != ELECTRIC_BLACKENEMY_STATE_STOP) {
						if (/*!this->carry_player &&*/ this->state != ELECTRIC_BLACKENEMY_STATE_SHOCK) {
							SetState(ELECTRIC_BLACKENEMY_STATE_WALK);
							y = y0 + min_ty * dy + ny * 0.1f;
						}
					}
					if (this->state == ELECTRIC_BLACKENEMY_STATE_STOP) {
						y = y0 + min_ty * dy + ny * 0.1f;
					}
				}
			}
		}
	}
	// clean up newCoObjects
	for (UINT i = 0; i < newCoObjects.size(); i++) newCoObjects[i] = nullptr;
	// clean up collision events
	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
}

void CElectricBlackEnemy::Render()
{
	if (!visible)
		return;

	int ani = 2;

	if (this->state == ELECTRIC_BLACKENEMY_STATE_DISAPPEAR)
		return;
	if (appear) {
		if (state == ELECTRIC_BLACKENEMY_STATE_STOP) {
			if (nx > 0) {
				ani = ELECTRIC_BLACKENEMY_ANI_STOP_RIGHT;
			}
			else {
				ani = ELECTRIC_BLACKENEMY_ANI_STOP_LEFT;
			}

		}
		if (state == ELECTRIC_BLACKENEMY_STATE_WALK) {
			if (ax > 0)
				ani = ELECTRIC_BLACKENEMY_ANI_WALKING_RIGHT;
			else
				ani = ELECTRIC_BLACKENEMY_ANI_WALKING_LEFT;
		}
		if (state == ELECTRIC_BLACKENEMY_STATE_SHOCK) {
			if (ax > 0)
				ani = ELECTRIC_BLACKENEMY_ANI_SHOCKING_RIGHT;
			else
				ani = ELECTRIC_BLACKENEMY_ANI_SHOCKING_LEFT;
		}
		if (state == ELECTRIC_BLACKENEMY_STATE_DIE) {
			if (nx > 0)
				ani = ELECTRIC_BLACKENEMY_ANI_DIE_RIGHT;
			else
				ani = ELECTRIC_BLACKENEMY_ANI_DIE_LEFT;
		}

	}
	if(appear && (state == ELECTRIC_BLACKENEMY_STATE_WALK || state == ELECTRIC_BLACKENEMY_STATE_SHOCK))
		animation_set->at(ani)->Render(x, y + 7);
	else
		animation_set->at(ani)->Render(x, y);
	
	if (shocking) {
		if (shocking_large) {
			if (ax > 0) {
				animation_set->at(ELECTRIC_BLACKENEMY_ANI_THUNDER_1_RIGHT)->Render(x + 3, y + 16);
				animation_set->at(ELECTRIC_BLACKENEMY_ANI_THUNDER_2_RIGHT)->Render(x - 7, y + 16);
			}

			else {
				animation_set->at(ELECTRIC_BLACKENEMY_ANI_THUNDER_1_LEFT)->Render(x, y + 16);
				animation_set->at(ELECTRIC_BLACKENEMY_ANI_THUNDER_2_LEFT)->Render(x - 7, y + 16);
			}
		}
		else {
			if (ax > 0)
				animation_set->at(ELECTRIC_BLACKENEMY_ANI_THUNDER_1_RIGHT)->Render(x + 3, y + 16);
			else
				animation_set->at(ELECTRIC_BLACKENEMY_ANI_THUNDER_1_LEFT)->Render(x, y + 16);
		}
	}

	//RenderBoundingBox();
}

void CElectricBlackEnemy::SetState(int state)
{
	CGameObject::SetState(state);
	Sound::GetInstance()->Stop("SOUND_Effect_1");
	switch (state)
	{
	case ELECTRIC_BLACKENEMY_STATE_DIE:
		ax = 0;
		if (CheckSideOfStar() == -1) {
			vx = ELECTRIC_BLACKENEMY_DEFLECT_SPEED_X;
		}
		else {
			vx = -ELECTRIC_BLACKENEMY_DEFLECT_SPEED_X;
		}
		vy = ELECTRIC_BLACKENEMY_DEFLECT_SPEED_Y;
		break;
	case ELECTRIC_BLACKENEMY_STATE_WALK:
		if (nx > 0)
			ax = ELECTRIC_BLACKENEMY_ACCELERATION;
		else  ax = -ELECTRIC_BLACKENEMY_ACCELERATION;
		break;
	case ELECTRIC_BLACKENEMY_STATE_FALL:
		this->vx = 0;
		vy -= ELECTRIC_BLACKENEMY_GRAVITY * dt;
		break;
	case ELECTRIC_BLACKENEMY_STATE_STOP:
		ax = 0;
		if (this->appear)
			this->vx = 0;
		else
		{
			this->vx = 0;
			this->vy = 0;
		}
		break;
	case ELECTRIC_BLACKENEMY_STATE_SHOCK:
		this->vx = 0;
		Sound::GetInstance()->Play("SOUND_Effect_1", 0,1);
		//ax = 0;
		break;
	}
}

void CElectricBlackEnemy::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	if (!visible)
		return;
	l = x;
	t = y - 1;
	r = x + ELECTRIC_BLACKENEMY_STOP_BBOX_WIDTH;
	b = y - ELECTRIC_BLACKENEMY_STOP_BBOX_HEIGHT + 1;
}

void CElectricBlackEnemy::CalculateSpeed() {

	// ===== WALK:
	if (state == ELECTRIC_BLACKENEMY_STATE_WALK && vx > ELECTRIC_BLACKENEMY_WALK_SPEED)
		vx = ELECTRIC_BLACKENEMY_WALK_SPEED;
	if (state == ELECTRIC_BLACKENEMY_STATE_WALK && vx < -ELECTRIC_BLACKENEMY_WALK_SPEED)
		vx = -ELECTRIC_BLACKENEMY_WALK_SPEED;
}

void CElectricBlackEnemy::TurnAroundSlowly()
{
	if (state == ELECTRIC_BLACKENEMY_STATE_WALK)
	{
		if (vx > 0)
			ax = -ELECTRIC_BLACKENEMY_ACCELERATION;
		if (vx < 0)
			ax = ELECTRIC_BLACKENEMY_ACCELERATION;
	}
}

void CElectricBlackEnemy::DetectPlayer()
{
	CGimmick* player = ((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->GetPlayer();

	//Reset dem neu ngang hang hoac o duoi gimmick
	if (this->state == ELECTRIC_BLACKENEMY_STATE_STOP && this->state != ELECTRIC_BLACKENEMY_STATE_DIE)
	{
		if ((player->y >= this->y + ELECTRIC_BLACKENEMY_BBOX_HEIGHT)
			|| (player->x <= this->x - ELECTRIC_BLACKENEMY_STOP_BBOX_WIDTH - 0.5f && this->y < player->x)
			|| (player->x >= this->x + ELECTRIC_BLACKENEMY_STOP_BBOX_WIDTH + 0.5f && this->y < player->x)) {
			SetState(ELECTRIC_BLACKENEMY_STATE_WALK);
			this->canTurnAround = true;
			StartShock();
		}
	}
	// Duoi theo gimmick
	if (state == ELECTRIC_BLACKENEMY_STATE_WALK)
	{
		if (this->canTurnAround && this->x > player->x + PLAYER_MAX_RANGE_WALK && this->ax > 0)
		{
			TurnAroundSlowly();
			this->canTurnAround = false;
			nx = -1;
		}
		if (this->canTurnAround && this->x < player->x - PLAYER_MAX_RANGE_WALK && this->ax < 0)
		{
			TurnAroundSlowly();
			this->canTurnAround = false;
			nx = 1;
		}
	}
	//reset quay dau
	if (state != ELECTRIC_BLACKENEMY_STATE_STOP && !player->onEnemy && state != ELECTRIC_BLACKENEMY_STATE_DIE) {
		if (!this->canTurnAround && (this->x >= player->x - (PLAYER_MAX_RANGE_FLY - 1)) && (this->x <= player->x + (PLAYER_MAX_RANGE_FLY - 1))) //reset turn around, can dam bao so +- < PLAYER_MAX_RANGE_FLY
			this->canTurnAround = true;
	}
		
}

void CElectricBlackEnemy::DetectStar() {
	CStar* star = ((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->GetStar();

	if (CheckAABB(star) && this->state != ELECTRIC_BLACKENEMY_STATE_DIE
		&& star->state != STAR_STATE_HIDE && star->state != STAR_STATE_EXPLOSIVE
		&& star->state != STAR_STATE_PENDING && star->state != STAR_STATE_READY)
	{
		if (this->state == ELECTRIC_BLACKENEMY_STATE_STOP) {
			SetState(ELECTRIC_BLACKENEMY_STATE_DIE);
			star->SetState(STAR_STATE_EXPLOSIVE);
		}
			
		if (this->state == ELECTRIC_BLACKENEMY_STATE_SHOCK) {
			this->shocking_large = true;
			star->SetState(STAR_STATE_EXPLOSIVE);
		}
		if(this->state == ELECTRIC_BLACKENEMY_STATE_WALK) {
			this->shocking = true;
			this->shocking_large = true;
			star->SetState(STAR_STATE_EXPLOSIVE);
			SetState(ELECTRIC_BLACKENEMY_STATE_SHOCK);
			StartShock();
		}
	}
}

int CElectricBlackEnemy::CheckSideOfStar() // -1 left, 1 right
{
	CStar* star = ((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->GetStar();
	if (star->x < this->x)
		return -1;
	else
		return 1;
}

void CElectricBlackEnemy::SpecialCollisionWithPlayer()
{
	CGimmick* player = ((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->GetPlayer();

	if (player->state != GIMMICK_STATE_DIE && player->getUntouchable() != 1
		&& state != ELECTRIC_BLACKENEMY_STATE_DIE)
	{
		if (IsCollidingWithPlayer())
		{
			if (this->x < player->x)
			{
				player->nx = 1.0;
			}
			else
			{
				player->nx = -1.0;
			}

			player->SetState(GIMMICK_STATE_STUN);
			player->StartUntouchable();
		}
	}
}

bool CElectricBlackEnemy::IsCollidingWithPlayer()
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