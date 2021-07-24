#include "SwordBoss.h"
#include "Utils.h"

CSwordBoss::CSwordBoss()
{
	nx = -1;
	SetState(SWORDBOSS_STATE_WALK);
}

void CSwordBoss::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CGameObject::Update(dt, coObjects);

	vx += ax * dt;
	vy -= SWORDBOSS_GRAVITY * dt;

	LimitPosition();
	DetectPlayer();
	CalculateSpeed();
	CalculateState();
	DetectStar();
	SpecialCollisionWithPlayer();

	onGround = false;
	render_onground = false;

	vector<LPGAMEOBJECT> newCoObjects;
	for (UINT i = 0; i < coObjects->size(); i++)
	{
		if (dynamic_cast<CBrick*>(coObjects->at(i))) newCoObjects.push_back(coObjects->at(i));

		if (dynamic_cast<CBrick*>(coObjects->at(i))) {
			CBrick* brick = dynamic_cast<CBrick*>(coObjects->at(i));
			if (onTopOf(brick,0.5f)) this->render_onground = true;
		}
	}

	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	if (state != SWORDBOSS_STATE_DIE)
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
				x = x0 + min_tx * dx + nx * 0.1f;
				y = y0 + min_ty * dy + ny * 0.1f;
				if (e->nx != 0)
				{
					vx = 0;
				}
				if (e->ny > 0) {
					vy = 0;
					onGround = true;
				}
			}
		}
	}
	// clean up newCoObjects
	for (UINT i = 0; i < newCoObjects.size(); i++) newCoObjects[i] = nullptr;
	// clean up collision events
	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
}

void CSwordBoss::Render()
{
	int ani = 0;
	if (state == SWORDBOSS_STATE_WALK) { //==============WALK
		if (vx == 0 || !render_onground) //khi nhay cung co animation dung yen
		{
			if (nx > 0)
				ani = SWORDBOSS_ANI_IDLE_RIGHT;
			else
				ani = SWORDBOSS_ANI_IDLE_LEFT;
		}
		else
		{
			if (nx > 0)
				ani = SWORDBOSS_ANI_WALK_RIGHT;
			else
				ani = SWORDBOSS_ANI_WALK_LEFT;
		}
	}
	else if (state == SWORDBOSS_STATE_CHOP)
	{
		if (nx > 0)
		{
			ani = SWORDBOSS_ANI_CHOP_RIGHT;
			animation_set->at(ani)->Render(x - 13, y - 2);
			return;
		}
		else
		{
			ani = SWORDBOSS_ANI_CHOP_LEFT;
			animation_set->at(ani)->Render(x - 13, y - 2);
			return;
		}
	}
	else if (state == SWORDBOSS_STATE_STUN)
	{
		if (nx > 0)
		{
			ani = SWORDBOSS_ANI_STUN_RIGHT;
		}
		else
		{
			ani = SWORDBOSS_ANI_STUN_LEFT;
		}
	}
	else if (state == SWORDBOSS_STATE_DIE)
	{
		if (nx > 0)
		{
			ani = SWORDBOSS_ANI_DIE_RIGHT;
		}
		else
		{
			ani = SWORDBOSS_ANI_DIE_LEFT;
		}
	}

	animation_set->at(ani)->Render(x, y);

	//RenderBoundingBox();
}

void CSwordBoss::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case SWORDBOSS_STATE_WALK:
		break;
	case SWORDBOSS_STATE_CHOP:
		chop_start = GetTickCount64();
		break;
	case SWORDBOSS_STATE_STUN:
		stun_start = GetTickCount64();
		ax = 0;
		if (nx < 0) {
			vx = SWORDBOSS_DEFLECT_SPEED_X;
		}
		else {
			vx = -SWORDBOSS_DEFLECT_SPEED_X;
		}
		vy = SWORDBOSS_DEFLECT_SPEED_Y;
		break;
	case SWORDBOSS_STATE_DIE:
		Sound::GetInstance()->Play("SOUND_Effect_84", 0, 1);
		ax = 0;
		if (nx < 0) {
			vx = SWORDBOSS_DEFLECT_SPEED_X;
		}
		else {
			vx = -SWORDBOSS_DEFLECT_SPEED_X;
		}
		vy = SWORDBOSS_DEFLECT_SPEED_Y;
		break;

	}
}

void CSwordBoss::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = x + SWORDBOSS_BBOX_WIDTH;
	b = y - SWORDBOSS_BBOX_HEIGHT;
}

void CSwordBoss::Jump()
{
	CScene* scene = CGame::GetInstance()->GetCurrentScene();
	CGimmick* player = ((CPlayScene*)scene)->GetPlayer();

	if (nx < 0) 
	{
		vx = -SWORDBOSS_JUMP_SPEED_X;
		ax = -SWORDBOSS_ACCELERATION;
	}
	else
	{
		vx = SWORDBOSS_JUMP_SPEED_X;
		ax = SWORDBOSS_ACCELERATION;
	}
	vy = SWORDBOSS_JUMP_SPEED;
}

void CSwordBoss::CalculateSpeed()
{
	CScene* scene = CGame::GetInstance()->GetCurrentScene();
	CGimmick* player = ((CPlayScene*)scene)->GetPlayer();

	float player_center = player->x + GIMMICK_BBOX_WIDTH / 2;
	float boss_center = x + SWORDBOSS_BBOX_WIDTH / 2;

	float player_boss_jump_distance = 0;

	if (crazy) player_boss_jump_distance = SWORDBOSS_PLAYER_CRAZY_CANJUMP_DISTANCE; // nhay lien tuc hon
	else player_boss_jump_distance = SWORDBOSS_PLAYER_CANJUMP_DISTANCE;

	// ===== WALK:
	// Gioi han toc do:
	if (state == SWORDBOSS_STATE_WALK || state == SWORDBOSS_STATE_CHOP) 
	{
		if (vx > SWORDBOSS_WALK_SPEED)
			vx = SWORDBOSS_WALK_SPEED;
		if (vx < -SWORDBOSS_WALK_SPEED)
			vx = -SWORDBOSS_WALK_SPEED;
	}

	//duoi theo gimmick:
	// WALK
	if (state == SWORDBOSS_STATE_WALK && onGround)
	{
		if (nx < 0)
		{
			if (isEqual(player_center + SWORDBOSS_PLAYER_DISTANCE, boss_center, 1.0f)) // so float cang nho thi cang dung dua (khong on dinh) 
			{
				vx = 0;
				ax = 0;
			}
			else if (player_center + SWORDBOSS_PLAYER_DISTANCE < boss_center)
				ax = -SWORDBOSS_ACCELERATION;
			else if (player_center + SWORDBOSS_PLAYER_DISTANCE > boss_center)
				ax = SWORDBOSS_ACCELERATION;
		}
		else
		{
			if (isEqual(player_center - SWORDBOSS_PLAYER_DISTANCE, boss_center, 1.0f))
			{
				vx = 0;
				ax = 0;
			}
			else if (player_center - SWORDBOSS_PLAYER_DISTANCE > boss_center)
				ax = SWORDBOSS_ACCELERATION;
			else if (player_center - SWORDBOSS_PLAYER_DISTANCE < boss_center)
				ax = -SWORDBOSS_ACCELERATION;
		}
	}
	// CHOP duoi theo player
	if (state == SWORDBOSS_STATE_CHOP && onGround)
	{
		if (nx < 0)
		{
			if (player_center + SWORDBOSS_PLAYER_DISTANCE == boss_center)
			{
				vx = 0;
				ax = 0;
			}
			else if (player_center + SWORDBOSS_PLAYER_CHOP_DISTANCE < boss_center)
				ax = -SWORDBOSS_ACCELERATION;
			else if (player_center + SWORDBOSS_PLAYER_CHOP_DISTANCE > boss_center)
				ax = SWORDBOSS_ACCELERATION;
		}
		else
		{
			if (player_center + SWORDBOSS_PLAYER_DISTANCE == boss_center)
			{
				vx = 0;
				ax = 0;
			}
			else if (player_center - SWORDBOSS_PLAYER_CHOP_DISTANCE > boss_center)
				ax = SWORDBOSS_ACCELERATION;
			else if (player_center - SWORDBOSS_PLAYER_CHOP_DISTANCE < boss_center)
				ax = -SWORDBOSS_ACCELERATION;
		}
	}

	//neu gimmick lai gan qua thi chop, neu o ria map thi nhay / neu crazy thi chi nhay (distance xa hon)
	if (state == SWORDBOSS_STATE_WALK && onGround)
	{
		if (nx < 0)
		{
			if (player_center + player_boss_jump_distance >= boss_center) // vx==0 de tranh truong hop lai gan chem ko nhay
			{
				if (x + SWORDBOSS_BBOX_WIDTH >= SWORDBOSS_MAP_END_RIGHT - 2) // o ria map thi nhay
					Jump();
				else  // Neu crazy thi nhay lien tuc neu khong thi chem
				{
					if (!crazy)
					{
						if (can_chop)
							SetState(SWORDBOSS_STATE_CHOP);
					}
					else
						Jump();
				}
			}
		}
		else
		{
			if (player_center - player_boss_jump_distance <= boss_center)
			{
				if (x <= SWORDBOSS_MAP_END_LEFT + 2)
					Jump();
				else
				{
					if (!crazy)
					{
						if (can_chop)
							SetState(SWORDBOSS_STATE_CHOP);
					}
					else
						Jump();
				}
			}
		}
	}
}

void CSwordBoss::DetectPlayer()
{
	CScene* scene = CGame::GetInstance()->GetCurrentScene();
	CGimmick* player = ((CPlayScene*)scene)->GetPlayer();

	// Duoi theo gimmick:
	if (player->x + GIMMICK_BBOX_WIDTH/2 < x + SWORDBOSS_BBOX_WIDTH/2)
		nx = -1;
	else 
		nx = 1;

}


void CSwordBoss::DetectStar()
{
	CScene* scene = CGame::GetInstance()->GetCurrentScene();
	CStar* star = ((CPlayScene*)scene)->GetStar();

	float star_center = star->x + GIMMICK_BBOX_WIDTH / 2;
	float boss_center = x + SWORDBOSS_BBOX_WIDTH / 2;

	if (state != SWORDBOSS_STATE_DIE)
	{
		if (star->state == STAR_STATE_WALKING_LEFT || star->state == STAR_STATE_WALKING_RIGHT) // do dan
		{
			if ((boss_center > star_center && boss_center <= star_center + SWORDBOSS_BLOCK_RANGE)
				|| (boss_center < star_center && boss_center >= star_center - SWORDBOSS_BLOCK_RANGE)) //o gan sao moi chop
				if (can_chop) { 
					SetState(SWORDBOSS_STATE_CHOP); }
		}

		// Trung dan :
		if (state == SWORDBOSS_STATE_CHOP && CheckSwordBlockStar()) // Do dan bang kiem
		{
			if(star->state == STAR_STATE_WALKING_LEFT || star->state == STAR_STATE_WALKING_RIGHT)
				star->SetState(STAR_STATE_EXPLOSIVE);
		}
		else
		{
			if (CheckAABB(star) && (star->state == STAR_STATE_WALKING_LEFT || star->state == STAR_STATE_WALKING_RIGHT))
			{
				star->SetState(STAR_STATE_EXPLOSIVE);

				if (life_counts == 1)
				{
					SetState(SWORDBOSS_STATE_DIE);
					CreateSword();
				}
				else
				{
					SetState(SWORDBOSS_STATE_STUN);
					life_counts -= 1;
				}

				if (life_counts == 3) // dinh 2 vien dan la boss se crazy
				{
					crazy = true;
				}
				else
					crazy = false;

			}
		}
	}



}

int CSwordBoss::CheckSideOfStar() // -1 left, 1 right dung de xac dinh huong die
{
	CScene* scene = CGame::GetInstance()->GetCurrentScene();
	CStar* star = ((CPlayScene*)scene)->GetStar();
	if (star->x < x)
		return -1;
	else
		return 1;
}

bool CSwordBoss::onTopOf(CGameObject* object, float equal)
{
	float ol, ot, or , ob;
	object->GetBoundingBox(ol, ot, or , ob);
	float l, t, r, b;
	GetBoundingBox(l, t, r, b);
	if (r >= ol && l <= or && abs(b - ot) < equal)
		return true;
	return false;
}

bool CSwordBoss::onSideOf(CGameObject* object, float equal)
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

void CSwordBoss::SpecialCollisionWithPlayer()
{
	CScene* scene = CGame::GetInstance()->GetCurrentScene();
	CGimmick* player = ((CPlayScene*)scene)->GetPlayer();
	CStar* star = ((CPlayScene*)scene)->GetStar();

	if (player->state != GIMMICK_STATE_DIE && player->getUntouchable() != 1
		&& state != SWORDBOSS_STATE_DIE)
	{
		if (CheckAABB(player))
		{
			if (x < player->x)
			{
				//player->vx = GIMMICK_DEFLECT_SPEED_X;
				//player->nx = 1.0;
			}
			else
			{
				//player->vx = -GIMMICK_DEFLECT_SPEED_X;
				//player->nx = -1.0;
			}

			player->SetState(GIMMICK_STATE_STUN);
			player->StartUntouchable();

			if (star->state == STAR_STATE_READY)
				star->Shot();
		}
	}
}

bool CSwordBoss::CheckSwordBlockStar()
{
	CScene* scene = CGame::GetInstance()->GetCurrentScene();
	CStar* star = ((CPlayScene*)scene)->GetStar();

	float l, t, r, b;
	GetBoundingBox(l, t, r, b);
	b += 8; // khoang cach tu sword xuong chan
	if (nx < 0)
	{
		r = l + SWORDBOSS_SWORD_BBOX_WIDTH;
	}
	else
	{
		l = r - SWORDBOSS_SWORD_BBOX_WIDTH;
	}

	float ol, ot, or , ob;
	star->GetBoundingBox(ol, ot, or , ob);

	if (or >= l && ol <= r
		&& ot >= b && ob <= t)
		return true;
	return false;
}

bool CSwordBoss::isEqual(float a, float b, float equal)
{
	if (abs(a - b) <= equal)
		return true;
	return false;
}

void CSwordBoss::LimitPosition()
{
	if (vx != 0)
	{
		if (x + SWORDBOSS_BBOX_WIDTH >= SWORDBOSS_MAP_END_RIGHT && ax > 0) //fixed loi ko the truy duoi tiep
		{
			vx = 0;
		}
		if (x <= SWORDBOSS_MAP_END_LEFT && ax < 0)
		{
			vx = 0;
		}
	}
}

void CSwordBoss::CalculateState()
{
	// tinh thoi gian chop
	if (state == SWORDBOSS_STATE_CHOP && GetTickCount64() - chop_start >= SWORDBOSS_CHOP_TIME)
	{
		SetState(SWORDBOSS_STATE_WALK);
		chop_start = 0;
		if (can_chop) {
			can_chop = false;
			canchop_start = GetTickCount64();
		}
	}
	 
	//tinh thoi gian giua cac cu chop
	if (state == SWORDBOSS_STATE_WALK && GetTickCount64() - canchop_start >= SWORDBOSS_CHOP_COOLDOWN) // cooldown giua cac chop
	{
		if(!can_chop)
			can_chop = true;
	}

	if (state == SWORDBOSS_STATE_WALK && GetTickCount64() - autochop_timing >= SWORDBOSS_TIME_BETWEEN_AUTOCHOPS) // thoi gian auto chop
	{
		SetState(SWORDBOSS_STATE_CHOP);
		autochop_timing = GetTickCount64();
	}

	if (state == SWORDBOSS_STATE_STUN && GetTickCount64() - stun_start >= 300)
	{
		SetState(SWORDBOSS_STATE_WALK);
		stun_start = 0;
	}
}

void CSwordBoss::CreateSword()
{
	CSword* sword = new CSword(x + (SWORDBOSS_BBOX_WIDTH - SWORD_BBOX_WIDTH) / 2, y - SWORDBOSS_BBOX_HEIGHT + SWORD_BBOX_HEIGHT + 2);

	//Animation set cho quai
	CAnimationSets* animation_sets = CAnimationSets::GetInstance();
	LPANIMATION_SET ani_set = animation_sets->Get(SWORD_ANISET_ID);
	sword->SetAnimationSet(ani_set);

	//them vao list object cua playscene
	CScene* scene = CGame::GetInstance()->GetCurrentScene();
	((CPlayScene*)scene)->PushBackObj(sword);
}

